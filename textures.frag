#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform sampler2D ourTexture3;

void main()
{
        vec3 yuv;  
        vec3 rgb;      
        yuv.x = texture(ourTexture1, TexCoord).r;  
        yuv.y = texture(ourTexture2, TexCoord).r - 0.5;  
        yuv.z = texture(ourTexture3, TexCoord).r - 0.5;  
        rgb = mat3( 1,       1,         1,  
                    0,       -0.39465,  2.03211,  
                    1.13983, -0.58060,  0) * yuv;      
        color = vec4(rgb, 1);  
}
