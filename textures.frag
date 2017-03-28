#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D ourTextureYUV;

void main()
{
        vec3 yuv;
        vec3 rgb;
        vec2 TexCoordY = vec2(TexCoord.x,TexCoord.y*2.0f/3.0f);
        vec2 TexCoordU = vec2(TexCoord.x/2.0f,(TexCoord.y+2.0f)/3.0f);
        vec2 TexCoordV = vec2(TexCoord.x/2.0f +0.5f,(TexCoord.y+2.0f)/3.0f);
        yuv.x = texture(ourTextureYUV, TexCoordY).r;
        yuv.y = texture(ourTextureYUV, TexCoordU).r - 0.5;
        yuv.z = texture(ourTextureYUV, TexCoordV).r - 0.5;
        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;
        color = vec4(rgb, 1);
}


//backup 3-28 WuNL
//#version 330 core
//in vec3 ourColor;
//in vec2 TexCoordY;
//in vec2 TexCoordU;
//in vec2 TexCoordV;
//
//out vec4 color;
//
//// Texture samplers
//uniform sampler2D ourTextureYUV;
//
//void main()
//{
//        vec3 yuv;
//        vec3 rgb;
//        yuv.x = texture(ourTextureYUV, TexCoordY).r;
//        yuv.y = texture(ourTextureYUV, TexCoordU).r - 0.5;
//        yuv.z = texture(ourTextureYUV, TexCoordV).r - 0.5;
//        rgb = mat3( 1,       1,         1,
//                    0,       -0.39465,  2.03211,
//                    1.13983, -0.58060,  0) * yuv;
//        color = vec4(rgb, 1);
//}
