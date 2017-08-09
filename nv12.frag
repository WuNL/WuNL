#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D ourTextureY;
uniform sampler2D ourTextureUV;

void main()
{
        vec3 yuv;
        vec3 rgb;
		
        yuv.x = texture(ourTextureY, TexCoord).r -16./256.;
        //yuv.yz = texture(ourTextureUV, TexCoord).ra - vec2(128./256.,128./256.);
	yuv.y = texture(ourTextureUV, TexCoord).r - 128./256.;
        yuv.z = texture(ourTextureUV, TexCoord).g - 128./256.;
        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;
        color = vec4(rgb, 1);
}


