//
// Created by OuyangPeng on 2021/10/23.
//
#pragma once
#include <GLES3/gl3.h>

typedef struct
{
    GLfloat   m[4][4];
} ESMatrix;


/**
 * 将 result 表示的矩阵 乘以 比例缩放矩阵，并在 result 中返回新矩阵
 * @param result    输入矩阵
 * @param sx        x轴上的比例 缩放因子
 * @param sy        y轴上的比例 缩放因子
 * @param sz        z轴上的比例 缩放因子
 */
void esScale (ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );
/**
 * 将 result 表示的矩阵 乘以 平移矩阵，并在 result 中返回新矩阵
 * @param result    输入矩阵
 * @param tx        x轴上的比例 平移因子
 * @param ty        y轴上的比例 平移因子
 * @param tz        z轴上的比例 平移因子
 */
void esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
/**
 *  将 result 表示的矩阵 乘以 旋转矩阵，并在 result 中返回新矩阵
 * @param result    输入矩阵
 * @param angle     指定旋转角度，以度数表示
 * @param x         指定向量的x坐标
 * @param y         指定向量的y坐标
 * @param z         指定向量的z坐标
 */
void esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
/**
 * 将 result 表示的矩阵 乘以 透视投影矩阵，并在 result 中返回新矩阵
 * @param result    输入矩阵
 * @param left      指定左裁剪平面坐标
 * @param right     指定右裁剪平面坐标
 * @param bottom    指定下裁剪平面坐标
 * @param top       指定上裁剪平面坐标
 * @param nearZ     指定到近深度裁剪平面的距离，必须为正数
 * @param farZ      指定到远深度裁剪平面的距离，必须为正数
 */
void esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );

/**
 * 将 result 表示的矩阵 乘以 透视投影矩阵，并在 result 中返回新矩阵。
 * 该函数是为了比直接使用esFrustum更简单地创建透视矩阵
 * @param result    输入矩阵
 * @param fovy      指定以度数表示的视野，应该在 0~180之间
 * @param aspect    渲染窗口的纵横比 （宽度/高度)
 * @param nearZ     指定到近深度裁剪平面的距离，必须为正数
 * @param farZ      指定到远深度裁剪平面的距离，必须为正数
 */
void esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ );

/**
 * 将 result 表示的矩阵 乘以 正交投影矩阵，并在 result 中返回新矩阵。
 * @param result    输入矩阵
 * @param left      指定左裁剪平面坐标
 * @param right     指定右裁剪平面坐标
 * @param bottom    指定下裁剪平面坐标
 * @param top       指定上裁剪平面坐标
 * @param nearZ     指定到近深度裁剪平面的距离，可以是正数也可以是负数
 * @param farZ      指定到远深度裁剪平面的距离，可以是正数也可以是负数
 */
void esOrtho ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );

/**
 * 这个函数将 srcA 和 srcB 相乘，并在 result中返回结果    result = srcA x srcB
 * @param result    指向返回相乘后矩阵的内存的指针
 * @param srcA      进行乘法运算的输入矩阵
 * @param srcB      进行乘法运算的输入矩阵
 */
void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );

/**
 * 一个单位矩阵
 * @param result  指向返回单位矩阵的内存的指针
 */
void esMatrixLoadIdentity ( ESMatrix *result );

/**
 * 用眼睛位置、视线向量和上向量生成一个视图变换矩阵
 * @param result    输出矩阵
 * @param posX      指定眼睛位置的坐标
 * @param posY      指定眼睛位置的坐标
 * @param posZ      指定眼睛位置的坐标
 * @param lookAtX   指定视线向量
 * @param lookAtY   指定视线向量
 * @param lookAtZ   指定视线向量
 * @param upX       指定上向量
 * @param upY       指定上向量
 * @param upZ       指定上向量
 */
void esMatrixLookAt ( ESMatrix *result,
                      float posX,    float posY,    float posZ,
                      float lookAtX, float lookAtY, float lookAtZ,
                      float upX,     float upY,     float upZ );

