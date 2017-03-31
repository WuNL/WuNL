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
#include <sched.h>
#include <thread>

void updatePixels(GLubyte* dst, int size)
{
    static int color = 0;

    if(!dst)
        return;

    int* ptr = (int*)dst;

    // copy 4 bytes at once
    for(int i = 0; i < 1080; ++i)
    {
        for(int j = 0; j < 1920; ++j)
        {
            *ptr = color;
            ++ptr;
        }
        color += 257;   // add an arbitary number (no meaning)
    }
    ++color;            // scroll down
}

void updateImage(GLubyte* dst,int x,int y, int w,int h,void* data)
{
    int pitch = 1920;
    GLubyte* dst1 = dst+y*pitch+x;
    GLubyte* src = (GLubyte*)data;
    for(int i=0; i<h; ++i)
    {
        memcpy(dst1,src,w);
        dst1    += pitch;
        src     += w;
    }
}


int glViewer::test()
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
    GLfloat vertices[] =
    {
        // Positions          // Colors           // Texture Coords
        0.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
        0.0f,  0.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,  0.0f, 0.0f,     -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f,     -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices1[] =
    {
        // Positions          // Colors           // Texture Coords
        1.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
        1.0f,  0.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        0.0f,  0.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
        0.0f,  1.0f, 0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };

    GLfloat vertices2[] =
    {
        // Positions          // Colors           // Texture Coords
        0.0f,  0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
        0.0f,  -1.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,  -1.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
        -1.0f,  0.0f, 0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices3[] =
    {
        // Positions          // Colors           // Texture Coords
        1.0f,  0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
        1.0f,  -1.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        0.0f,  -1.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
        0.0f,  0.0f, 0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat* verticesVec[]={vertices,vertices1,vertices2,vertices3};
//    GLfloat vertices[] =
//    {
//        // Positions          // Colors           // Texture Coords Y U V
//        0.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,          1.0f, 1.0f,             0.5f, (GLfloat)1/3,      1.0f, (GLfloat)1/3, // Top Right
//        0.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,          1.0f, (GLfloat)1/3,     0.5f, 0.0f,              1.0f, 0.0f,// Bottom Right
//        -1.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,         0.0f, (GLfloat)1/3,     0.0f, 0.0f ,             0.5f, 0.0f,// Bottom Left
//        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,         0.0f, 1.0f,             0.0f, (GLfloat)1/3,      0.5f, (GLfloat)1/3// Top Left
//    };
    GLuint indices[] =    // Note that we start from 0!
    {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };


    int DATA_SIZE = 1920*1080*3/2;
    glGenBuffers(copyNum, pboIds);
    glGenVertexArrays(copyNum, VAO);
    glGenBuffers(copyNum, VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(copyNum, texture);
    for(int i =0;i<copyNum;++i)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        std::cout<<sizeof(verticesVec[i])<<std::endl;
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), verticesVec[i], GL_STATIC_DRAW);
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
        glBindTexture(GL_TEXTURE_2D, texture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920,1080*3/2, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
//    FILE *fp_out1;
//    fp_out1 = fopen("out1.yuv", "wb");
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        //std::cout << "This program (gl) is on CPU " << sched_getcpu() << std::endl;
        if(!(*pFrameQueueVecPtr_)[0].empty())
        {

            AVFrame	*pFrame ;
            pFrame=(*pFrameQueueVecPtr_)[0].front();

            std::cout<<"size:"<<(*pFrameQueueVecPtr_)[0].size()<<std::endl;;

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for(int i=0; i<copyNum; ++i)
            {
                glBindTexture(GL_TEXTURE_2D, texture[i]);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);

                GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptr)
                {
                    memcpy(ptr,pFrame->data[0],1920*1080);
                    updateImage(ptr,0,1080,1920/2,1080/2,pFrame->data[1]);
                    updateImage(ptr,1920/2,1080,1920/2,1080/2,pFrame->data[2]);
                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer

                }
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920,1080*3/2, GL_RED, GL_UNSIGNED_BYTE, 0);
                glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureYUV"), 0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                glBindVertexArray(VAO[i]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            glBindVertexArray(0);
            glfwSwapBuffers(window);
            av_frame_free(&pFrame);
            (*pFrameQueueVecPtr_)[0].pop();
        }
        else
        {
            // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
            glfwPollEvents();
            // Render
            // Clear the colorbuffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            for(int i=0;i<copyNum;++i)
            {
                // Bind Texture
                glBindTexture(GL_TEXTURE_2D, texture[i]);
                ourShader->Use();
                glBindVertexArray(VAO[i]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            glBindVertexArray(0);
//          Swap the screen buffers
            glfwSwapBuffers(window);
            //usleep(30000);
        }
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    glDeleteBuffers(copyNum, pboIds);
    return 0;
}



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
    GLfloat vertices[] =
    {
        // Positions          // Colors           // Texture Coords
        0.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
        0.0f,  0.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,  0.0f, 0.0f,     -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f,     -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices1[] =
    {
        // Positions          // Colors           // Texture Coords
        1.0f,  1.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
        1.0f,  0.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        0.0f,  0.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
        0.0f,  1.0f, 0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
//    GLfloat vertices[] =
//    {
//        // Positions          // Colors           // Texture Coords Y U V
//        0.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,          1.0f, 1.0f,             0.5f, (GLfloat)1/3,      1.0f, (GLfloat)1/3, // Top Right
//        0.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,          1.0f, (GLfloat)1/3,     0.5f, 0.0f,              1.0f, 0.0f,// Bottom Right
//        -1.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,         0.0f, (GLfloat)1/3,     0.0f, 0.0f ,             0.5f, 0.0f,// Bottom Left
//        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,         0.0f, 1.0f,             0.0f, (GLfloat)1/3,      0.5f, (GLfloat)1/3// Top Left
//    };
    GLuint indices[] =    // Note that we start from 0!
    {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
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


    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
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
    GLuint texture[2];
    glGenTextures(2, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920,1080*3.0f/2.0f, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

    glBindTexture(GL_TEXTURE_2D, texture[1]); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920,1080*3.0f/2.0f, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    FILE *fp_out1;
    fp_out1 = fopen("out1.yuv", "wb");
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        //std::cout << "This program (gl) is on CPU " << sched_getcpu() << std::endl;
        if(!(*pFrameQueueVecPtr_)[0].empty())
        {

            AVFrame	*pFrame ;//= av_frame_alloc();
            pFrame=(*pFrameQueueVecPtr_)[0].front();

            std::cout<<"size:"<<(*pFrameQueueVecPtr_)[0].size()<<std::endl;;

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


            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,1920,1080,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[0]);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,1080,1920/2,1080/2,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[1]);
            glTexSubImage2D(GL_TEXTURE_2D,0,1920/2,1080,1920/2,1080/2,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[2]);
            glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureYUV"), 0);
            glBindVertexArray(VAO[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindTexture(GL_TEXTURE_2D, texture[1]);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,1920,1080,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[0]);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,1080,1920/2,1080/2,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[1]);
            glTexSubImage2D(GL_TEXTURE_2D,0,1920/2,1080,1920/2,1080/2,GL_RED,GL_UNSIGNED_BYTE,pFrame->data[2]);
            glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureYUV"), 0);
            glBindVertexArray(VAO[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
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
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            ourShader->Use();
            glBindVertexArray(VAO[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


            glBindTexture(GL_TEXTURE_2D, texture[1]);
            ourShader->Use();
            glBindVertexArray(VAO[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
//          Swap the screen buffers
            glfwSwapBuffers(window);
            usleep(30000);
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
