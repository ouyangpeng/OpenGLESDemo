#version 300 es
precision highp float;

// 马赛克效果就是把图片的一个相当大小的区域用同一个点的颜色来表示.可以认为是大规模的降低图像的分辨率,而让图像的一些细节隐藏起来。

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

const vec2 TexSize = vec2(400.0, 400.0);
const vec2 mosaicSize = vec2(8.0, 8.0);

void main() {
    vec2 intXY = vec2(
        v_texCoord.x * TexSize.x,
        v_texCoord.y * TexSize.y
    );
    vec2 XYMosaic = vec2(
        floor(intXY.x / mosaicSize.x) * mosaicSize.x,
        floor(intXY.y / mosaicSize.y) * mosaicSize.y
    );
    vec2 UVMosaic = vec2(
        XYMosaic.x / TexSize.x,
        XYMosaic.y / TexSize.y
    );
    outColor = texture(s_TextureMap, UVMosaic);
}