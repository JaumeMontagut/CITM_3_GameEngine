#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"

class Shortcut;
class ComponentTransform;
class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(const char * name, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start(JSONFile * config) override;
	update_status Update(float dt) override;
	update_status PostUpdate() override;
	void RotateCamera(float dt);
	bool CleanUp() override;

	//void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	//void FocusToObject(ComponentTransform& transform);
	void Move(const float3 &Movement);
	float* GetViewMatrix();

	void DrawConfigurationUi() override;
	bool SaveConfiguration(JSONFile* module_file) override;
	bool LoadConfiguration(JSONFile* module_file) override;
	float3 GetPos();
	ComponentCamera* GetCurrentCamera();

public:
	float3 reference;
	bool see_frustrum_culling = false;
private:
	float camera_move_speed = 150.f;
	float camera_rotate_speed = 20.f;

	Shortcut * navigate_forward = nullptr;
	Shortcut * navigate_backward = nullptr;
	Shortcut * navigate_left = nullptr;
	Shortcut * navigate_right = nullptr;
	Shortcut * navigate_up = nullptr;
	Shortcut * navigate_down = nullptr;
	Shortcut * navigate_fast = nullptr;
	Shortcut * focus_object = nullptr;

	ComponentCamera * scene_camera = nullptr;
	ComponentCamera * current_camera = nullptr;

	LineSegment picking;
	int camera_combo;

	SDL_Cursor* cursor = nullptr;
	
	friend class ModuleRenderer3D;
	friend class ModuleScene;
};