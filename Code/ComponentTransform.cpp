#include "ComponentTransform.h"
#include "MathGeoLib/include/Math/float4.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "Globals.h"

#include "glew/include/GL/glew.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleGui.h"
#include <string>
CLASS_DEFINITION(Component, ComponentTransform)

ComponentTransform::ComponentTransform(GameObject *gameobject) : Component(gameobject)
{
	name = "Transform"; //TODO: Every transform component (and other components) don't need its own name, they can all have the same

	//TODO: This is for testing purposes, remove when done
	global_matrix = local_matrix = local_matrix.identity;
	SetTransform(float3(0,0,0), float3(1,1,1), float3(0,0,0));
	UpdateVector();
}

ComponentTransform::~ComponentTransform()
{
	CleanUp();
}

void ComponentTransform::SetParent(ComponentTransform *parent)
{
	if (parent != nullptr)
	{
		this->parent = parent;
		parent->children.push_back(this);
		RecalculateMatrices();
	}
	//TODO: This should update the position, rotation and scale so that it remains intact in world space but it displays relative to the parent
}

ComponentTransform * ComponentTransform::GetParent()
{
	return parent;
}

ComponentTransform * ComponentTransform::Find(const char * name)
{
	ComponentTransform* ret = nullptr;
	for (std::vector<ComponentTransform*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		if ((*iter)->gameobject->name == name)
		{
			return (*iter);
		}
		ret = (*iter)->Find(name);
		if (ret != nullptr)
		{
			return ret;
		}
	}
	return ret;
}

void ComponentTransform::OnPostUpdate()
{
	//DrawAxis();
	bounding_box.Draw();
}

void ComponentTransform::PropertiesEditor()
{
	ImGui::SameLine();
	if (ImGui::Checkbox("Static", &is_static))
	{
		SwitchedStatic();
	}
	
	bool position_changed = false,
		rotation_changed = false,
		scale_changed = false;

	if (ImGui::Button("Reset"))
	{
		Reset();
	}
	if (ImGui::InputFloat3("Position", (float *)&position, "%.2f"))
	{
		position_changed = true;
	}
	if (ImGui::InputFloat3("Rotation", (float *)&euler_rotation, "%.2f"))
	{
		rotation_changed = true;
	}
	if (ImGui::InputFloat3("Scale", (float *)&scale, "%.2f"))
	{
		scale_changed = true;
	}
	if (position_changed || rotation_changed || scale_changed)
	{
		SetTransform(position, scale, euler_rotation);
	}
}

void ComponentTransform::OnSave(JSONFile * file)
{
	JSONFile tranform_file = file->AddSection("Transform");
	tranform_file.SaveFloatArray("Position", (const float*)&position, 3);
	tranform_file.SaveFloatArray("Rotation", (const float*)&euler_rotation, 3);
	tranform_file.SaveFloatArray("Scale", (const float*)&scale, 3);
}

void ComponentTransform::OnLoad(JSONFile * file)
{
	
	float3 aux_pos= float3::zero;
	file->LoadFloatArray("Position", (float*)&aux_pos);
	float3 aux_rot = float3::zero;
	file->LoadFloatArray("Rotation", (float*)&aux_rot);
	float3 aux_scale = float3::zero;
	file->LoadFloatArray("Scale", (float*)&aux_scale);
	this->SetTransform(aux_pos, aux_scale, aux_rot);
}


void ComponentTransform::SwitchedStatic()
{
	if (is_static)
	{
		App->scene->octree.Insert(this);
	}
	else
	{
		App->scene->RecreateOctree();
	}
}

void ComponentTransform::CleanUp()
{
	LOG("COMPONENT TRANSFORM CLEANUP");
	if (App->gui->GetSelectedTransform() == this)
	{
		App->gui->SetSelectedGameObjec(nullptr);
	}
	bounding_box.CleanUp();
}

