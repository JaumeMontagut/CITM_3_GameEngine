#ifndef __MODULE_RENDER_3D_H__
#define __MODULE_RENDER_3D_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "RenderTexture.h"

#define MAX_LIGHTS 8

struct SDL_Window;
struct ImVec2;

class ModuleRenderer3D : public Module
{
private:
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;
	bool texture_2d = false;
public:
	ModuleRenderer3D(const char* name, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(JSONFile * module_file);
	void UpdateProjectionMatrix();
	bool Start(JSONFile * module_file) override;
	update_status PreUpdate() override;
	update_status PostUpdate() override;
	bool CleanUp();

	bool SaveConfiguration(JSONFile * module_file);
	bool LoadConfiguration(JSONFile * module_file);

	void OnResize(int width, int height);
	
	void DrawConfigurationUi();
public:
	Light lights[MAX_LIGHTS];
	RenderTexture scene_fbo;
	RenderTexture game_fbo;

	friend class PanelConfiguration;
};

#endif