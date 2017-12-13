#include "viewer.h"
#include <math.h>
void viewer::updateImage(GLubyte* dst,int x,int y, int w,int h,void* data)
{
    int pitch = frameWidth;
    GLubyte* dst1 = dst+y*pitch+x;
    GLubyte* src = (GLubyte*)data;
    for(int i=0; i<h; ++i)
    {
        memcpy(dst1,src,w);
        dst1    += pitch;
        src     += w;
    }
}

viewer::viewer(int width,int height,int ind,bool autoS)
{
    //ctor
    w = width;
    h = height;
    index = ind;
    autoSwitch = autoS;
    active = true;

    frameWidth=1920;
    frameHeight=1080;
    splitNum_ = WINDOW_STYLE;
    splitNum_old = splitNum_;
    for(int i=0; i<16; ++i)
    {
        verticesVec[i] = new GLfloat[20];
    }
}

viewer::~viewer()
{
    //dtor
}

void viewer::run()
{
    m_Thread = boost::thread(&viewer::devFun, this);
}

void viewer::display()
{
    printf("display ! \n");
}

void viewer::setStyleInter()
{
    setVertices(splitNum_, 0);

    int DATA_SIZE = frameWidth*frameHeight;
    GLuint indices[] =    // Note that we start from 0!
    {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    GLfloat vertices[] =
    {
        // Positions                       // Texture Coords
        -1.0f/3.0f,  1.0f,      0.0f,      1.0f, 1.0f,            // Top Right
        -1.0f/3.0f,  1.0f/3.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,       1.0f/3.0f, 0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f,       1.0f,      0.0f,      0.0f, 1.0f  // Top Left
    };
    for(int i=0; i<splitNum_; ++i)
    {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), verticesVec[i], GL_STATIC_DRAW);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // TexCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0); // Unbind VAO
    }
}

void viewer::setStyle(int splitNum)
{
    splitNum_ = splitNum;
    printf("setStyle: splitNum=%d\n",splitNum);
}

