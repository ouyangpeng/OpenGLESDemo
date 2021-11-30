#include <cstdlib>
#include <cstring>
#include <cmath>
#include "esShapes.h"

int esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,
                             GLfloat **texCoords, GLuint **indices )
{
   int i;
   int j;
   int numParallels = numSlices / 2;
   int numVertices = ( numParallels + 1 ) * ( numSlices + 1 );
   int numIndices = numParallels * numSlices * 6;
   float angleStep = ( 2.0f * ES_PI ) / ( ( float ) numSlices );

   // Allocate memory for buffers
   if ( vertices != nullptr )
   {
      *vertices = (GLfloat *)malloc ( sizeof ( GLfloat ) * 3 * numVertices );
   }

   if ( normals != nullptr )
   {
      *normals = (GLfloat *)malloc ( sizeof ( GLfloat ) * 3 * numVertices );
   }

   if ( texCoords != nullptr )
   {
      *texCoords = (GLfloat *)malloc ( sizeof ( GLfloat ) * 2 * numVertices );
   }

   if ( indices != nullptr )
   {
      *indices = (GLuint *)malloc ( sizeof ( GLuint ) * numIndices );
   }

   for ( i = 0; i < numParallels + 1; i++ )
   {
      for ( j = 0; j < numSlices + 1; j++ )
      {
         int vertex = ( i * ( numSlices + 1 ) + j ) * 3;

         if ( vertices )
         {
            ( *vertices ) [vertex + 0] = radius * sinf ( angleStep * ( float ) i ) *
                                         sinf ( angleStep * ( float ) j );
            ( *vertices ) [vertex + 1] = radius * cosf ( angleStep * ( float ) i );
            ( *vertices ) [vertex + 2] = radius * sinf ( angleStep * ( float ) i ) *
                                         cosf ( angleStep * ( float ) j );
         }

         if ( normals )
         {
            ( *normals ) [vertex + 0] = ( *vertices ) [vertex + 0] / radius;
            ( *normals ) [vertex + 1] = ( *vertices ) [vertex + 1] / radius;
            ( *normals ) [vertex + 2] = ( *vertices ) [vertex + 2] / radius;
         }

         if ( texCoords )
         {
            int texIndex = ( i * ( numSlices + 1 ) + j ) * 2;
            ( *texCoords ) [texIndex + 0] = ( float ) j / ( float ) numSlices;
            ( *texCoords ) [texIndex + 1] = ( 1.0f - ( float ) i ) / ( float ) ( numParallels - 1 );
         }
      }
   }

   // Generate the indices
   if ( indices != nullptr )
   {
      GLuint *indexBuf = ( *indices );

      for ( i = 0; i < numParallels ; i++ )
      {
         for ( j = 0; j < numSlices; j++ )
         {
            *indexBuf++  = i * ( numSlices + 1 ) + j;
            *indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + j;
            *indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );

            *indexBuf++ = i * ( numSlices + 1 ) + j;
            *indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );
            *indexBuf++ = i * ( numSlices + 1 ) + ( j + 1 );
         }
      }
   }

   return numIndices;
}

