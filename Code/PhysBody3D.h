#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include <list>
#include "Bullet/include/LinearMath/btQuaternion.h"

class btRigidBody;
class Module;
class vec3;
// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);

protected:
	btRigidBody * body = nullptr;

public:
	std::list<Module*> collision_listeners;
	vec3 GetPos();
	btQuaternion GetRotation();
};

#endif // __PhysBody3D_H__