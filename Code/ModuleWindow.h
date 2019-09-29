#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "parson/parson.h"

#define MY_BUFSIZE 1024 // Buffer size for console window titles.

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	void LoadConfig();
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	void CleanUpConfig();

	void SaveConfig();

	void SetTitle(const char* title);

private:
	Uint32 GetFlags();
	void DecideGLAndGLSLVersions();
	bool SetWindow();

public:
	int GetWindowWidth();
	int GetWindowHeight();
	bool IsVsync();

	//The window we'll be rendering to
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;
	const char* glsl_version = nullptr;


	//TODO: Make them private
	//This values will be overriden by config.json
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;
	bool vsync = false;

private:
	JSON_Value * config = nullptr;
	JSON_Object * config_obj = nullptr;
};

#endif // __ModuleWindow_H__