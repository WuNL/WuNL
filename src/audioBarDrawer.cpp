#include "audioBarDrawer.h"

barDrawer::barDrawer(int window_width, int window_height):shader("shaders/drawer.vert", "shaders/drawer.frag")
{


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

void audioBarDrawer::draw()
{
    // update vertices
    shader.Use();
    float vertices[] = {
            // positions          // colors           // texture coords
             0.1f,  0.1f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
             0.1f, -0.1f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -0.1f, -0.1f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -0.1f,  0.1f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
        };
    unsigned int indices[] = {
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

}