int esGenCube(float scale, GLfloat **vertices,
        GLfloat **normals, GLfloat **texCoords, GLuint **indices) {
   int i;
   int numVertices = 24;
   int numIndices = 36;

   GLfloat cubeVerts[] =
           {
                   -0.5f, -0.5f, -0.5f,
                   -0.5f, -0.5f, 0.5f,
                   0.5f, -0.5f, 0.5f,
                   0.5f, -0.5f, -0.5f,
                   -0.5f, 0.5f, -0.5f,
                   -0.5f, 0.5f, 0.5f,
                   0.5f, 0.5f, 0.5f,
                   0.5f, 0.5f, -0.5f,
                   -0.5f, -0.5f, -0.5f,
                   -0.5f, 0.5f, -0.5f,
                   0.5f, 0.5f, -0.5f,
                   0.5f, -0.5f, -0.5f,
                   -0.5f, -0.5f, 0.5f,
                   -0.5f, 0.5f, 0.5f,
                   0.5f, 0.5f, 0.5f,
                   0.5f, -0.5f, 0.5f,
                   -0.5f, -0.5f, -0.5f,
                   -0.5f, -0.5f, 0.5f,
                   -0.5f, 0.5f, 0.5f,
                   -0.5f, 0.5f, -0.5f,
                   0.5f, -0.5f, -0.5f,
                   0.5f, -0.5f, 0.5f,
                   0.5f, 0.5f, 0.5f,
                   0.5f, 0.5f, -0.5f,
           };

   GLfloat cubeNormals[] =
           {
                   0.0f, -1.0f, 0.0f,
                   0.0f, -1.0f, 0.0f,
                   0.0f, -1.0f, 0.0f,
                   0.0f, -1.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, -1.0f,
                   0.0f, 0.0f, -1.0f,
                   0.0f, 0.0f, -1.0f,
                   0.0f, 0.0f, -1.0f,
                   0.0f, 0.0f, 1.0f,
                   0.0f, 0.0f, 1.0f,
                   0.0f, 0.0f, 1.0f,
                   0.0f, 0.0f, 1.0f,
                   -1.0f, 0.0f, 0.0f,
                   -1.0f, 0.0f, 0.0f,
                   -1.0f, 0.0f, 0.0f,
                   -1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f,
           };

   GLfloat cubeTex[] =
           {
                   0.0f, 0.0f,
                   0.0f, 1.0f,
                   1.0f, 1.0f,
                   1.0f, 0.0f,
                   1.0f, 0.0f,
                   1.0f, 1.0f,
                   0.0f, 1.0f,
                   0.0f, 0.0f,
                   0.0f, 0.0f,
                   0.0f, 1.0f,
                   1.0f, 1.0f,
                   1.0f, 0.0f,
                   0.0f, 0.0f,
                   0.0f, 1.0f,
                   1.0f, 1.0f,
                   1.0f, 0.0f,
                   0.0f, 0.0f,
                   0.0f, 1.0f,
                   1.0f, 1.0f,
                   1.0f, 0.0f,
                   0.0f, 0.0f,
                   0.0f, 1.0f,
                   1.0f, 1.0f,
                   1.0f, 0.0f,
           };

   // Allocate memory for buffers
   if (vertices != nullptr) {
      *vertices = (GLfloat *) malloc(sizeof(GLfloat) * 3 * numVertices);
      memcpy(*vertices, cubeVerts, sizeof(cubeVerts));

      for (i = 0; i < numVertices * 3; i++) {
         (*vertices)[i] *= scale;
      }
   }

   if (normals != nullptr) {
      *normals = (GLfloat *) malloc(sizeof(GLfloat) * 3 * numVertices);
      memcpy(*normals, cubeNormals, sizeof(cubeNormals));
   }

   if (texCoords != nullptr) {
      *texCoords = (GLfloat *) malloc(sizeof(GLfloat) * 2 * numVertices);
      memcpy(*texCoords, cubeTex, sizeof(cubeTex));
   }


   // Generate the indices
   if (indices != nullptr) {
      GLuint cubeIndices[] =
              {
                      0, 2, 1,
                      0, 3, 2,
                      4, 5, 6,
                      4, 6, 7,
                      8, 9, 10,
                      8, 10, 11,
                      12, 15, 14,
                      12, 14, 13,
                      16, 17, 18,
                      16, 18, 19,
                      20, 23, 22,
                      20, 22, 21
              };

      *indices = (GLuint *) malloc(sizeof(GLuint) * numIndices);
      memcpy(*indices, cubeIndices, sizeof(cubeIndices));
   }

   return numIndices;
}

