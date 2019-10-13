#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include <vector>

class GameObject;

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject * gameobject);
	//Create 

	void SetParent(ComponentTransform * parent);
	//void SetChildren(std::vector<Transform*> children);

public:
	float3 position;
	float3 rotation;

	//You should modify the local matrix, the global matrix is recalculated from it and the parents' local matrix
	float4x4 local_matrix;

	float4x4 global_matrix;

private:
	ComponentTransform * parent = nullptr;
	std::vector<ComponentTransform*> children;

	friend class PanelHierarchy;
	friend class ModuleScene;
};
#endif // !TRANSFORM_H_