void ComponentTransform::SetTransform(float3 &position, float3 &scale, float3 &euler_rotation)
{
	this->position = position;
	this->euler_rotation = euler_rotation;
	this->scale = scale;

	qrotation = Quat::FromEulerXYZ(euler_rotation.x * DEGTORAD, euler_rotation.y * DEGTORAD, euler_rotation.z * DEGTORAD);

	RecalculateMatrices();
}

void ComponentTransform::SetTransform(float3 &position, float3 &scale, Quat &qrotation)
{
	this->position = position;
	this->qrotation = qrotation;
	this->euler_rotation = qrotation.ToEulerXYZ() * RADTODEG;
	this->scale = scale;

	RecalculateMatrices();
}

void ComponentTransform::SetTransform(const float4x4 &local_matrix)
{
	this->local_matrix = local_matrix;

	RecalculateMatrices();
}

void ComponentTransform::RecalculateMatrices()
{
	this->local_matrix = float4x4::FromTRS(position, qrotation, scale);

	if (parent != nullptr)
	{
		global_matrix = parent->global_matrix * local_matrix;
	}
	else
	{
		global_matrix = local_matrix;
	}

	bounding_box.MultiplyByMatrix(global_matrix);

	UpdateVector();

	for (auto i = gameobject->components.begin(); i != gameobject->components.end(); ++i)
	{
		(*i)->TransformHaveChanged();
	}
	UpdateChildrenMatrices();
	
}

void ComponentTransform::UpdateChildrenMatrices()
{
	for (std::vector<ComponentTransform *>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		(*iter)->global_matrix = global_matrix * (*iter)->local_matrix;
		(*iter)->UpdateVector();
		(*iter)->bounding_box.MultiplyByMatrix((*iter)->global_matrix);
		(*iter)->UpdateChildrenMatrices();
	}
}

void ComponentTransform::UpdateVector()
{
	float3x3 matrix = global_matrix.Float3x3Part();
	x = { 1,0,0 };
	x =  matrix * x;
	y = { 0,1,0 };
	y = matrix * y;
	z = { 0,0,1 };
	z = matrix * z;
}

void ComponentTransform::SetPosition(const float3 &position)
{
	this->position = position;

	RecalculateMatrices();
}

void ComponentTransform::SetRotation(const float3 &euler_rotation)
{
	this->euler_rotation = euler_rotation;
	qrotation = Quat::FromEulerXYZ(euler_rotation.x * DEGTORAD, euler_rotation.y * DEGTORAD, euler_rotation.z * DEGTORAD);

	RecalculateMatrices();
}

void ComponentTransform::SetRotation(const Quat &qrotation)
{
	this->qrotation = qrotation;
	euler_rotation = qrotation.ToEulerXYZ() * RADTODEG;

	RecalculateMatrices();
}

void ComponentTransform::SetScale(const float3 &scale)
{
	this->scale = scale;

	RecalculateMatrices();
}

void ComponentTransform::SetGlobalMatrix(const float4x4& matrix)
{
	float3 position, scale;
	Quat rotation;
	matrix.Decompose(position, rotation, scale);
	this->SetTransform(position, scale, rotation);

}

std::vector<ComponentTransform*> ComponentTransform::GetChildren()
{
	return 	children;
}

void ComponentTransform::SetLocalMatrix(const float4x4 & matrix)
{
	if (parent != nullptr)
	{
		math::float4x4 new_matrix = parent->global_matrix.Inverted();
		new_matrix = new_matrix * matrix;

		float3 aux_position, aux_scale;
		Quat aux_rotation;

		new_matrix.Decompose(aux_position, aux_rotation, aux_scale);

		SetTransform(position, scale, aux_rotation);
	}
}

void ComponentTransform::SetSelected(bool state)
{
	is_selected = state;
	for (std::vector<ComponentTransform*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		(*iter)->SetSelected(state);
	}
}

