#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(position, 1.0f);
	ourColor = color;
	//ourColor.x=1;
	//ourColor.y=1
	//ourColor.z=1
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	// TexCoord = texCoord;
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);

}

/*
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoordY;
layout (location = 3) in vec2 texCoordU;
layout (location = 4) in vec2 texCoordV;

out vec3 ourColor;
out vec2 TexCoordY;
out vec2 TexCoordU;
out vec2 TexCoordV;

void main()
{
	gl_Position = vec4(position, 1.0f);
	ourColor = color;
	//ourColor.x=1;
	//ourColor.y=1
	//ourColor.z=1
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	// TexCoord = texCoord;
	TexCoordY = vec2(texCoordY.x, 1.0 - texCoordY.y);
	TexCoordU = vec2(texCoordU.x, 1.0 - texCoordU.y);
	TexCoordV = vec2(texCoordV.x, 1.0 - texCoordV.y);
}
*/
