#include "audioBarDrawer.h"
#include <algorithm>
barDrawer::barDrawer(int window_width, int window_height):shader("shaders/drawer.vert", "shaders/drawer.frag")
{
    window_width_ = window_width;

    window_height_ = window_height;
}

barDrawer::~barDrawer()
{

}

audioBarDrawer::audioBarDrawer(int window_width, int window_height):barDrawer(window_width,window_height)

{
    //ctor
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

audioBarDrawer::~audioBarDrawer()
{
    //dtor
}

void audioBarDrawer::draw(int audioScale,int xOffset, int yOffset, int height)
{

    float w = window_width_/2;
    float h = window_height_/2;

    float xOffset21 = (float)(xOffset-w)/w;
    float yOffset21 = (float)(yOffset-h)/h;
    float height21 = (float)(height)/h;



    float audioScale21 = (float)(audioScale/255.0f);


    float red   = (audioScale21>0.3f ? 1.0f : 0.0f);
    float green = (audioScale21 < 0.7f ? 1.0f : 0.0f);
    float blue  = 0.0f;
    audioScale21*=height21;
    float topHeight = std::min(yOffset21+audioScale21,yOffset21+height21);
    // update vertices
    shader.Use();
    float vertices[] =
    {
        // positions          // colors                            // texture coords
        xOffset21+0.01f, topHeight, 0.0f,   red, green, blue,   1.0f, 1.0f, // top right
        xOffset21+0.01f, yOffset21, 0.0f,          red, green, blue,   1.0f, 0.0f, // bottom right
        xOffset21,       yOffset21, 0.0f,          red, green, blue,   0.0f, 0.0f, // bottom left
        xOffset21,       topHeight, 0.0f,   red, green, blue,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

}
