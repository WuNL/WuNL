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
    colorVec.push_back(glm::vec3(1.0, 0.0f, 0.0f));
    colorVec.push_back(glm::vec3(0.0, 1.0f, 0.0f));
    colorVec.push_back(glm::vec3(0.0, 0.0f, 1.0f));
    colorVec.push_back(glm::vec3(0.2, 0.1f, 0.5f));
    colorVec.push_back(glm::vec3(1.0, 0.4f, 0.1f));
    curSize = 1;
    curLocation = 0;
    curColor = 0;
    leftOffset = 35.0f;
    topOffset = 35.0f;

    w = width;
    h = height;
    index = ind;
    autoSwitch = autoS;
    active = true;

    frameWidth=1920;
    frameHeight=1080;
    splitNum_ = WINDOW_STYLE;
    splitNum_old = splitNum_;
    for(int i=0; i<20; ++i)
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


    switch(curLocation)
    {
    //left top
    case 0:
    {
        leftOffset = 35.0f;
        topOffset = -35.0f;
        break;
    }
    // left bottom
    case 1:
    {
        leftOffset = 35.0f;
        topOffset = -(w_height/sqrt(splitNum_))+35.0f;
        break;
    }

    }
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
    GLfloat cor4X7[4][7][2]=
    {
        { {-1.0f,0.0f},{-1.0f*2.0f/3.0f,0.0f}, {-1.0f/3.0f,0.0f},    {0.0f,0.0f},    {1.0f/3.0f,0.0f}, {1.0f*2.0f/3.0f,0.0f}, {1.0f,0.0f} },
        { {-1.0f,-1.0f/3.0f},{-1.0f*2.0f/3.0f,-1.0f/3.0f}, {-1.0f/3.0f,-1.0f/3.0f},    {0.0f,-1.0f/3.0f},    {1.0f/3.0f,-1.0f/3.0f}, {1.0f*2.0f/3.0f,-1.0f/3.0f}, {1.0f,-1.0f/3.0f} },
        { {-1.0f,-2.0f/3.0f},{-1.0f*2.0f/3.0f,-2.0f/3.0f}, {-1.0f/3.0f,-2.0f/3.0f},    {0.0f,-2.0f/3.0f},    {1.0f/3.0f,-2.0f/3.0f}, {1.0f*2.0f/3.0f,-2.0f/3.0f}, {1.0f,-2.0f/3.0f} },
        { {-1.0f,-1.0f},{-1.0f*2.0f/3.0f,-1.0f}, {-1.0f/3.0f,-1.0f},    {0.0f,-1.0f},    {1.0f/3.0f,-1.0f}, {1.0f*2.0f/3.0f,-1.0f}, {1.0f,-1.0f} }
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
    case 20:
    {
        GLfloat tmpVec0[20]=
        {
            0.0f,    1.0f,     0.0f, 1.0f, 1.0f,            // Top Right
            0.0f,    0.0f,     0.0f, 1.0f, 0.0f,            // Bottom Right
            -1.0f,   0.0f,     0.0f, 0.0f, 0.0f,            // Bottom Left
            -1.0f,   1.0f,     0.0f, 0.0f, 1.0f            // Top Left
        };
        memcpy(verticesVec[0],tmpVec0,20*sizeof(GLfloat));

        GLfloat tmpVec1[20]=
        {
            1.0f,    1.0f,     0.0f, 1.0f, 1.0f,            // Top Right
            1.0f,    0.0f,     0.0f, 1.0f, 0.0f,            // Bottom Right
            0.0f,    0.0f,     0.0f, 0.0f, 0.0f,            // Bottom Left
            0.0f,   1.0f,      0.0f, 0.0f, 1.0f            // Top Left
        };
        memcpy(verticesVec[1],tmpVec1,20*sizeof(GLfloat));

        for(int i=0; i<18; ++i)
        {

            int y = i%6;
            int x = i/6;
            GLfloat tmpVec[20]=
            {
                cor4X7[x][y+1][0],    cor4X7[x][y+1][1],     0.0f, 1.0f, 1.0f,            // Top Right
                cor4X7[x+1][y+1][0],  cor4X7[x+1][y+1][1],   0.0f, 1.0f, 0.0f,            // Bottom Right
                cor4X7[x+1][y][0],    cor4X7[x+1][y][1],     0.0f, 0.0f, 0.0f,            // Bottom Left
                cor4X7[x][y][0],      cor4X7[x][y][1],       0.0f, 0.0f, 1.0f            // Top Left
            };
            memcpy(verticesVec[i+2],tmpVec,20*sizeof(GLfloat));
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

    pthread_t thread = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    thread = pthread_self();
    CPU_SET(15 + index + 1, &cpuset);
    int rc = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        std::cout << "Error calling pthread_setaffinity_np !!! ";



    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    const GLFWvidmode* mode = glfwGetVideoMode(monitors[index]);
    w_width = mode->width;
    w_height = mode->height;
//    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
//    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
//    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
//    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
//    glfwWindowHint(GLFW_ICONIFIED,GL_FALSE);
//    glfwWindowHint(GLFW_FOCUSED,GL_FALSE);
//    glfwWindowHint(GLFW_AUTO_ICONIFY,GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//    glfwWindowHint(GLFW_MAXIMIZED, GL_FALSE);
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
        window = glfwCreateWindow(mode->width, mode->height, s,NULL, NULL);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetWindowPos(window, index*1920, 0);
        //glfwSetWindowPos(window,1440*index,0);
//        glfwSetWindowMonitor(window, NULL, 1920*index, 0, mode->width, mode->height, 0);
        glfwMakeContextCurrent(window);
//    // set vsync. 0:off max 1000+fps 1: on max 60fps 2: on max 30fps
//        if(index==0)
        glfwSwapInterval(-1);
        //glfwSetWindowMonitor(window, monitors[index], 0, 0, mode->width, mode->height, mode->refreshRate);

    }
    else
    {
//        window = glfwCreateWindow(w, h, s, NULL, NULL);
//        glfwSetWindowMonitor(window,monitors[index],0,0,w,h,mode->refreshRate);

    }

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, mode->width, mode->height);

//    Display *dpy = glXGetCurrentDisplay();
//    GLXDrawable drawable = glXGetCurrentDrawable();
//    const int interval = 1;
//
//    glXSwapIntervalEXT(dpy, drawable, interval);


    if(autoSwitch)
    {
        tr = new textRender(mode->width, mode->height,"/home/sdt/workspace/WuNL/SourceHanSerifCN-Bold.otf");
    }
    else
    {
        tr = new textRender(w,h,"/home/sdt/workspace/WuNL/SourceHanSerifCN-Bold.otf");
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
    glGenBuffers(20, pboIds);
    glGenBuffers(20, pboUV);
    glGenVertexArrays(20, VAO);
    glGenBuffers(20, VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(1, &texture);
    glGenTextures(1, &textureUV);

    ourShader = new Shader("nv12.vs", "nv12.frag");
    ourShader->Use();

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
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, 20, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
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
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, 20, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

    //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);



// Initialize the AVFrame
    AVFrame* frame = av_frame_alloc();
    frame->width = 1920;
    frame->height = 1080;
    frame->format = AV_PIX_FMT_NV12;



//    AVFrame* pFrameYUV1080P = av_frame_alloc();
    unsigned char *out_buffer1080P;
    out_buffer1080P=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV12,  1920,1080,1));
    memset(out_buffer1080P,0x01,avpicture_get_size(AV_PIX_FMT_NV12, 1920, 1080)*2/3);
    memset(out_buffer1080P+avpicture_get_size(AV_PIX_FMT_NV12, 1920, 1080)*2/3,0x80,avpicture_get_size(AV_PIX_FMT_NV12, 1920, 1080)/3);
    av_image_fill_arrays(frame->data, frame->linesize,out_buffer1080P,
                         AV_PIX_FMT_NV12,1920,1080,1);

    for(int i=0; i<20; ++i)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, frame->data[0], GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE/2, frame->data[1], GL_STREAM_DRAW);
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


