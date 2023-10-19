// compiled with cmake and linked later
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "lib.h"

int SCREEN_WIDTH = 1400;
int SCREEN_HEIGHT = 1000;

float rotation = 0.0;
float scale = 0.1;
bool rotating = false;
bool moving = false;

double movex = 0.0;
double movey = 0.0;
double lastx,lasty;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scale = (yoffset>0) ? scale*1.5 : (yoffset<0) ? scale/1.5 : scale;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        rotating = true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        rotating = false;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        moving = true;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        moving = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (rotating) {
        rotation = rotation + (lastx-xpos)/2;
    }
    if (moving) {
        movex = movex + (xpos-lastx)/scale/500;
        movey = movey + (lasty-ypos)/scale/500;
    }
    lastx = xpos;
    lasty = ypos;
}


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 myColor;"
    "void main()\n"
    "{\n"
    // "gl_PointSize = 10.0;\n"
    "   gl_Position = projection * view * model * vec4(aPos.x, aPos.z, aPos.y, 1.0);\n"
    "   myColor = aCol;\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 myColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(myColor/256.0f, 1.0f);"
    "}\n\0";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LAS visualizer", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    LasFile lasfile("../data/lidar_san.las");
    std::cout << lasfile << std::endl;
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, lasfile.data_stream.size()*sizeof(float), &lasfile.data_stream[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(1);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f,0.0f,0.0f,1.0f);

        glm::mat4 projectionM = glm::ortho(-SCREEN_WIDTH/2.0f, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, -SCREEN_HEIGHT/2.0f, -1000.0f, 1000.0f);//glm::perspective(glm::radians(45.0f),(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,0.1f,100.0f);
        glm::mat4 viewM = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,3.0f));
        glm::mat4 modelM = glm::mat4(1.0f);
        modelM = glm::scale(modelM,glm::vec3(scale));
        modelM = glm::rotate(modelM,glm::radians(180.0f),glm::vec3(0.0f,0.0f,1.0f));
        modelM = glm::rotate(modelM,glm::radians(30.0f),glm::vec3(1.0f,0.0f,0.0f));
        modelM = glm::translate(modelM,glm::vec3(-(lasfile.header.x_min+lasfile.header.x_max)/2.0,-(lasfile.header.z_min+lasfile.header.z_max)/2.0,-(lasfile.header.y_min+lasfile.header.y_max)/2.0));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(modelM));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,&viewM[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projectionM));
    
        // glm::mat4 transM = glm::mat4(1.0f);
        // transM = glm::scale(transM,glm::vec3(scale));
        // // transM = glm::scale(transM,glm::vec3(0.0015));
        // transM = glm::rotate(transM,glm::radians(30.0f),glm::vec3(-1.0,0.0,0.0));
        // // transM = glm::rotate(transM,(float)glfwGetTime()/5, glm::vec3(0.0f, 1.0f, 0.0f));
        // transM = glm::rotate(transM,glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        // transM = glm::translate(transM,glm::vec3(-(lasfile.header.x_min+lasfile.header.x_max)/2.0,-(lasfile.header.z_min+lasfile.header.z_max)/2.0,-(lasfile.header.y_min+lasfile.header.y_max)/2.0));
        // transM = glm::translate(transM,glm::vec3(movex,movey,0.0));
        // glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"transform"),1,GL_FALSE,glm::value_ptr(transM));

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, lasfile.header.numOfPointRecords);
        glFinish();
        glfwSwapBuffers(window);
        glFinish();
        glfwPollEvents();
        glFlush();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}