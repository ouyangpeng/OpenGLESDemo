#include <GLES3/gl3.h>

#define ES_PI  (3.14159265f)

/**
 * 为一个球体生成集合形状。为顶点数据分配内存并将结果保存在数组中。为 GL_TRIANGLE_STRIP 生成索引列表
 * @param numSlices 球体中的垂直和水平切片数量
 * @param radius
 * @param vertices      如果不为NULL，则包含float3 位置 数组
 * @param normals       如果不为NULL，则包含float3 法线 数组
 * @param texCoords     如果不为NULL，则包含float2 texCoords 数组
 * @param indices       如果不为NULL，则包含 三角形条带索引 数组
 * @return  以GL_TRIANGLE_STRIP的形式渲染缓冲区时需要的索引数量（如果索引数组不为NULL，则为其中保存的索引数量）
 */
int esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,
                             GLfloat **texCoords, GLuint **indices );

/**
 * 为立方体生成集合形状。为顶点数据分配内存并将结果保存在数组中。为 GL_TRIANGLE_STRIP 生成索引列表
 * @param scale         立方体的大小，单位立方体为1.0
 * @param vertices      如果不为NULL，则包含float3 位置 数组
 * @param normals       如果不为NULL，则包含float3 法线 数组
 * @param texCoords     如果不为NULL，则包含float2 texCoords 数组
 * @param indices       如果不为NULL，则包含 三角形条带索引 数组
 * @return 以GL_TRIANGLE_STRIP的形式渲染缓冲区时需要的索引数量（如果索引数组不为NULL，则为其中保存的索引数量）
 */
int esGenCube ( float scale, GLfloat **vertices, GLfloat **normals,
                           GLfloat **texCoords, GLuint **indices );

/**
 * 生成由三角形组成的方格网。为顶点数据分配内存并将结果保存在数组中。为 GL_TRIANGLE_STRIP 生成索引列表
 * @param size          立方体的大小，单位立方体为1.0
 * @param vertices      如果不为NULL，则包含float3 位置 数组
 * @param indices       如果不为NULL，则包含 三角形条带索引 数组
 * @return  以GL_TRIANGLE_STRIP的形式渲染缓冲区时需要的索引数量（如果索引数组不为NULL，则为其中保存的索引数量）
 */
int esGenSquareGrid ( int size, GLfloat **vertices, GLuint **indices );
