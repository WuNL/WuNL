#include "glViewer.h"
#define SWS_WIDTH  1920
#define SWS_HEIGHT  1080

glViewer::glViewer()
{
    //ctor
    windowStyle = WINDOW_STYLE;
    windowChange = 1;
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


void glViewer::setStyle(int style)
{
    windowStyle = style;
    windowChange = 1;
}

void updatePixels(GLubyte* dst, int size)
{
    static int color = 0;

    if(!dst)
        return;

    int* ptr = (int*)dst;

    // copy 4 bytes at once
    for(int i = 0; i < SWS_HEIGHT; ++i)
    {
        for(int j = 0; j < SWS_WIDTH; ++j)
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
    int pitch = SWS_WIDTH;
    GLubyte* dst1 = dst+y*pitch+x;
    GLubyte* src = (GLubyte*)data;
    for(int i=0; i<h; ++i)
    {
        memcpy(dst1,src,w);
        dst1    += pitch;
        src     += w;
    }
}

bool glViewer::bufferEmpty()
{
    for(int i =0; i<WINDOW_STYLE; ++i)
    {
        if(!(*pFrameQueueVecPtr_)[i].empty())
            return false;
    }
    return true;
}

void glViewer::run()
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


    m_Thread = boost::thread(&glViewer::test, this);
}

int glViewer::test()
{

    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);
    int channelNum = windowStyle;
    int DATA_SIZE = 0;
    GLfloat* verticesVec[channelNum];
    glGenBuffers(16, pboIds);
    glGenVertexArrays(16, VAO);
    glGenBuffers(16, VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(16, texture);
    // Build and compile our shader program
    ourShader = new Shader("textures.vs", "textures.frag");
    ourShader->Use();
    AVFrame	*pFrame ;
    static int framecount = 0;
    struct timeval t_start,t_end;
    long cost_time=0;
    gettimeofday(&t_start,NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
    printf("start time:%ld ms\n",start);
    while (!glfwWindowShouldClose(window))
    {
        //std::cout<<"windowStyle! "<<windowStyle<<"\t windowChange:"<<windowChange<<std::endl;
        if(windowChange==1)
        {
            std::cout<<"windowStyle! "<<windowStyle<<"\t windowChange:"<<windowChange<<std::endl;
            channelNum =windowStyle;
            if(channelNum==1)
            {

            }
            else if(channelNum==4)
            {
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
                verticesVec[0]=vertices;
                verticesVec[1]=vertices1;
                verticesVec[2]=vertices2;
                verticesVec[3]=vertices3;
                GLuint indices[] =    // Note that we start from 0!
                {
                    0, 1, 3, // First Triangle
                    1, 2, 3  // Second Triangle
                };
                DATA_SIZE = SWS_WIDTH*SWS_HEIGHT*3/2;

                for(int i =0; i<WINDOW_STYLE; ++i)
                {
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                    glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                    glBindVertexArray(VAO[i]);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
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
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SWS_WIDTH,SWS_HEIGHT*3/2, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
                }
            }
            else if(channelNum==9)
            {
                // Set up vertex data (and buffer(s)) and attribute pointers
                GLfloat vertices[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    -1.0f/3.0f,  1.0f,      0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    -1.0f/3.0f,  1.0f/3.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    -1.0f,       1.0f/3.0f, 0.0f,     -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    -1.0f,       1.0f,      0.0f,     -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices1[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    1.0f/3.0f,  1.0f,       0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    1.0f/3.0f,  1.0f/3.0f,  0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    -1.0f/3.0f,  1.0f/3.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    -1.0f/3.0f,  1.0f,      0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices2[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    1.0f,       1.0f,       0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    1.0f,       1.0f/3.0f,  0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    1.0f/3.0f,  1.0f/3.0f,  0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    1.0f/3.0f,  1.0f,       0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices3[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    -1.0f/3.0f,  1.0f/3.0f,      0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    -1.0f/3.0f,  -1.0f/3.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    -1.0f,       -1.0f/3.0f, 0.0f,     -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    -1.0f,       1.0f/3.0f,      0.0f,     -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices4[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    1.0f/3.0f,  1.0f/3.0f,       0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    1.0f/3.0f,  -1.0f/3.0f,  0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    -1.0f/3.0f,  -1.0f/3.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    -1.0f/3.0f,  1.0f/3.0f,      0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices5[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    1.0f,       1.0f/3.0f,       0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    1.0f,       -1.0f/3.0f,  0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    1.0f/3.0f,  -1.0f/3.0f,  0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    1.0f/3.0f,  1.0f/3.0f,       0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices6[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    -1.0f/3.0f,  -1.0f/3.0f,      0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    -1.0f/3.0f,  -1.0f, 0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    -1.0f,       -1.0f, 0.0f,     -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    -1.0f,       -1.0f/3.0f,      0.0f,     -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices7[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    1.0f/3.0f,  -1.0f/3.0f,       0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    1.0f/3.0f,  -1.0f,  0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    -1.0f/3.0f,  -1.0f, 0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    -1.0f/3.0f,  -1.0f/3.0f,      0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                GLfloat vertices8[] =
                {
                    // Positions                        // Colors           // Texture Coords
                    1.0f,       -1.0f/3.0f,       0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f,            // Top Right
                    1.0f,       -1.0f,  0.0f,      1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
                    1.0f/3.0f,  -1.0f,  0.0f,      -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom Left
                    1.0f/3.0f,  -1.0f/3.0f,       0.0f,      -1.0f, 1.0f, 0.0f,      0.0f, 1.0f  // Top Left
                };
                verticesVec[0]=vertices;
                verticesVec[1]=vertices1;
                verticesVec[2]=vertices2;
                verticesVec[3]=vertices3;
                verticesVec[4]=vertices4;
                verticesVec[5]=vertices5;
                verticesVec[6]=vertices6;
                verticesVec[7]=vertices7;
                verticesVec[8]=vertices8;
                GLuint indices[] =    // Note that we start from 0!
                {
                    0, 1, 3, // First Triangle
                    1, 2, 3  // Second Triangle
                };
                DATA_SIZE = SWS_WIDTH*SWS_HEIGHT*3/2;
                for(int i =0; i<channelNum; ++i)
                {
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                    glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                    glBindVertexArray(VAO[i]);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
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
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SWS_WIDTH,SWS_HEIGHT*3/2, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
                }
            }
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
            windowChange = 0;
        }
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //std::cout << "This program (gl) is on CPU " << sched_getcpu() << std::endl;
        if(!bufferEmpty())
        {
            for(int i=0; i<windowStyle; ++i)
            {
                if((*pFrameQueueVecPtr_)[i].empty())
                {
                    glBindTexture(GL_TEXTURE_2D, texture[i]);

                    glBindVertexArray(VAO[i]);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    continue;
                }
                pFrame=(*pFrameQueueVecPtr_)[i].front();

                glBindTexture(GL_TEXTURE_2D, texture[i]);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
                GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptr)
                {
                    memcpy(ptr,pFrame->data[0],SWS_WIDTH*SWS_HEIGHT);
                    updateImage(ptr,0,SWS_HEIGHT,SWS_WIDTH/2,SWS_HEIGHT/2,pFrame->data[1]);
                    updateImage(ptr,SWS_WIDTH/2,SWS_HEIGHT,SWS_WIDTH/2,SWS_HEIGHT/2,pFrame->data[2]);
                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
                }
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SWS_WIDTH,SWS_HEIGHT*3/2, GL_RED, GL_UNSIGNED_BYTE, 0);
                glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureYUV"), 0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                glBindVertexArray(VAO[i]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                (*pFrameQueueVecPtr_)[i].pop();
                av_frame_free(&pFrame);
            }
            framecount++;
            gettimeofday(&t_end,NULL);
            long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
            cost_time = end - start;
            if(framecount%20==0)
                printf("fps:    %f\n",(float)framecount*1000/cost_time);
            glBindVertexArray(0);
            glfwSwapBuffers(window);
        }
        else
        {
            for(int i=0; i<windowStyle; ++i)
            {
                // Bind Texture
                glBindTexture(GL_TEXTURE_2D, texture[i]);

                glBindVertexArray(VAO[i]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            glBindVertexArray(0);
            //Swap the screen buffers
            glfwSwapBuffers(window);
            //usleep(30000);
        }
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    glDeleteBuffers(WINDOW_STYLE, pboIds);
    return 0;
}

int glViewer::init()
{
    return 0;
}
#include <stdio.h>
int glViewer::render()
{
    return 0;
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