//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
//        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
//        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV[i]);
//        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE/2, 0, GL_STREAM_DRAW);
    }

    int framecount = 0;
    int realcount = 0;
    float fps = 0.0f;
    struct timeval t_start,t_end;
    long cost_time=0;
    gettimeofday(&t_start,NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
    long startTime = start;
    printf("start time:%ld ms\n",start);

    struct timeval renderTime;
    uint64_t renderTimeMS;

    while (active)
    {

//        glfwWaitEvents();
        if(splitNum_!=splitNum_old)
        {
            setStyleInter();
            splitNum_old = splitNum_;
        }
//        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        gettimeofday(&renderTime,NULL);
        renderTimeMS = ((uint64_t)renderTime.tv_sec)*1000+(uint64_t)renderTime.tv_usec/1000;


        //if(!(*pFrameQueueVecPtr_)[0].empty() || !(*pFrameQueueVecPtr_)[1].empty())
        {
            //int i = 1;
            for(int i = 0; i<splitNum_; ++i)
            {
                int j = (*videoPositionVecPtr_)[index][i];
//                printf("j=%d \n",j);
                if( j<0 || (*pFrameQueueVecPtr_)[j].first.empty())
                {
                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, splitNum_, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, splitNum_, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                    continue;
                }

                mutexPtr_->lock();

                AVFrame* pFrameOri=(*pFrameQueueVecPtr_)[j].first.front();

                if(abs(renderTimeMS - pFrameOri->pts) > 2000)
                {
                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, splitNum_, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, splitNum_, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                    mutexPtr_->unlock();
                    continue;
                }


                AVFrame *pFrame = av_frame_alloc();

                pFrame->format = pFrameOri->format;
                pFrame->width = pFrameOri->width;
                pFrame->height = pFrameOri->height;
                av_frame_get_buffer(pFrame, 32);
                av_frame_copy(pFrame, pFrameOri);
                av_frame_copy_props(pFrame, pFrameOri);


                mutexPtr_->unlock();


                realcount++;

                if(frameWidth!=pFrame->width || frameHeight!=pFrame->height)
                {
                    std::cout<<index<<"\t"<<i<<"\t"<<frameWidth<<"\t"<<frameHeight<<"\t"<<pFrame->width<<"\t"<<pFrame->height<<std::endl;
                    frameWidth=pFrame->width;
                    frameHeight=pFrame->height;
                    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, frameWidth, frameHeight, splitNum_, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);

                    glBindTexture(GL_TEXTURE_2D_ARRAY, textureUV);

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG, frameWidth/2,frameHeight/2, splitNum_, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

//                    if(needFreeFrame)
                    av_frame_free(&pFrame);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindVertexArray(0);
                    continue;

                }

                glBindVertexArray(VAO[i]);

                glActiveTexture(GL_TEXTURE0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
                GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
                if(ptr)
                {
                    if(frameWidth==0 || frameHeight==0)
                    {
                        printf("viewer continue!\n");
//                    if(needFreeFrame)
                        av_frame_free(&pFrame);
                        continue;
                    }

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
                    if(frameWidth==0 || frameHeight==0)
                    {
                        printf("viewer continue!\n");
//                    if(needFreeFrame)
                        av_frame_free(&pFrame);
                        continue;
                    }
                    memcpy(ptrUV,pFrame->data[1],frameWidth*frameHeight/2);
                    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
                }
//                    if(needFreeFrame)
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
                //printf("index %d  fps:    %f    real fps:    %f      frameWidth:%d     cost:%ld\n",index,(float)framecount*1000/cost_time,(float)realcount*1000/cost_time,frameWidth,(end-startTime)/1000);
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
//            usleep(100);

            glfwPollEvents();

        }
    }
}

void viewer::displayFun()
{
    //glfwMakeContextCurrent(window);
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, w_width, w_height);

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
    if(!showtext)
        return;
    assert(splitNum==splitNum_);
    //std::cout<<"(*videoPositionVecPtr_) = "<<(*videoPositionVecPtr_)[0][0]<<std::endl;
    int j = 0;
//    if(w_width==1920)
//    {
//        topOffset*=1.5;
//        leftOffset*=1.5;
//    }
    switch(splitNum)
    {
    case 1:
    {
        j = (*videoPositionVecPtr_)[index][0];
        if(j>=0)
        {
            std::string s = (*pFrameQueueVecPtr_)[j].second.first;
            tr->RenderText(s,(GLfloat)(leftOffset), (GLfloat)w_height+topOffset, fontSize, colorVec[curColor]);
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
            std::string s = (*pFrameQueueVecPtr_)[j].second.first;
            tr->RenderText(s,(GLfloat)(w_width*(i%2)/2+leftOffset),(GLfloat)(w_height/(int(i/2)+1)+topOffset),fontSize, colorVec[curColor]);
        }
        break;
    }
    case 9:
    {
        float fontSize_9 = fontSize*0.8f;
        float leftOffset_9 = 20.5f;
        for(int i = 0; i<splitNum_; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second.first;
            tr->RenderText(s,(GLfloat)(w_width*(i%3)/3+leftOffset_9),(GLfloat)(w_height*(3-int(i/3))/3)+topOffset,fontSize_9, colorVec[curColor]);
        }
        break;
    }
    case 16:
    {
        float fontSize_16 = fontSize*0.7f;
        float leftOffset_16 = 17.5f;
        for(int i = 0; i<splitNum_; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second.first;
            tr->RenderText(s,(GLfloat)(w_width*(i%4)/4+leftOffset_16),(GLfloat)(w_height*(4-int(i/4))/4)+topOffset,fontSize_16, colorVec[curColor]);
        }
        break;
    }

    case 20:
    {
        for(int i = 0; i<2; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second.first;
            tr->RenderText(s,(GLfloat)(w_width*(i%2)/2+leftOffset),(GLfloat)(w_height/(int(i/2)+1)+topOffset),fontSize, colorVec[curColor]);
        }
        float leftOffset_20 = 17.5f;
        float topOffset_20 = -w_height/2 - 25.0f;
        float fontSize_20 = fontSize*0.5f;
        for(int i = 0; i<splitNum_-2; ++i)
        {
            j = (*videoPositionVecPtr_)[index][i+2];
            if(j<0)
                continue;
            std::string s = (*pFrameQueueVecPtr_)[j].second.first;
            tr->RenderText(s,(GLfloat)(w_width*(i%6)/6+leftOffset_20),(GLfloat)(w_height*(6-int(i/6))/6)+topOffset_20,fontSize_20, colorVec[curColor]);
        }
        break;
    }
    default:
    {
        break;
    }
    }

    if(showfps)
    {
        if(index == 0)
        {
            char fpsStr[100];
            sprintf(fpsStr,"v0 FPS: %f ",fps);
            tr->RenderText(fpsStr,(GLfloat)(w_width-300),(GLfloat)(w_height-25),0.5f, glm::vec3(0.5, 0.1f, 0.5f));
        }
        else
        {
            char fpsStr[100];
            sprintf(fpsStr,"v1 FPS: %f ",fps);
            tr->RenderText(fpsStr,(GLfloat)(w_width-300),(GLfloat)(w_height-25),0.5f, glm::vec3(0.5, 0.1f, 0.5f));
        }

    }



    ourShader->Use();

}
