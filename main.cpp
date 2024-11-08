#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>
#include <vector>
#include <iomanip> // 用于 std::setprecision
#include <chrono>  // 用于时间计算

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateFrameRate(GLFWwindow* window, double deltaTime);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// 相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// float lastX = SCR_WIDTH / 2.0f;
// float lastY = SCR_HEIGHT / 2.0f;
float yaw = 0.0f;
float pitch = 0.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// 体纹理参数
const unsigned int x_dim = 584;
const unsigned int y_dim = 584;
const unsigned int z_dim = 631;
// 要不要重新处理
//#define REPROCESS

// // 写入三维向量到二进制文件
// void writeToFile(const std::string& filename, const std::vector<std::vector<std::vector<glm::vec4>>>& data) {
//     std::ofstream file(filename, std::ios::binary);
    
//     if (!file) {
//         std::cerr << "无法打开文件进行写入!" << std::endl;
//         return;
//     }

//     size_t x_dim = data.size();
//     size_t y_dim = data[0].size(); // 假设所有的向量都有相同的 y 维
//     size_t z_dim = data[0][0].size(); // 假设所有的向量都有相同的 z 维

//     // 写入维度信息
//     file.write(reinterpret_cast<char*>(&x_dim), sizeof(size_t));
//     file.write(reinterpret_cast<char*>(&y_dim), sizeof(size_t));
//     file.write(reinterpret_cast<char*>(&z_dim), sizeof(size_t));

//     // 写入数据
//     for (size_t x = 0; x < x_dim; ++x) {
//         for (size_t y = 0; y < y_dim; ++y) {
//             file.write(reinterpret_cast<const char*>(data[x][y].data()), z_dim * sizeof(glm::vec4));
//         }
//     }

//     file.close();
// }

// // 从二进制文件读取三维向量
// void readFromFile(const std::string& filename, std::vector<std::vector<std::vector<glm::vec4>>>& data) {
//     std::ifstream file(filename, std::ios::binary);
    
//     if (!file) {
//         std::cerr << "无法打开文件进行读取!" << std::endl;
//         return;
//     }

//     size_t x_dim, y_dim, z_dim;

//     // 读取维度信息
//     file.read(reinterpret_cast<char*>(&x_dim), sizeof(size_t));
//     file.read(reinterpret_cast<char*>(&y_dim), sizeof(size_t));
//     file.read(reinterpret_cast<char*>(&z_dim), sizeof(size_t));

//     // 重新调整数据结构
//     data.resize(x_dim, std::vector<std::vector<glm::vec4>>(y_dim, std::vector<glm::vec4>(z_dim)));

//     // 读取数据
//     for (size_t x = 0; x < x_dim; ++x) {
//         for (size_t y = 0; y < y_dim; ++y) {
//             file.read(reinterpret_cast<char*>(data[x][y].data()), z_dim * sizeof(glm::vec4));
//         }
//     }

//     file.close();
// }

// // 体数据转换 -> 放到 shader 中处理
// std::vector<std::vector<std::vector<glm::vec4>>> transformed_data(x_dim, std::vector<std::vector<glm::vec4>>(y_dim, std::vector<glm::vec4>(z_dim)));
// glm::vec4 color_func[] = 
// {
//     glm::vec4(0.0f, 0.0f, 0.0f, -3024.0f),
//     glm::vec4(0.62f, 0.36f, 0.18f, -800.0f),
//     glm::vec4(0.88f, 0.60f, 0.29f, 0.0f),
//     glm::vec4(1.0f, 1.0f, 1.0f, 3071.0f)
// };
// glm::vec2 opacity_func[] =
// {
//     glm::vec2(0.0f, -3024.0f),
//     glm::vec2(0.0f, -800.0f),
//     glm::vec2(0.4f, 300.0f),
//     glm::vec2(0.8f, 3071.0f)
// };
// glm::vec3 color_func_linear(short value)
// {
//     for (int i = 0; i < 4; ++i)
//     {
//         if (value <= color_func[i].w)
//         {
//             return glm::vec3(glm::mix(color_func[i], color_func[i + 1], (value - color_func[i].w) / (color_func[i + 1].w - color_func[i].w)));
//         }
//     }
//     return glm::vec3(1.0f);
// }
// float opacity_func_linear(short value)
// {
//     for (int i = 0; i < 4; ++i)
//     {
//         if (value <= opacity_func[i].y)
//         {
//             return glm::mix(opacity_func[i], opacity_func[i + 1], (value - opacity_func[i].y) / (opacity_func[i + 1].y - opacity_func[i].y)).x;
//         }
//     }
//     return 1.0f;
// }
// void transform_fuction()
// {
//     for (unsigned int x = 0; x < x_dim; ++x)
//     {
//         for (unsigned int y = 0; y < y_dim; ++y)
//         {
//             for (unsigned int z = 0; z < z_dim; ++z)
//             {
//                 short value = raw_data[x][y][z];
//                 transformed_data[x][y][z] = glm::vec4(color_func_linear(value), opacity_func_linear(value));
//             }
//         }
//     }
// }


