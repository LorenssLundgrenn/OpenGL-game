#include <iostream>
#include <SDL.h>
#include <glad/glad.h>
#include <plog/Log.h>
#include <string.h>
#include <vector>

#include "GameState.h"

GLuint GameState::compile_shader(GLuint type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    return shader;
}

GameState::GameState(int win_width, int win_height) {
    this->win_width = win_width;
    this->win_height = win_height;
    int init_status = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_status > 0) {
        PLOGD << "falied to initialize SDL2: " << SDL_GetError();
        exit(1);
    }

    // specify OpenGL version (4.6)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // init OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // create window
    this->window = SDL_CreateWindow("OpenGL Window",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    if (window == NULL) {
        PLOGD << "falied to create SDL2 window: " << SDL_GetError();
        exit(1);
    }

    // create OpenGL context
    this->gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        PLOGD << "failed to create OpenGL context";
    }

    // init Glad
    int gald_init =  gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (!gald_init) {
        PLOGD << "failed to initialize glad";
        exit(1);
    }

    // vertex specification
    const std::vector<GLfloat> vertex_position {
        -0.8f, -0.8f, -0.0f,
        0.8f, -0.8f, 0.8f,
        0.0f, 0.8f, 0.0f
    };

    // generate vertex array object
    glGenVertexArrays(1, &this->vertex_array);
    glBindVertexArray(this->vertex_array);

    // generate vertex buffer object
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        vertex_position.size() * sizeof(GLfloat),
        vertex_position.data(),
        GL_STATIC_DRAW
    );

    // configure OpenGL context
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

    // create OpenGL graphics pipeline
    GLuint gl_program = glCreateProgram();
    GLuint vertex_shader = this->compile_shader(GL_VERTEX_SHADER, this->vertex_source);
    GLuint fragment_shader = this->compile_shader(GL_FRAGMENT_SHADER, this->fragment_source);

    // attach shaders to graphics pipeline
    glAttachShader(gl_program, vertex_shader);
    glAttachShader(gl_program, fragment_shader);
    glLinkProgram(gl_program);
    glValidateProgram(gl_program);
    this->graphics_pipeline = gl_program;
}

GameState::~GameState() {
    SDL_Quit();
    SDL_DestroyWindow(window);
}

void GameState::run() {
    while (this->running) {
        this->get_inputs();
        this->setup_draw();
        this->draw();
        SDL_GL_SwapWindow(this->window);

        /*
        //DEBUG
        std::cout << "OpenGL Info:\n";
        std::cout << "version: " << glGetString(GL_VERSION) << "\n";
        std::cout << "vendor: " << glGetString(GL_VENDOR) << "\n";
        std::cout << "renderer: " << glGetString(GL_RENDERER) << "\n";
        std::cout << "shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
        this->running = false;
        //DEBUG END
        */
    }
}

void GameState::get_inputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            PLOGD << "program terminating by user";
            this->running = false;
        }
    }
}

void GameState::setup_draw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, this->win_width, this->win_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(this->graphics_pipeline);
}

void GameState::draw() {
    glBindVertexArray(this->vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}