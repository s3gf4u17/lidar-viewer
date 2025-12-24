// compiled with cmake and linked later
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "lib.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Dodaj portable-file-dialogs
#include "portable-file-dialogs.h"

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 1000
#define DEFAULT_SCALE 16

float rotationx = 0.0;
float rotationy = 0.0;
float scale = 3.0;
bool rotating = false;
bool moving = false;
float pointsize = 1.0;

double movex = 0.0;
double movey = 0.0;
double lastx,lasty;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scale = (yoffset>0) ? scale*1.2 : (yoffset<0) ? scale/1.5 : scale;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        glfwSetCursor(window, cursor);
        rotating = true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        glfwSetCursor(window, NULL);
        rotating = false;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        glfwSetCursor(window, cursor);
        moving = true;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        glfwSetCursor(window, NULL);
        moving = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (rotating) {
        rotationx = rotationx + (lastx-xpos)/2;
    }
    if (moving) {
        movex = movex + std::max(std::min((xpos-lastx)/scale,1.0),-1.0);
        movey = movey + std::max(std::min((ypos-lasty)/scale,1.0),-1.0);
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
    "   gl_Position = projection * view * model * vec4(aPos.x, aPos.z, aPos.y, 1.0);\n"
    "   myColor = aCol;\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 myColor;\n"
    "void main()\n"
    "{\n"
    "   float intensity = myColor.r / 256.0f;\n"  // Normalize intensity
    "   vec3 color;\n"
    "   if (intensity < 0.25) {\n"
    "       color = mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), intensity * 4.0);\n"  // Blue to Cyan
    "   } else if (intensity < 0.5) {\n"
    "       color = mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0), (intensity - 0.25) * 4.0);\n"  // Cyan to Green
    "   } else if (intensity < 0.75) {\n"
    "       color = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (intensity - 0.5) * 4.0);\n"  // Green to Yellow
    "   } else {\n"
    "       color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), (intensity - 0.75) * 4.0);\n"  // Yellow to Red
    "   }\n"
    "   FragColor = vec4(color, 1.0);"
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

    // imgui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();(void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Inicjalizacja bez pliku
    LasFile* lasfile = nullptr;
    std::string currentFilePath = "";
    bool fileLoaded = false;
    
    unsigned int VBO = 0, VAO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glPointSize(pointsize);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPointSize(pointsize);

        // Rysowanie tylko jeśli plik jest załadowany
        if (fileLoaded && lasfile != nullptr) {
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glm::mat4 projectionM = glm::ortho(-display_w/2.0f, display_w/2.0f, display_h/2.0f, -display_h/2.0f, -100000.0f, 100000.0f);
            glm::mat4 viewM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
            viewM = glm::translate(viewM, glm::vec3(movex, movey, 0.0f));
            glm::mat4 modelM = glm::mat4(1.0f);
            modelM = glm::scale(modelM, glm::vec3(scale));
            modelM = glm::rotate(modelM, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            modelM = glm::rotate(modelM, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelM = glm::rotate(modelM, glm::radians(rotationx), glm::vec3(0.0f, 1.0f, 0.0f));
            modelM = glm::translate(modelM, glm::vec3(
                -(lasfile->header.x_min + lasfile->header.x_max) / 2.0,
                -(lasfile->header.z_min + lasfile->header.z_max) / 2.0,
                -(lasfile->header.y_min + lasfile->header.y_max) / 2.0
            ));
            
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelM));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &viewM[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionM));

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_POINTS, 0, lasfile->header.numOfPointRecords);
        }

        glfwPollEvents();

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | 
                                            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | 
                                            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            ImGui::Begin("control", (bool*)true, window_flags);
            
            // Przycisk ładowania pliku
            if (ImGui::Button("Załaduj plik LAS")) {
                auto selection = pfd::open_file("Wybierz plik LAS", ".", {"Pliki LAS", "*.las"}).result();
                if (!selection.empty()) {
                    std::string filepath = selection[0];
                    
                    // Usuń stary plik jeśli istnieje
                    if (lasfile != nullptr) {
                        delete lasfile;
                        lasfile = nullptr;
                    }
                    
                    try {
                        // Załaduj nowy plik
                        lasfile = new LasFile(filepath);
                        currentFilePath = filepath;
                        std::cout << *lasfile << std::endl;
                        
                        // Aktualizuj bufory OpenGL
                        glBindVertexArray(VAO);
                        glBindBuffer(GL_ARRAY_BUFFER, VBO);
                        glBufferData(GL_ARRAY_BUFFER, lasfile->data_stream.size() * sizeof(float), 
                                    &lasfile->data_stream[0], GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
                        glEnableVertexAttribArray(1);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        glBindVertexArray(0);
                        
                        fileLoaded = true;
                        
                        // Reset widoku
                        rotationx = 0.0;
                        scale = DEFAULT_SCALE;
                        movex = 0.0;
                        movey = 0.0;
                    } catch (const std::exception& e) {
                        std::cerr << "Błąd ładowania pliku: " << e.what() << std::endl;
                        fileLoaded = false;
                    }
                }
            }
            
            ImGui::Separator();
            
            // Informacje o pliku
            if (fileLoaded && lasfile != nullptr) {
                ImGui::Text("Załadowany plik:");
                ImGui::TextWrapped("%s", currentFilePath.c_str());
                ImGui::Text("Punktów: %u", lasfile->header.numOfPointRecords);
                ImGui::Separator();
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Brak załadowanego pliku");
                ImGui::Separator();
            }
            
            // Kontrolki widoku
            ImGui::Text("Widok:");
            if (ImGui::Button("Reset")) {
                rotationx = 0.0;
                scale = 3.0;
                movex = 0.0;
                movey = 0.0;
            }

            ImGui::SliderFloat("Rozmiar punktu", &pointsize, 1.0f, 3.0f);
            
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glFinish();
        glfwSwapBuffers(window);
        glFinish();
        glFlush();
    }

    // Czyszczenie
    if (lasfile != nullptr) {
        delete lasfile;
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    return 0;
}