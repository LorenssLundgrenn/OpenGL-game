#ifndef GAME_STATE
#define GAME_STATE

#include <SDL.h>
#include <glad/glad.h>
#include <string.h>

class GameState {
private:
	int win_width{ 0 };
	int win_height{ 0 };
	bool running = true;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
	std::string vertex_source = 
		"#version 410 core\n"
		"in vec4 position;\n"
		"void main() {\n"
		"	gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
		"}\n";
	std::string fragment_source = 
		"#version 410 core\n"
		"out vec4 color;\n"
		"void main() {\n"
		"	color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
		"}\n";
	GLuint vertex_array{ 0 };
	GLuint vertex_buffer{ 0 };
	GLuint graphics_pipeline{ 0 };

	GLuint compile_shader(GLuint type, const std::string &source);

public:
	GameState(int win_width, int win_height);
	~GameState();

	void run();
	void get_inputs();
	void setup_draw();
	void draw();
};

#endif