void viewer::setVertices(int splitNum, int style)
{
    GLfloat cor3X3[3][3][2] =
    {
        { {-1.0f,1.0f},{0.0f,1.0f},{1.0f,1.0f} },
        { {-1.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f} },
        { {-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f} },
    };

    GLfloat cor4X4[4][4][2] =
    {
        { {-1.0f,1.0f},{-1.0f/3.0f,1.0f},{1.0f/3.0f,1.0f},{1.0f,1.0f} },
        { {-1.0f,1.0f/3.0f},{-1.0f/3.0f,1.0f/3.0f},{1.0f/3.0f,1.0f/3.0f},{1.0f,1.0f/3.0f} },
        { {-1.0f,-1.0f/3.0f},{-1.0f/3.0f,-1.0f/3.0f},{1.0f/3.0f,-1.0f/3.0f},{1.0f,-1.0f/3.0f} },
        { {-1.0f,-1.0f},{-1.0f/3.0f,-1.0f},{1.0f/3.0f,-1.0f},{1.0f,-1.0f} },
    };

    GLfloat corFXF[5][5][2]=
    {
        { {-1.0f,1.0f},{-0.5f,1.0f},{0.0f,1.0f},{0.5f,1.0f},{1.0f,1.0f} },
        { {-1.0f,0.5f},{-0.5f,0.5f},{0.0f,0.5f},{0.5f,0.5f},{1.0f,0.5f} },
        { {-1.0f,0.0f},{-0.5f,0.0f},{0.0f,0.0f},{0.5f,0.0f},{1.0f,0.0f} },
        { {-1.0f,-0.5f},{-0.5f,-0.5f},{0.0f,-0.5f},{0.5f,-0.5f},{1.0f,-0.5f} },
        { {-1.0f,-1.0f},{-0.5f,-1.0f},{0.0f,-1.0f},{0.5f,-1.0f},{1.0f,-1.0f} }
    };
    splitNum_ = splitNum;
    switch(splitNum_)
    {
    case 1:
    {
        GLfloat tmpVec[20]=
        {
            1.0f,    1.0f,     0.0f, 1.0f, 1.0f,            // Top Right
            1.0f,    -1.0f,    0.0f, 1.0f, 0.0f,            // Bottom Right
            -1.0f,   -1.0f,    0.0f, 0.0f, 0.0f,            // Bottom Left
            -1.0f,   1.0f,     0.0f, 0.0f, 1.0f            // Top Left
        };
        memcpy(verticesVec[0],tmpVec,20*sizeof(GLfloat));
        break;
    }
    case 4:
    {
        for(int i=0; i<4; ++i)
        {

            int y = i%2;
            int x = i/2;
            GLfloat tmpVec[20]=
            {
                cor3X3[x][y+1][0],    cor3X3[x][y+1][1],     0.0f, 1.0f, 1.0f,            // Top Right
                cor3X3[x+1][y+1][0],  cor3X3[x+1][y+1][1],   0.0f, 1.0f, 0.0f,            // Bottom Right
                cor3X3[x+1][y][0],    cor3X3[x+1][y][1],     0.0f, 0.0f, 0.0f,            // Bottom Left
                cor3X3[x][y][0],      cor3X3[x][y][1],       0.0f, 0.0f, 1.0f            // Top Left
            };
            memcpy(verticesVec[i],tmpVec,20*sizeof(GLfloat));
        }
        break;
    }
    case 9:
    {
        for(int i=0; i<9; ++i)
        {

            int y = i%3;
            int x = i/3;
            GLfloat tmpVec[20]=
            {
                cor4X4[x][y+1][0],    cor4X4[x][y+1][1],     0.0f, 1.0f, 1.0f,            // Top Right
                cor4X4[x+1][y+1][0],  cor4X4[x+1][y+1][1],   0.0f, 1.0f, 0.0f,            // Bottom Right
                cor4X4[x+1][y][0],    cor4X4[x+1][y][1],     0.0f, 0.0f, 0.0f,            // Bottom Left
                cor4X4[x][y][0],      cor4X4[x][y][1],       0.0f, 0.0f, 1.0f            // Top Left
            };
            memcpy(verticesVec[i],tmpVec,20*sizeof(GLfloat));
        }
        break;
    }
    case 16:
    {
        for(int i=0; i<16; ++i)
        {

            int y = i%4;
            int x = i/4;
            GLfloat tmpVec[20]=
            {
                corFXF[x][y+1][0],    corFXF[x][y+1][1],     0.0f, 1.0f, 1.0f,            // Top Right
                corFXF[x+1][y+1][0],  corFXF[x+1][y+1][1],   0.0f, 1.0f, 0.0f,            // Bottom Right
                corFXF[x+1][y][0],    corFXF[x+1][y][1],     0.0f, 0.0f, 0.0f,            // Bottom Left
                corFXF[x][y][0],      corFXF[x][y][1],       0.0f, 0.0f, 1.0f            // Top Left
            };
            memcpy(verticesVec[i],tmpVec,20*sizeof(GLfloat));
        }
        break;
    }
    default:
    {
        for(int i=0; i<16; ++i)
        {

            int y = i%4;
            int x = i/4;
            GLfloat tmpVec[20]=
            {
                corFXF[x][y+1][0],    corFXF[x][y+1][1],     0.0f, 1.0f, 1.0f,            // Top Right
                corFXF[x+1][y+1][0],  corFXF[x+1][y+1][1],   0.0f, 1.0f, 0.0f,            // Bottom Right
                corFXF[x+1][y][0],    corFXF[x+1][y][1],     0.0f, 0.0f, 0.0f,            // Bottom Left
                corFXF[x][y][0],      corFXF[x][y][1],       0.0f, 0.0f, 1.0f            // Top Left
            };
            memcpy(verticesVec[i],tmpVec,20*sizeof(GLfloat));
        }
        splitNum_ = 16;
        break;
    }
    }

}

void viewer::devFun()
{
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    const GLFWvidmode* mode = glfwGetVideoMode(monitors[index]);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_ICONIFIED,GL_FALSE);
    glfwWindowHint(GLFW_FOCUSED,GL_FALSE);
    glfwWindowHint(GLFW_AUTO_ICONIFY,GL_FALSE);
//glfwWindowHint(GLFW_,GL_FALSE);
    std::cout<<mode->width<<"  "<<mode->height<<std::endl;
    char* s;
    if(index==0)
    {
        s = "viewer 0";
    }
    else
        s = "viewer 1";
    if(autoSwitch)
    {
        w = mode->width;
        h = mode->height;
        window = glfwCreateWindow(1440, 900, s,NULL, NULL);
        //glfwSetWindowPos(window,1440*index,0);
        glfwSetWindowMonitor(window, monitors[index], 0, 0, mode->width, mode->height, mode->refreshRate);
        glfwMakeContextCurrent(window);
//    // set vsync. 0:off max 1000+fps 1: on max 60fps 2: on max 30fps
        glfwSwapInterval(-1);
        //glfwSetWindowMonitor(window, monitors[index], 0, 0, mode->width, mode->height, mode->refreshRate);

    }
    else
    {
        window = glfwCreateWindow(w, h, s, NULL, NULL);
        glfwSetWindowMonitor(window,monitors[index],0,0,w,h,mode->refreshRate);

    }

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