GLfloat* esGenerateCubeData(float* point1,
                            float* point2,
                            float* point3,
                            float* point4,
                            float* point5,
                            float* point6,
                            float* point7,
                            float* point8,
                            int elementsPerPoint) {

   // Given a cube with the points define as follows:
   // front left top, front right top, front left bottom,front right bottom
   // back left top, back right top, back left bottom, front right bottom
   // return an array of 6 sides, 2 triangles per side, 3 vertices per cube.

   int FRONT = 0;
   int RIGHT = 1;
   int BACK = 2;
   int LEFT = 3;
   int TOP = 4;
   //    int BOTTOM = 5;

   int size = elementsPerPoint * 6 * 6;
   auto* cubeData = new float[size];

   for (int face = 0; face < 6; face++) {
      // Relative to the side,
      // p1 = top left
      // p2 = top right
      // p3 = bottom left
      // p4 = bottom right
      float* p1, * p2, * p3, * p4;

      // Select the points for this face
      if (face == FRONT) {
         p1 = point1;
         p2 = point2;
         p3 = point3;
         p4 = point4;
      }
      else if (face == RIGHT) {
         p1 = point2;
         p2 = point6;
         p3 = point4;
         p4 = point8;
      }
      else if (face == BACK) {
         p1 = point6;
         p2 = point5;
         p3 = point8;
         p4 = point7;
      }
      else if (face == LEFT) {
         p1 = point5;
         p2 = point1;
         p3 = point7;
         p4 = point3;
      }
      else if (face == TOP) {
         p1 = point5;
         p2 = point6;
         p3 = point1;
         p4 = point2;
      }
      else // if (face == BOTTOM)
      {
         p1 = point8;
         p2 = point7;
         p3 = point4;
         p4 = point3;
      }

      // In OpenGL counter-clockwise winding is default.
      // This means that when we look at a triangle,
      // if the points are counter-clockwise we are looking at the "front".
      // If not we are looking at the back.
      // OpenGL has an optimization where all back-facing triangles are culled, since they
      // usually represent the backside of an object and aren't visible anyways.

      // Build the triangles
      //  1---3,6
      //  | / |
      // 2,4--5
      int offset = face * elementsPerPoint * 6;

      for (int i = 0; i < elementsPerPoint; i++) {
         cubeData[offset++] = p1[i];
      }
      for (int i = 0; i < elementsPerPoint; i++) {
         cubeData[offset++] = p3[i];
      }
      for (int i = 0; i < elementsPerPoint; i++) {
         cubeData[offset++] = p2[i];
      }
      for (int i = 0; i < elementsPerPoint; i++) {
         cubeData[offset++] = p3[i];
      }
      for (int i = 0; i < elementsPerPoint; i++) {
         cubeData[offset++] = p4[i];
      }
      for (int i = 0; i < elementsPerPoint; i++) {
         cubeData[offset++] = p2[i];
      }
   }
   return cubeData;
}



int esGenSquareGrid ( int size, GLfloat **vertices, GLuint **indices )
{
   int i, j;
   int numIndices = ( size - 1 ) * ( size - 1 ) * 2 * 3;

   // Allocate memory for buffers
   if ( vertices != nullptr )
   {
      int numVertices = size * size;
      float stepSize = ( float ) size - 1;
      *vertices =  (GLfloat *) malloc ( sizeof ( GLfloat ) * 3 * numVertices );

      for ( i = 0; i < size; ++i ) // row
      {
         for ( j = 0; j < size; ++j ) // column
         {
            ( *vertices ) [ 3 * ( j + i * size )     ] = i / stepSize;
            ( *vertices ) [ 3 * ( j + i * size ) + 1 ] = j / stepSize;
            ( *vertices ) [ 3 * ( j + i * size ) + 2 ] = 0.0f;
         }
      }
   }

   // Generate the indices
   if ( indices != nullptr )
   {
      *indices =  (GLuint *) malloc ( sizeof ( GLuint ) * numIndices );

      for ( i = 0; i < size - 1; ++i )
      {
         for ( j = 0; j < size - 1; ++j )
         {
            // two triangles per quad
            ( *indices ) [ 6 * ( j + i * ( size - 1 ) )     ] = j + ( i )   * ( size )    ;
            ( *indices ) [ 6 * ( j + i * ( size - 1 ) ) + 1 ] = j + ( i )   * ( size ) + 1;
            ( *indices ) [ 6 * ( j + i * ( size - 1 ) ) + 2 ] = j + ( i + 1 ) * ( size ) + 1;

            ( *indices ) [ 6 * ( j + i * ( size - 1 ) ) + 3 ] = j + ( i )   * ( size )    ;
            ( *indices ) [ 6 * ( j + i * ( size - 1 ) ) + 4 ] = j + ( i + 1 ) * ( size ) + 1;
            ( *indices ) [ 6 * ( j + i * ( size - 1 ) ) + 5 ] = j + ( i + 1 ) * ( size )    ;
         }
      }
   }

   return numIndices;
}
