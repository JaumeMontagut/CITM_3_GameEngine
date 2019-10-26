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

	ModuleWindow(const char* name ,bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(JSON_Object* config) override;

	bool CleanUp();

	void SetTitle(const char* title);

	bool Save(JSON_Object * config);

	int GetWindowWidth();
	int GetWindowHeight();
	
	void SetWidth(float& width);
	void SetHeight(float& height);
	void SetWindowSize(float width, float height);

	bool IsVsync();
	
	void SetBrightness(float brightness);
	
	void GetMaxWindowSize(float& width, float& height);


	void DrawConfigurationUi();
	
	bool SaveConfiguration(JSON_Object * module_obj) override;
	bool LoadConfiguration(JSON_Object * module_obj) override;

private:
	Uint32 GetFlags();
	void DecideGLAndGLSLVersions();
	bool SetWindow();
	
	

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;
	const char* glsl_version = nullptr;


	//TODO: Make them private
	//This values will be overriden by config_root.json
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;
	bool vsync = false;

	float brightness = 100.f;
	float max_width, max_height, min_width = 640, min_height = 480;
};

#endif // __ModuleWindow_H__