bool ComponentTransform::IsSelected()
{
	return is_selected;
}

bool ComponentTransform::IsInChilds(ComponentTransform * object)
{
	for (std::vector<ComponentTransform*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		if (object == (*iter))
		{
			return true;
		}
	}
	return false;
}

bool ComponentTransform::Intersect(LineSegment ray)
{
	AABB box = bounding_box.GetAABB();
	if (box.IsFinite())
	{
		return ray.Intersects(box);
	}
	else
	{
		return false;
	}
}

float3 ComponentTransform::GetZAxis()
{
	return z;
}
float3 ComponentTransform::GetYAxis()
{
	return y;
}


float3 ComponentTransform::GetPosition() const
{
	return position;
}

float3 ComponentTransform::GetGlobalPosition() const
{
	float3 global_position;
	Quat global_rotation;
	float3 global_scale;
	global_matrix.Decompose(global_position, global_rotation, global_scale);
	return global_position;
}

Quat ComponentTransform::GetRotation() const
{
	return qrotation;
}

float3 ComponentTransform::GetRotationEuler() const
{
	return euler_rotation;
}

float3 ComponentTransform::GetScale() const
{
	return scale;
}

float4x4 ComponentTransform::GetGlobalMatrix() const
{
	return global_matrix;
}

float4x4 ComponentTransform::GetLocalMatrix() const
{
	return local_matrix;
}

void ComponentTransform::Reset()
{
	SetTransform(float3(0, 0, 0), float3(1, 1, 1), float3(0, 0, 0));
}

void ComponentTransform::UpdateDisplayValues()
{
	global_matrix.Decompose(position, qrotation, scale);
	euler_rotation = qrotation.ToEulerXYZ();
}

void ComponentTransform::DeleteChildren()
{
	if (!children.empty())
	{
		for (std::vector<ComponentTransform *>::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			(*iter)->DeleteChildren();
			delete (*iter)->gameobject;
		}
		children.clear();
	}
}

void ComponentTransform::DeleteFromChildren(ComponentTransform * object)
{
	if (gameobject->transform->children.size() > 0)
	{
		for (std::vector<ComponentTransform *>::iterator iter = gameobject->transform->children.begin(); iter != gameobject->transform->children.end(); ++iter)
		{
			if ((*iter) == object)
			{
				(*iter)->parent = nullptr;
				children.erase(iter);
				return;
			}
		}
	}
}

void ComponentTransform::AddChild(ComponentTransform * new_object)
{
	if (new_object->parent != nullptr)
	{
		new_object->parent->DeleteFromChildren(new_object);
	}
	children.push_back(new_object);
	new_object->parent = this;
	new_object->SetGlobalMatrix(this->global_matrix.Inverted() * new_object->global_matrix);
}

void ComponentTransform::DrawAxis()
{
	float length = 2;
	glLineWidth(5);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);  // Blue
	glVertex3f(position.x, position.y, position.z);
	glVertex3f(position.x + z.x * length, position.y + z.y * length, position.z + z.z * length);

	glColor3f(1.0f, 0.0f, 0.0f);  // Red
	glVertex3f(position.x, position.y, position.z);
	glVertex3f(position.x + x.x * length, position.y + x.y * length, position.z + x.z * length);

	glColor3f(0.0f, 1.0f, 0.0f);  // Green
	glVertex3f(position.x, position.y, position.z);
	glVertex3f(position.x + y.x * length, position.y + y.y * length, position.z + y.z * length);

	glEnd();
	
}

AABB ComponentTransform::GetAABB()
{
	return bounding_box.GetAABB();
}

void ComponentTransform::GetStaticObjects(std::vector<ComponentTransform*>& static_objects)
{
	if (is_static)
	{
		static_objects.push_back(this);
	}

	if (children.size())
	{
		for (std::vector<ComponentTransform*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		{
			(*iter)->GetStaticObjects(static_objects);
		}
	}
}