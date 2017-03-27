#version 330 core
in vec3 ourColor;
in vec2 TexCoordY;
in vec2 TexCoordU;
in vec2 TexCoordV;

out vec4 color;

// Texture samplers
uniform sampler2D ourTextureYUV;

void main()
{
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture(ourTextureYUV, TexCoordY).r;
        yuv.y = texture(ourTextureYUV, TexCoordU).r - 0.5;
        yuv.z = texture(ourTextureYUV, TexCoordV).r - 0.5;
        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;
        color = vec4(rgb, 1);
}
