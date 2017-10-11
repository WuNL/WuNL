#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2DArray ourTextureY;
uniform sampler2DArray ourTextureUV;
uniform int layer = 0;
void main()
{
        vec3 yuv;
        vec3 rgb;
		
        yuv.x = texture(ourTextureY, vec3(TexCoord,layer)).r -16./256.;
	yuv.yz = texture(ourTextureUV, vec3(TexCoord,layer)).rg - 128./256.;
        //yuv.z = texture(ourTextureUV, vec3(TexCoord,layer)).g - 128./256.;

        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;
        color = vec4(rgb, 1);
}


