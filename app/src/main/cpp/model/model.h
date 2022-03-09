#ifndef MODEL_H
#define MODEL_H

#include <glm/gtc/matrix_transform.hpp>

//包含Assimp对应的头文件
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>
#include "mesh.h"
#include <LogUtils.h>

using namespace std;

// 参考链接： https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/

//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

// 一个模型（Model）包含许多个网格（Mesh），各个 Mesh 独立渲染共同组成整个 Model
class Model {
public:
    /*  Model Data */

    // 接下来我们将所有加载过的纹理储存在另一个vector中，在模型类的顶部声明为一个私有变量：
    vector<Texture> textures_loaded;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    //模型所包含的网格
    vector<Mesh> meshes;
    //模型文件所在目录
    string directory;
    glm::vec3 maxXyz, minXyz;
    bool gammaCorrection;
    bool hasTexture;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) :
            gammaCorrection(gamma),
            hasTexture(false) {
        loadModel(path);
    }

    // 渲染模型，即依次渲染各个网格：Draw函数没有什么特别之处，基本上就是遍历了所有网格，并调用它们各自的Draw函数。
    // draws the model, and thus all its meshes
    void Draw(Shader shader) {
        // 绘制模型就是遍历每个 Mesh 进行绘制：
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    float GetMaxViewDistance() {
        glm::vec3 vec3 = (abs(minXyz) + abs(maxXyz)) / 2.0f;
        float maxDis = fmax(vec3.x, fmax(vec3.y, vec3.z));
        LOGD("Model::GetMaxViewDistance maxDis=%f", maxDis)
        return maxDis;
    }

    glm::vec3 GetAdjustModelPosVec() {
        glm::vec3 vec3 = (minXyz + maxXyz) / 2.0f;
        LOGD("Model::GetAdjustModelPosVec vec3(%f, %f, %f)", vec3.x, vec3.y, vec3.z)
        return (minXyz + maxXyz) / 2.0f;
    }

    bool ContainsTextures() {
        return hasTexture;
    }

    // 销毁模型的所有网格
    void Destroy() {
        for (Mesh &mesh : meshes) {
            mesh.Destroy();
        }
    }

private:
    /*  Functions   */

    // 加载模型
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path) {
        // Assimp很棒的一点在于，它抽象掉了加载不同文件格式的所有技术细节，只需要一行代码就能完成所有的工作
        // 使用 Assimp 加载 3D 模型比较简单，最终模型被加载到一个 Assimp 中定义的 aiScene 对象中，
        // aiScene 对象除了包含网格和材质，还包含一个 aiNode 对象（根节点），
        // 然后我们还需要遍历各个子节点的网格。
        // read file via ASSIMP
        // 首先声明了Assimp命名空间内的一个Importer，之后调用了它的ReadFile函数。
        Assimp::Importer importer;
        // 参数
        // aiProcess_Triangulate  通过设定aiProcess_Triangulate，我们告诉Assimp，如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形。
        // aiProcess_FlipUVs  将在处理的时候翻转y轴的纹理坐标（你可能还记得我们在纹理教程中说过，在OpenGL中大部分的图像的y轴都是反的，所以这个后期处理选项将会修复这个）。

        // 其它一些比较有用的选项有：
        //   aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
        //   aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
        //   aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。

        // Assimp提供了很多有用的后期处理指令，你可以在这里链接中找到全部的指令。  http://assimp.sourceforge.net/lib_html/postprocess_8h.html
        // 实际上使用Assimp加载模型是非常容易的（你也可以看到）。困难的是之后使用返回的场景对象将加载的数据转换到一个Mesh对象的数组。
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                       aiProcess_CalcTangentSpace);
        DEBUG_LOGCATE()
        // check for errors

        // 在我们加载了模型之后，我们会检查场景和其根节点不为null，
        // 并且检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的。
        // 如果遇到了任何错误，我们都会通过导入器的GetErrorString函数来报告错误并返回。
        // 我们也获取了文件路径的目录路径。
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) // if is Not Zero
        {
            LOGD("Model::loadModel path=%s, assimpError=%s", path.c_str(),
                 importer.GetErrorString())
            return;
        }
        DEBUG_LOGCATE()

        // 如果什么错误都没有发生，我们希望处理场景中的所有节点，所以我们将第一个节点（根节点）传入了递归的processNode函数。
        // 因为每个节点（可能）包含有多个子节点，我们希望首先处理参数中的节点，再继续处理该节点所有的子节点，以此类推。
        // 这正符合一个递归结构，所以我们将定义一个递归函数。
        // 递归函数在做一些处理之后，使用不同的参数递归调用这个函数自身，直到某个条件被满足停止递归。
        // 在我们的例子中退出条件(Exit Condition)是所有的节点都被处理完毕。

        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        //处理节点
        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // 处理 aiScene 对象包含的节点和子节点
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene) {
        // process each mesh located at the current node
        // 处理节点所有的网格（如果有的话）
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).

            // 我们首先检查每个节点的网格索引，并索引场景的mMeshes数组来获取对应的网格。
            // 返回的网格将会传递到processMesh函数中，它会返回一个Mesh对象，我们可以将它存储在meshes列表/vector。
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            if (mesh != nullptr)
                meshes.push_back(processMesh(mesh, scene));
        }
        DEBUG_LOGCATE()
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        // 接下来对它的子节点重复这一过程
        // 所有网格都被处理之后，我们会遍历节点的所有子节点，并对它们调用相同的processMesh函数。
        // 当一个节点不再有任何子节点之后，这个函数将会停止执行。
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            //递归处理所有节点
            processNode(node->mChildren[i], scene);
        }

    }

    void updateMaxMinXyz(glm::vec3 pos) {
        maxXyz.x = pos.x > maxXyz.x ? pos.x : maxXyz.x;
        maxXyz.y = pos.y > maxXyz.y ? pos.y : maxXyz.y;
        maxXyz.z = pos.z > maxXyz.z ? pos.z : maxXyz.z;

        minXyz.x = pos.x < minXyz.x ? pos.x : minXyz.x;
        minXyz.y = pos.y < minXyz.y ? pos.y : minXyz.y;
        minXyz.z = pos.z < minXyz.z ? pos.z : minXyz.z;
    }

    // 生成网格 Mesh
    // 处理网格的过程主要有三部分：获取所有的顶点数据，获取它们的网格索引，并获取相关的材质数据。
    // 处理后的数据将会储存在三个vector当中，我们会利用它们构建一个Mesh对象，并返回它到函数的调用者那里。
    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;


        // Walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            // 处理顶点坐标、法线和纹理坐标

            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions 处理顶点坐标
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            updateMaxMinXyz(vector);

            // normals 处理法线
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;


            // texture coordinates 处理纹理坐标
            // 纹理坐标的处理也大体相似，但Assimp允许一个模型在一个顶点上有最多8个不同的纹理坐标，
            // 我们不会用到那么多，我们只关心第一组纹理坐标。我们同样也想检查网格是否真的包含了纹理坐标（可能并不会一直如此）
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?   网格是否有纹理坐标？
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }

        // 处理顶点索引
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        // Assimp的接口定义了每个网格都有一个面(Face)数组，每个面代表了一个图元，
        // 在我们的例子中（由于使用了aiProcess_Triangulate选项）它总是三角形。
        // 一个面包含了多个索引，它们定义了在每个图元中，我们应该绘制哪个顶点，并以什么顺序绘制，
        // 所以如果我们遍历了所有的面，并储存了面的索引到indices这个vector中就可以了。
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }


        // 所有的外部循环都结束了，我们现在有了一系列的顶点和索引数据，它们可以用来通过glDrawElements函数来绘制网格。
        // 然而，为了结束这个话题，并且对网格提供一些细节，我们还需要处理网格的材质。


        // 处理材质
        // process materials

        // 和节点一样，一个网格只包含了一个指向材质对象的索引。
        // 如果想要获取网格真正的材质，我们还需要索引场景的mMaterials数组。
        // 网格材质索引位于它的mMaterialIndex属性中，我们同样可以用它来检测一个网格是否包含有材质：
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN


            // 我们首先从场景的mMaterials数组中获取aiMaterial对象。
            // 接下来我们希望加载网格的漫反射和/或镜面光贴图。
            // 一个材质对象的内部对每种纹理类型都存储了一个纹理位置数组。
            // 不同的纹理类型都以aiTextureType_为前缀。
            // 我们使用一个叫做loadMaterialTextures的工具函数来从材质中获取纹理。
            // 这个函数将会返回一个Texture结构体的vector，我们将在模型的textures vector的尾部之后存储它。

            // 1. diffuse maps
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                               "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // 2. specular maps
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                                "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // 3. normal maps
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                                   "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // 4. height maps
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                                   "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // 创建纹理并加载图像数据
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.

    // loadMaterialTextures函数遍历了给定纹理类型的所有纹理位置，
    // 获取了纹理的文件位置，并加载并和生成了纹理，将信息储存在了一个Vertex结构体中。
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
        DEBUG_LOGCATE()
        vector<Texture> textures;
        // 我们首先通过GetTextureCount函数检查储存在材质中纹理的数量，这个函数需要一个纹理类型。
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            // 我们会使用GetTexture获取每个纹理的文件位置，它会将结果储存在一个aiString中。
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;

            // 我们希望将纹理的路径与储存在textures_loaded这个vector中的所有纹理进行比较，
            // 看看当前纹理的路径是否与其中的一个相同。
            // 如果是的话，则跳过纹理加载/生成的部分，直接使用定位到的纹理结构体为网格的纹理。
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            // 如果纹理还没有被加载，则加载它
            if (!skip) {   // if texture hasn't been loaded already, load it
                Texture texture;
                // 我们接下来使用另外一个叫做TextureFromFile的工具函数，它将会（用stb_image.h）加载一个纹理并返回该纹理的ID。
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                // 添加到已加载的纹理中
                // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    // 在 native 层加载纹理的时候，我们使用 OpenCV 对图片进行解码，然后生成纹理对象：
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false) {
        string filename = string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = nullptr;

        // load the texture using OpenCV
        LOGD("TextureFromFile Loading texture %s", filename.c_str())

        // 使用 OpenCV 对图片进行解码
        cv::Mat textureImage = cv::imread(filename);
        if (!textureImage.empty()) {
            hasTexture = true;
            // OpenCV 默认解码成 BGR 格式，这里转换为 RGB
            // opencv reads textures in BGR format, change to RGB for GL
            cv::cvtColor(textureImage, textureImage, CV_BGR2RGB);
            // opencv reads image from top-left, while GL expects it from bottom-left
            // vertically flip the image
            //cv::flip(textureImage, textureImage, 0);

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureImage.cols,
                         textureImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         textureImage.data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            GO_CHECK_GL_ERROR()
        } else {
            LOGD("TextureFromFile Texture failed to load at path: %s", path)
        }

        return textureID;
    }
};

#endif
