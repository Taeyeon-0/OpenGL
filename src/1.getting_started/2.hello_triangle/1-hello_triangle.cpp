// 绘制三角形
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

// 顶点着色器 (Vertex Shader)
// 版本声明：使用 OpenGL 3.3 核心版本的 GLSL 语言
const char* vertexShaderSource =
    "#version 330 core\n"
    // 输入变量 aPos，表示顶点位置，绑定到顶点属性位置0
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    // 将顶点位置转换为齐次坐标，并赋值给内置变量 gl_Position，
    // 这是顶点着色器的最终输出，用于后续渲染管线
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// 片元着色器 (Fragment Shader)
const char* fragmentShaderSource =
    "#version 330 core\n"
    // 输出变量 FragColor，表示当前片元颜色
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    // 给当前片元赋固定颜色，橙色调 (RGBA: 1.0, 0.5, 0.2, 1.0)
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    // 启用通过着色器程序来控制点的大小（Point Size）的功能。
    // OpenGL 会查找在顶点着色器中输出的内置变量 gl_PointSize。
    // 顶点着色器可以计算并写入一个浮点值到 gl_PointSize。
    // 这个 gl_PointSize 的值将用于设置当前正在绘制的这个点的大小（以像素为单位）。
    glEnable(GL_PROGRAM_POINT_SIZE);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 1. 将顶点数据储存在显卡的内存中，用VBO这个顶点缓冲对象管理
    // VBO(Vertex Buffer Object， 顶点缓冲对象), 是GPU显存中的一块缓冲区，用来存放顶点数据
    // VBO 运行一次性的将大量顶点数据上传到显存，减少CPU到GPU的传输，提高渲染性能
    // 1.1 生成缓冲区，glGenBuffers创建一个或多个VBO的ID
    unsigned int VBO;
    // n表示生成的缓冲区对象的数量
    glGenBuffers(1, &VBO);
    // 1.2 将VBO绑定到GL_ARRAY_BUFFER缓冲区，后续顶点缓冲区相关的操作会作用于这个缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 1.4 向显卡上传数据并将其存储在指定的缓冲对象中
    // 第四个参数指定了我们希望显卡如何管理给定的数据
    // GL_STATIC_DRAW ：数据不会或几乎不会改变。
    // GL_DYNAMIC_DRAW：数据会被改变很多。
    // GL_STREAM_DRAW ：数据每次绘制时都会改变。
    // 三角形的位置数据不会改变，每次渲染调用时都保持原样，所以它的使用类型最好是GL_STATIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 2. 顶点着色器 
    // 顶点着色器是GPU上运行的一段小程序，它是图形渲染管线中第一个可编程的阶段，负责处理每个顶点的数据。
    // 顶点着色器就是用来对输入的每个顶点做逐顶点处理的程序
    // 2.1 创建顶点着色器
    // GL_VERTEX_SHADER表示为顶点着色器
    unsigned int vertexShader  = glCreateShader(GL_VERTEX_SHADER);
    // 2.2 将着色器源码附加到着色器对象上，然后编译
    // 编译的着色器对象作为第一个参数。第二参数指定了传递的源码字符串数量，这里只有一个。第三个参数是顶点着色器真正的源码
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    // 2.3 编译着色器
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    // 查询顶点着色器的编译状态，将结果存储在 success 变量中
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        // 获取编译错误信息，并将其存储在 infoLog 数组中
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
    }

    // 3. 片段着色器
    // 片段着色器负责计算最终像素的颜色
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
    }

    // 4. 着色器程序 - 负责把多个着色器“链接”成一个整体，供 GPU 渲染管线调用
    // 4.1 创建着色器程序并附加顶点和片段着色器
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // 4.2 链接程序
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    }

    // 4.3 使用着色器
    // 从调用这句开始，OpenGL 的渲染管线会使用这个程序处理所有的顶点和片段着色器。
    // 可以创建多个着色器程序，如果想切换渲染效果，只需要调用不同的 glUseProgram 激活不同的程序即可
    glUseProgram(shaderProgram);

    // 4.4 删除着色器 (已链接进程序)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 5. VAO顶点数组对象
    // VAO 是 OpenGL 用来存储顶点属性配置状态的对象，记录了顶点数据的格式和绑定的VBO状态
    // 有了 VAO 就可以一次性设置顶点属性格式和绑定缓冲区，以后绘制时只需绑定 VAO 即可恢复所有顶点状态。
    // 当有多个VBO，就可以使用多个VAO保存状态，绘制时只需切换对应的 VAO 即可
    // 5.1 创建绑定顶点数组对象
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 5.2 配置顶点属性指针
    // 第一个参数 0 表示我们要配置的是位置属性，位置属性的 location 在顶点着色器中定义为 layout(location = 0)
    // 第二个参数 3 表示每个顶点有 3 个分量，分别是 x, y, z
    // 第三个参数 GL_FLOAT 表示数据类型为浮点数
    // 第四个参数 GL_FALSE 表示不希望数据被标准化(设为0) 标准化是标准化到0到1的范围，只对整数类型有效
    // 第五个参数 3 * sizeof(float) 表示相邻顶点之间的步幅（stride），即每个顶点的数据大小
    // 第六个参数 static_cast<void*>(nullptr) 表示从缓冲区的起始位置开始读取数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

    // 启用顶点属性，启用在上面配置的属性
    // 这里启用的位置属性（location = 0），使得顶点着色器能够接收到位置数据
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 绘制三角形
        // 图元类型为 GL_TRIANGLES
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}