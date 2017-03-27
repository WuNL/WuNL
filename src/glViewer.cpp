#include "glViewer.h"
#include "SOIL/SOIL.h"
glViewer::glViewer()
{
    //ctor
}

glViewer::~glViewer()
{
    //dtor

    glfwTerminate();
}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int WIDTH=WINDOW_WIDTH;
int HEIGHT=WINDOW_HEIGHT;

int glViewer::init()
{
    // Init GLFW
    glfwInit();
    std::cout<<"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:"<<GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS<<std::endl;

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);


    // Build and compile our shader program
    ourShader = new Shader("textures.vs", "textures.frag");


    // Set up vertex data (and buffer(s)) and attribute pointers
//    GLfloat vertices[] =
//    {
//        // Positions          // Colors           // Texture Coords
//        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
//        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left
//    };
    GLfloat vertices[] =
    {
        // Positions          // Colors           // Texture Coords Y U V
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,          1.0f, 1.0f,             0.5f, (GLfloat)1/3,      1.0f, (GLfloat)1/3, // Top Right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,          1.0f, (GLfloat)1/3,     0.5f, 0.0f,              1.0f, 0.0f,// Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,         0.0f, (GLfloat)1/3,     0.0f, 0.0f ,             0.5f, 0.0f,// Bottom Left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,         0.0f, 1.0f,             (GLfloat)1/3, 0.0f,      0.5f, (GLfloat)1/3// Top Left
    };
    GLuint indices[] =    // Note that we start from 0!
    {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(10 * sizeof(GLfloat)));
    glEnableVertexAttribArray(4);
    glBindVertexArray(0); // Unbind VAO


    // Load and create a texture
    GLuint texture,texture1,texture2;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920,1080*3/2, 0, GL_ALPHA, GL_UNSIGNED_BYTE,NULL);




//     Load image, create texture and generate mipmaps
    //int width, height;
    //unsigned char* image = SOIL_load_image("sdt.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
    //std::cout<<width<<" "<<height<<std::endl;
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //glGenerateMipmap(GL_TEXTURE_2D);
    //SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    FILE *fp_out1;
    fp_out1 = fopen("out1.yuv", "wb");
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if(!(*pFrameQueueVecPtr_)[0].empty())
        {

            AVFrame	*pFrame ;//= av_frame_alloc();
            pFrame=(*pFrameQueueVecPtr_)[0].front();

            //std::cout<<"size:"<<(*pFrameQueueVecPtr_)[0].size()<<"\t";

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);


//            if (0) {
//
//            //Y, U, V
//            for(int i=0;i<pFrame->height;i++){
//            fwrite(pFrame->data[0]+pFrame->linesize[0]*i,1,pFrame->width,fp_out1);
//            }
//            for(int i=0;i<pFrame->height/2;i++){
//            fwrite(pFrame->data[1]+pFrame->linesize[1]*i,1,pFrame->width/2,fp_out1);
//            }
//            for(int i=0;i<pFrame->height/2;i++){
//            fwrite(pFrame->data[2]+pFrame->linesize[2]*i,1,pFrame->width/2,fp_out1);
//            }
//
//            printf("Succeed to decode 1 frame!\n");
//            }


            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            //glTexStorage2D(GL_TEXTURE_2D,0,GL_RED,1920,1080);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920,1080, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
//            std::cerr << "OpenGL error 0: " << glGetError() << std::endl;
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,1920,1080,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[0]);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,1080,1920/2,1080/2,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[1]);
            glTexSubImage2D(GL_TEXTURE_2D,0,1920/2,1080,1920/2,1080/2,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[2]);
//            std::cerr << "OpenGL error: " << glGetError() << std::endl;
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920,1080, 0, GL_RED, GL_UNSIGNED_BYTE, pFrame->data[0]);
            glUniform1i(glGetUniformLocation(ourShader->Program, "ourTexture1"), 0);


            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            //glBindTexture(GL_TEXTURE_2D, 0);
            glfwSwapBuffers(window);
            av_frame_free(&pFrame);
            (*pFrameQueueVecPtr_)[0].pop();
            //usleep(30000);
        }
        else
        {
            // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
            glfwPollEvents();
            // Render
            // Clear the colorbuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            // Bind Texture
            glBindTexture(GL_TEXTURE_2D, texture);
            ourShader->Use();
            glBindTexture(GL_TEXTURE_2D, texture1);
            ourShader->Use();
            glBindTexture(GL_TEXTURE_2D, texture2);
            ourShader->Use();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
//          Swap the screen buffers
            glfwSwapBuffers(window);
        }




    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
#include <stdio.h>
int glViewer::render()
{

    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);


    // Build and compile our shader program
    Shader ourShader("textures.vs", "textures.frag");


    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        // Positions          // Colors           // Texture Coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left
    };
    GLuint indices[] =    // Note that we start from 0!
    {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO


    // Load and create a texture
    GLuint texture;
    glGenTextures(2, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
    std::cout<<width<<" "<<height<<std::endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // Bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // Activate shader
        ourShader.Use();

        // Draw container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return 0;
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
