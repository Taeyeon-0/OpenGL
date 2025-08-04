// 在包含GLFW的头文件之前包含GLAD的头文件
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        return -1;
    }
    // glfwWindowHint用于设置创建窗口时的各种提示参数
    // 设置OpenGL上下文版本， 主版本和次版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // GLFW_OPENGL_CORE_PROFILE(核心模式) GLFW_OPENGL_COMPAT_PROFILE(兼容模式-立即模式)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 用于Mac OS X系统
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello_Window", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        // 结束GLFW，释放资源
        glfwTerminate();
        return -1;
    }
    // 设置 window 窗口上下文成为当前线程的主上下文
    glfwMakeContextCurrent(window);

    // 使用GLAD加载OpenGL函数指针
    // gladLoadGLLoader函数的参数是一个函数指针，
    // glfwGetProcAddress是GLFW提供的获取OpenGL函数指针的方法
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置视口 - OpenGL渲染窗口的尺寸大小
    glViewport(0, 0, 800, 600);

    // 注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 渲染循环
    // glfwWindowShouldClose(window) 函数检查窗口是否应该关闭，如果窗口被请求关闭（例如，用户点击了关闭按钮），该函数会返回 true。
    while (!glfwWindowShouldClose(window)) {
        // 输入检测ESC键
        processInput(window);

        // 渲染指令
        // 生成随机颜色
        const float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);// 从0到1的随机数
        const float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);// 从0到1的随机数
        const float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);// 从0到1的随机数

        // 设置清除颜色
        // glClearColor定义了在清除颜色缓冲区时所使用的颜色
        // glClear执行实际的清楚
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换前后缓冲区，OpenGL采用双缓冲机制，保证用户看到图像是完整的，避免渲染过程中的闪烁和撕裂
        // 前缓冲区是当前正在显示在屏幕上的缓冲区
        // 后缓冲区是用于存储即将显示的图像的缓冲区。所有的绘制命令和渲染操作都是在后缓冲区完成的。
        // OpenGL内部确保在交换之前所有的绘制命令都被完成
        glfwSwapBuffers(window);
        // 处理用户事件，输入事件，比如键盘按下、鼠标移动、窗口大小变化等
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}