//    Display *dpy = glXGetCurrentDisplay();
//    GLXDrawable drawable = glXGetCurrentDrawable();
//    const int interval = 1;
//
//    glXSwapIntervalEXT(dpy, drawable, interval);


    if (GLX_NV_swap_group)
{
  /* Looks like ARB_fragment_program is supported. */
  printf("GLX_NV_swap_group is supported!\n");
}
    if(autoSwitch)
    {
        tr = new textRender(mode->width, mode->height,"/home/sdt/workspace/textRender/SourceHanSerifCN-Bold.otf");
    }
    else
    {
        tr = new textRender(w,h,"/home/sdt/workspace/textRender/SourceHanSerifCN-Bold.otf");
    }


    setVertices(splitNum_,0);
    std::cout<<verticesVec[0][0]<<std::endl;
    GLfloat vertices[] =
    {
        // Positions                       // Texture Coords
        -1.0f/3.0f,  1.0f,      0.0f,      1.0f, 1.0f,            // Top Right
        -1.0f/3.0f,  1.0f/3.0f, 0.0f,      1.0f, 0.0f, // Bottom Right
        -1.0f,       1.0f/3.0f, 0.0f,      0.0f, 0.0f, // Bottom Left
        -1.0f,       1.0f,      0.0f,      0.0f, 1.0f  // Top Left
    };
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
    int DATA_SIZE = frameWidth*frameHeight;

    // Load and create a texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, 9);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, 16, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
//    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, 9, )
    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // Load and create a texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, 9);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, 16, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    for(int i=0; i<16; ++i)
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

    int framecount = 0;
    int realcount = 0;
    float fps = 0.0f;
    struct timeval t_start,t_end;
    long cost_time=0;
    gettimeofday(&t_start,NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
    printf("start time:%ld ms\n",start);
    while (active)
    {


        if(splitNum_!=splitNum_old)
        {
            setStyleInter();
            splitNum_old = splitNum_;
        }
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        //if(!(*pFrameQueueVecPtr_)[0].empty() || !(*pFrameQueueVecPtr_)[1].empty())
        {
            //int i = 1;
            for(int i = 0; i<splitNum_; ++i)
            {
                int j = (*videoPositionVecPtr_)[index][i];
//                printf("j=%d \n",j);
                if( j<0 || (*pFrameQueueVecPtr_)[j].first.empty())
                {
                    glBindVertexArray(VAO[i]);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, splitNum_, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, splitNum_, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);
                    glActiveTexture(GL_TEXTURE0);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);

                    glUniform1i(glGetUniformLocation(ourShader->Program, "layer"), 2);
                    glProgramUniform1i(ourShader->Program, glGetUniformLocation(ourShader->Program, "layer"), i);


                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, frameWidth,frameHeight, 1, GL_RED, GL_UNSIGNED_BYTE, 0);
                    glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureY"), 0);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


                    glActiveTexture(GL_TEXTURE1);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, frameWidth/2,frameHeight/2, 1, GL_RG, GL_UNSIGNED_BYTE, 0);
                    glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureUV"), 1);
                    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                    continue;
                }
                pFrame=(*pFrameQueueVecPtr_)[j].first.front();
                (*pFrameQueueVecPtr_)[j].first.pop();
                realcount++;
                if(pFrame->width!=1920/sqrt(splitNum_) || pFrame->height!=1080/sqrt(splitNum_))
                {
                    av_frame_free(&pFrame);
                    continue;
                }

                if(frameWidth!=pFrame->width || frameHeight!=pFrame->height)
                {
                    std::cout<<i<<"\t"<<frameWidth<<"\t"<<frameHeight<<pFrame->width<<"\t"<<pFrame->height<<std::endl;
                    frameWidth=pFrame->width;
                    frameHeight=pFrame->height;
                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, splitNum_, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, splitNum_, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

                }

                glBindVertexArray(VAO[i]);

                glActiveTexture(GL_TEXTURE0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptr)
                {

                    memcpy(ptr,pFrame->data[0],frameWidth*frameHeight);

                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
                }
                glUniform1i(glGetUniformLocation(ourShader->Program, "layer"), 2);
                glProgramUniform1i(ourShader->Program, glGetUniformLocation(ourShader->Program, "layer"), i);


                glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, frameWidth,frameHeight, 1, GL_RED, GL_UNSIGNED_BYTE, 0);
                glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureY"), 0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


                glActiveTexture(GL_TEXTURE1);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
                GLubyte* ptrUV = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptrUV)
                {
                    memcpy(ptrUV,pFrame->data[1],frameWidth*frameHeight/2);
                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
                }
                av_frame_free(&pFrame);
                glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, frameWidth/2,frameHeight/2, 1, GL_RG, GL_UNSIGNED_BYTE, 0);
                glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureUV"), 1);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }


            glBindVertexArray(0);



            framecount++;
            gettimeofday(&t_end,NULL);
            long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
            cost_time = end - start;
            if(cost_time/1000.0 > 1)
            {
                fps = (float)framecount*1000/cost_time;
                printf("index %d  fps:    %f    real fps:    %f      frameWidth:%d\n",index,(float)framecount*1000/cost_time,(float)realcount*1000/cost_time,frameWidth);
                gettimeofday(&t_start,NULL);
                start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
                framecount = 0;
                realcount = 0;
            }