int main()
{
//读取和转换体数据
#pragma region
// #ifndef REPROCESS
//     readFromFile("../transformed_data.bin", transformed_data);
// #else
    std::ifstream raw_file("../cbct.raw", std::ios::binary);
    if (!raw_file.is_open())
    {
        throw std::runtime_error("Failed to open raw file");
    }

    // 计算总元素数量
    size_t total_elements = x_dim * y_dim * z_dim;
    std::vector<int16_t> raw_data(total_elements);
    std::vector<float> volume_data(total_elements);

    // 读取文件到 raw_data
    raw_file.read(reinterpret_cast<char *>(raw_data.data()), total_elements * sizeof(int16_t));
    raw_file.close();

    for (int i = 0; i < raw_data.size(); ++i)
    {
        //raw_data[i] += 1000;
        volume_data[i] = static_cast<float>(raw_data[i]);
    }
//     transform_fuction();
//     writeToFile("../transformed_data.bin", transformed_data);
// #endif
#pragma endregion

// 窗口和正方体
#pragma region
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Volume Rendering", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 初始化GLEW
    glewExperimental = GL_TRUE; // 启用现代特性
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW 初始化失败!" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader.vs", "shader.fs"); // you can name your shader files however you like

    float vertices[] = {
        // positions          // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    };
    unsigned int indices[] = {
        //前
        0, 1, 2,
        2, 3, 0,
        //后
        4, 5, 6,
        6, 7, 4,
        //左
        4, 0, 3,
        3, 7, 4,
        //右
        1, 5, 6,
        6, 2, 1,
        //上
        3, 2, 6,
        6, 7, 3,
        //下
        0, 4, 5,
        5, 1, 0
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#pragma endregion

    // 体纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, x_dim, y_dim, z_dim, 0, GL_RGBA, GL_FLOAT, &transformed_data[0][0][0]);
    //glTexImage3D(GL_TEXTURE_3D, 0, GL_R16I, x_dim, y_dim, z_dim, 0, GL_RED_INTEGER, GL_SHORT, &raw_data[0]);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, x_dim, y_dim, z_dim, 0, GL_RED, GL_FLOAT, &(volume_data[0]));


    ourShader.use();
    ourShader.setInt("volume", 0);
    ourShader.setVec3("cameraPos", camera.Position);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, texture);
        // render the triangle
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        // std::cout<<"view matrix0:\n"<<view[0][0]<<" "<<view[0][1]<<" "<<view[0][2]<<" "<<view[0][3]<<std::endl;
        // std::cout<<"view matrix1:\n"<<view[1][0]<<" "<<view[1][1]<<" "<<view[1][2]<<" "<<view[1][3]<<std::endl;
        // std::cout<<"view matrix2:\n"<<view[2][0]<<" "<<view[2][1]<<" "<<view[2][2]<<" "<<view[2][3]<<std::endl;
        // std::cout<<"view matrix3:\n"<<view[3][0]<<" "<<view[3][1]<<" "<<view[3][2]<<" "<<view[3][3]<<std::endl;
        // break;

        // model transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // 更新帧率
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        updateFrameRate(window, deltaTime);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //     camera.ProcessKeyboard(FORWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //     camera.ProcessKeyboard(BACKWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //     camera.ProcessKeyboard(LEFT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //     camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pitch += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pitch -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        yaw += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        yaw -= 1.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void updateFrameRate(GLFWwindow* window, double deltaTime) {
    static int frameCount = 0;
    static double elapsedTime = 0.0;
    
    frameCount++;
    elapsedTime += deltaTime;

    // 每秒更新一次帧率
    if (elapsedTime >= 1.0) {
        // 更新窗口标题，显示当前帧率
        std::string title = "Volume Rendering - FPS: " + std::to_string(frameCount);
        glfwSetWindowTitle(window, title.c_str());
        
        frameCount = 0;
        elapsedTime = 0.0;
    }
}