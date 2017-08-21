#include "viewer.h"

void viewer::updateImage(GLubyte* dst,int x,int y, int w,int h,void* data)
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

viewer::viewer()
{
    //ctor
}

viewer::~viewer()
{
    //dtor
}

void viewer::run()
{
    // Init GLFW
    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Viewer By WuNL", NULL, NULL);


    m_Thread = boost::thread(&viewer::devFun, this);
}

void viewer::display()
{
    printf("display ! \n");
}

void viewer::devFun()
{
    glfwMakeContextCurrent(window);
    // set vsync. 0:off max 1000+fps 1: on max 60fps
    glfwSwapInterval(1);
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        // Positions                       // Texture Coords
        -1.0f/3.0f,  1.0f,      0.0f,      1.0f, 1.0f,            // Top Right
        -1.0f/3.0f,  1.0f/3.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,       1.0f/3.0f, 0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f,       1.0f,      0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices1[] =
    {
        // Positions                       / Texture Coords
        1.0f/3.0f,  1.0f,       0.0f,      1.0f, 1.0f,            // Top Right
        1.0f/3.0f,  1.0f/3.0f,  0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f/3.0f,  1.0f/3.0f, 0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f/3.0f,  1.0f,      0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices2[] =
    {
        // Positions                       // Texture Coords
        1.0f,       1.0f,       0.0f,      1.0f, 1.0f,            // Top Right
        1.0f,       1.0f/3.0f,  0.0f,      1.0f, 0.0f, // Bottom Right
        1.0f/3.0f,  1.0f/3.0f,  0.0f,      0.0f, 0.0f, // Bottom Left
        1.0f/3.0f,  1.0f,       0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices3[] =
    {
        // Positions                        // Texture Coords
        -1.0f/3.0f,  1.0f/3.0f,  0.0f,      1.0f, 1.0f,            // Top Right
        -1.0f/3.0f,  -1.0f/3.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,       -1.0f/3.0f, 0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f,       1.0f/3.0f,  0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices4[] =
    {
        // Positions                        // Texture Coords
        1.0f/3.0f,  1.0f/3.0f,   0.0f,      1.0f, 1.0f,            // Top Right
        1.0f/3.0f,  -1.0f/3.0f,  0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f/3.0f,  -1.0f/3.0f, 0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f/3.0f,  1.0f/3.0f,  0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices5[] =
    {
        // Positions                        // Texture Coords
        1.0f,       1.0f/3.0f,   0.0f,      1.0f, 1.0f,            // Top Right
        1.0f,       -1.0f/3.0f,  0.0f,      1.0f, 0.0f, // Bottom Right
        1.0f/3.0f,  -1.0f/3.0f,  0.0f,      0.0f, 0.0f, // Bottom Left
        1.0f/3.0f,  1.0f/3.0f,   0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices6[] =
    {
        // Positions                        // Texture Coords
        -1.0f/3.0f,  -1.0f/3.0f, 0.0f,      1.0f, 1.0f,            // Top Right
        -1.0f/3.0f,  -1.0f,      0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,       -1.0f,      0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f,       -1.0f/3.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices7[] =
    {
        // Positions                        // Texture Coords
        1.0f/3.0f,  -1.0f/3.0f,  0.0f,      1.0f, 1.0f,            // Top Right
        1.0f/3.0f,  -1.0f,       0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f/3.0f,  -1.0f,      0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f/3.0f,  -1.0f/3.0f, 0.0f,      0.0f, 1.0f  // Top Left
    };
    GLfloat vertices8[] =
    {
        // Positions                       // Texture Coords
        1.0f,       -1.0f/3.0f,  0.0f,      1.0f, 1.0f,            // Top Right
        1.0f,       -1.0f,       0.0f,      1.0f, 0.0f, // Bottom Right
        1.0f/3.0f,  -1.0f,       0.0f,      0.0f, 0.0f, // Bottom Left
        1.0f/3.0f,  -1.0f/3.0f,  0.0f,      0.0f, 1.0f  // Top Left
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
    glGenBuffers(16, pboIds);
    glGenBuffers(16, pboUV);
    glGenVertexArrays(16, VAO);
    glGenBuffers(16, VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(1, &texture);
    glGenTextures(1, &textureUV);

    ourShader = new Shader("nv12.vs", "nv12.frag");
    ourShader->Use();
    AVFrame	*pFrame ;
    int DATA_SIZE = SWS_WIDTH*SWS_HEIGHT;

    // Load and create a texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, SWS_WIDTH, SWS_HEIGHT, 9);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, SWS_WIDTH, SWS_HEIGHT, 9, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
//    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, SWS_WIDTH, SWS_HEIGHT, 9, )
    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // Load and create a texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, SWS_WIDTH/2,SWS_HEIGHT/2, 9);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, SWS_WIDTH/2,SWS_HEIGHT/2, 9, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    for(int i=0;i<9;++i)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE/2, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), verticesVec[i], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // TexCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0); // Unbind VAO


        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE/2, 0, GL_STREAM_DRAW);
    }





    static int framecount = 0;
    struct timeval t_start,t_end;
    long cost_time=0;
    gettimeofday(&t_start,NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
    printf("start time:%ld ms\n",start);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        //if(!(*pFrameQueueVecPtr_)[0].empty() || !(*pFrameQueueVecPtr_)[1].empty())
        {
            //int i = 1;
            for(int i = 0; i<9; ++i)
            {
                if((*pFrameQueueVecPtr_)[i].empty())
                {
                    glBindVertexArray(VAO[i]);

                    glActiveTexture(GL_TEXTURE0);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);

                    glUniform1i(glGetUniformLocation(ourShader->Program, "layer"), 2);
                    glProgramUniform1i(ourShader->Program, glGetUniformLocation(ourShader->Program, "layer"), i);


                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SWS_WIDTH,SWS_HEIGHT, 1, GL_RED, GL_UNSIGNED_BYTE, 0);
                    glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureY"), 0);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


                    glActiveTexture(GL_TEXTURE1);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SWS_WIDTH/2,SWS_HEIGHT/2, 1, GL_RG, GL_UNSIGNED_BYTE, 0);
                    glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureUV"), 1);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                    continue;
                }
                pFrame=(*pFrameQueueVecPtr_)[i].front();
                (*pFrameQueueVecPtr_)[i].pop();

                glBindVertexArray(VAO[i]);

                glActiveTexture(GL_TEXTURE0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptr)
                {
                    memcpy(ptr,pFrame->data[0],SWS_WIDTH*SWS_HEIGHT);
                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
                }
                glUniform1i(glGetUniformLocation(ourShader->Program, "layer"), 2);
                glProgramUniform1i(ourShader->Program, glGetUniformLocation(ourShader->Program, "layer"), i);


                glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SWS_WIDTH,SWS_HEIGHT, 1, GL_RED, GL_UNSIGNED_BYTE, 0);
                glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureY"), 0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


                glActiveTexture(GL_TEXTURE1);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
                GLubyte* ptrUV = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptrUV)
                {
                    memcpy(ptrUV,pFrame->data[1],SWS_WIDTH*SWS_HEIGHT/2);
                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
                }

                glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SWS_WIDTH/2,SWS_HEIGHT/2, 1, GL_RG, GL_UNSIGNED_BYTE, 0);
                glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureUV"), 1);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                av_frame_free(&pFrame);
            }


            glBindVertexArray(0);
            glfwPollEvents();
            glfwSwapBuffers(window);

            framecount++;
            gettimeofday(&t_end,NULL);
            long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
            cost_time = end - start;

            if(cost_time/1000.0 > 1)
            {
                printf("fps:    %f\n",(float)framecount*1000/cost_time);
                gettimeofday(&t_start,NULL);
                start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
                framecount = 0;
            }
        }
    }
}

void viewer::displayFun()
{
    glfwMakeContextCurrent(window);
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat points[] =
    {
        // Positions                           // Texture Coords
        -1.0f,  1.0f, 0.0f,        1.0f, 1.0f,            // Top Right
        -1.0f,  1.0f, 0.0f,        1.0f, 0.0f, // Bottom Right
        -1.0f,  1.0f, 0.0f,        0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f,        0.0f, 1.0f  // Top Left
    };

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    const char* vertex_shader =
        "#version 400\n"
        "in vec3 vp;"
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";
    const char* fragment_shader =
        "#version 400\n"
        "out vec4 frag_colour;"
        "void main() {"
        "  frag_colour = vec4(0.5, 0.0, 0.5, 0.0);"
        "}";
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);
    while(!glfwWindowShouldClose(window))
    {
        // wipe the drawing surface clear
        glClearColor(1,0.1,0.11,0.5);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }
}
