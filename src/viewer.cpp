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
     // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        // Positions                           // Texture Coords
        1.0f/3.0f,  1.0f/3.0f,  0.0f,        1.0f, 1.0f,            // Top Right
        1.0f/3.0f,  -1.0f/3.0f, 0.0f,        1.0f, 0.0f, // Bottom Right
        -1.0f/3.0f, -1.0f/3.0f, 0.0f,        0.0f, 0.0f, // Bottom Left
        -1.0f/3.0f,  1.0f/3.0f, 0.0f,        0.0f, 1.0f  // Top Left
    };
    GLuint indices[] =    // Note that we start from 0!
    {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    glGenBuffers(1, &pboIds);
    glGenBuffers(1, &pboUV);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(1, &texture);

    ourShader = new Shader("nv12.vs", "nv12.frag");

    AVFrame	*pFrame ;
    int DATA_SIZE = SWS_WIDTH*SWS_HEIGHT;
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE/2, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // TexCoord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0); // Unbind VAO

        // Load and create a texture
        glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SWS_WIDTH,SWS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE,NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

         // Load and create a texture
        glBindTexture(GL_TEXTURE_2D, textureUV); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, SWS_WIDTH/2,SWS_HEIGHT/2, 0, GL_RG, GL_UNSIGNED_BYTE,NULL);

        glBindTexture(GL_TEXTURE_2D, 0);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(!(*pFrameQueueVecPtr_)[0].empty())
        {
            pFrame=(*pFrameQueueVecPtr_)[0].front();
            (*pFrameQueueVecPtr_)[0].pop();
            ourShader->Use();

            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
            GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
            if(ptr)
            {
                memcpy(ptr,pFrame->data[0],SWS_WIDTH*SWS_HEIGHT);
                //updateImage(ptr,0,SWS_HEIGHT,SWS_WIDTH,SWS_HEIGHT/2,pFrame->data[1]);
                //updateImage(ptr,SWS_WIDTH/2,SWS_HEIGHT,SWS_WIDTH/2,SWS_HEIGHT/2,pFrame->data[2]);
                glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
            }
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SWS_WIDTH,SWS_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, 0);
            glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureY"), 0);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);




            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboUV);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE/2, 0, GL_STREAM_DRAW);
            GLubyte* ptrUV = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
            if(ptrUV)
            {
                memcpy(ptrUV,pFrame->data[1],SWS_WIDTH*SWS_HEIGHT/2);
                //updateImage(ptr,0,SWS_HEIGHT,SWS_WIDTH,SWS_HEIGHT/2,pFrame->data[1]);
                //updateImage(ptr,SWS_WIDTH/2,SWS_HEIGHT,SWS_WIDTH/2,SWS_HEIGHT/2,pFrame->data[2]);
                glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
            }
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textureUV);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SWS_WIDTH/2,SWS_HEIGHT/2, GL_RG, GL_UNSIGNED_BYTE, 0);
            glUniform1i(glGetUniformLocation(ourShader->Program, "ourTextureUV"), 1);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//            lock.lock();
            if(pFrame)
            av_frame_free(&pFrame);
//            lock.unlock();
            glBindVertexArray(0);
            glfwPollEvents();
            glfwSwapBuffers(window);
            usleep(30000);
        }
        else
        {
            // Bind Texture
            //glBindTexture(GL_TEXTURE_2D, texture);
            ourShader->Use();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//            glBindVertexArray(0);
            glfwPollEvents();
            //Swap the screen buffers
            glfwSwapBuffers(window);
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