//    Display *dpy = glXGetCurrentDisplay();
//    GLXDrawable drawable = glXGetCurrentDrawable();
//    const int interval = 1;
            renderTexts(splitNum_,fps);
            //glXSwapBuffers(glXGetCurrentDisplay(),glXGetCurrentDrawable());
            //glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);
            if(index!=0)
                usleep(20000);
            //glfwPollEvents();

        }
    }
}

void viewer::displayFun()
{
    //glfwMakeContextCurrent(window);
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

void viewer::renderTexts(int splitNum,float fps)
{
    assert(splitNum==splitNum_);
    //std::cout<<"(*videoPositionVecPtr_) = "<<(*videoPositionVecPtr_)[0][0]<<std::endl;
    int j = 0;
    switch(splitNum)
    {
    case 1:
    {
        j = (*videoPositionVecPtr_)[index][0];
        if(j>=0)
        {
            std::string s = (*pFrameQueueVecPtr_)[j].second;
            tr->RenderText(s,(GLfloat)0.0f, (GLfloat)WINDOW_HEIGHT-35, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
        }

        break;
    }
    case 4:
    {
        for(int i = 0; i<splitNum_; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second;
            tr->RenderText(s,(GLfloat)(WINDOW_WIDTH*(i%2)/2+25.0f),(GLfloat)(WINDOW_HEIGHT/(int(i/2)+1)-25.0f),0.5f, glm::vec3(0.5, 0.8f, 0.2f));
        }
        break;
    }
    case 9:
    {
        for(int i = 0; i<splitNum_; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second;
            tr->RenderText(s,(GLfloat)(WINDOW_WIDTH*(i%3)/3+25.0f),(GLfloat)(WINDOW_HEIGHT*(3-int(i/3))/3-25.0f),0.4f, glm::vec3(0.5, 0.8f, 0.2f));
        }
        break;
    }
    case 16:
    {
        for(int i = 0; i<splitNum_; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second;
            tr->RenderText(s,(GLfloat)(WINDOW_WIDTH*(i%4)/4+25.0f),(GLfloat)(WINDOW_HEIGHT*(4-int(i/4))/4-25.0f),0.3f, glm::vec3(0.5, 0.8f, 0.2f));
        }
        break;
    }
    default:
    {
        break;
    }
    }

    if(showFPS)
    {
        if(index == 0)
        {
            char fpsStr[100];
            sprintf(fpsStr,"v0 FPS: %f ",fps);
            tr->RenderText(fpsStr,(GLfloat)(WINDOW_WIDTH-300),(GLfloat)(WINDOW_HEIGHT-25),0.5f, glm::vec3(0.5, 0.1f, 0.5f));
        }
        else
        {
            char fpsStr[100];
            sprintf(fpsStr,"v1 FPS: %f ",fps);
            tr->RenderText(fpsStr,(GLfloat)(WINDOW_WIDTH-300),(GLfloat)(WINDOW_HEIGHT-25),0.5f, glm::vec3(0.5, 0.1f, 0.5f));
        }

    }



    ourShader->Use();

}
