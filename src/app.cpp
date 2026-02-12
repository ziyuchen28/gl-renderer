 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

struct shader_program_sources
{
    std::string vertex_source;
    std::string frag_soruce;
};

static shader_program_sources parse_shader(const std::string &file_path)
{
    std::ifstream stream(file_path);

    enum class shader_type 
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    shader_type type = shader_type::NONE;

    std::array<std::string, 2> src;
    const size_t sz = std::filesystem::file_size(file_path); 
    // /2 just best effort estimate, 128 - arbitrary cushion, 
    // string still need to dynamically grow if needed
    for (auto &s : src) s.reserve(sz / 2 + 128); 

    auto put = [&](shader_type t, const char *p, size_t n) {
        src[(int)t].append(p, n);
        src[(int)t].push_back('\n');
    };

    std::string line;
    while (std::getline(stream, line)) {
        static int shader_prog_start = 0;
        if (line.find("#shader") != std::string::npos) {
            shader_prog_start = 1;
            if (line.find("vertex") != std::string::npos) {
                type = shader_type::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos){
                type = shader_type::FRAGMENT;
            }
        }
        else if (shader_prog_start){
            put(type, line.data(), line.size());
        }
    }
    return {std::move(src[0]), std::move(src[1])};

} 

static unsigned int compile_shader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = &source[0];
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        // w/a for c++ not supporting VLA
        char *message = (char*)alloca(len * sizeof(char));
        glGetShaderInfoLog(id, len, &len, message);
        std::cout << "Failed to compile shader" << 
            (type == GL_VERTEX_SHADER ? "vertex" : "frag") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }


    return id;
}


static unsigned int create_shader(const std::string &vertex_shader, 
                                  const std::string &frag_shader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, frag_shader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}


int main(void)
{

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "glew init err" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;


    // vertex buffer
    float posisitons[6] = {
        -0.5f, -0.5f,
        0.0f,   0.5f,
        0.5f,  -0.5f,
    };
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), posisitons, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    shader_program_sources source = parse_shader("res/shaders/basic.shader");

    unsigned int shader = create_shader(source.vertex_source, source.frag_soruce);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
