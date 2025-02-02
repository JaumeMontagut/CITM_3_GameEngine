#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include <string>
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "JSONFile.h"
#include "Globals.h"

#define INVALID_GAMEOBJECT_UID 0

class Component;
class PanelProperties;
class PanelHierarchy;

class GameObject
{
public:


	GameObject(std::string name, ComponentTransform * parent, UID uid = INVALID_GAMEOBJECT_UID);
	~GameObject();
	bool OnStart();
	bool OnUpdate(float dt);
	bool OnPostUpdate();
	void SetActive(bool value);
	void OnSave(JSONFile*);
	void OnLoad(JSONFile*);
	UID GetUID();

	template <class ComponentClass>
	ComponentClass * CreateComponent();


	template< class ComponentClass >
	ComponentClass* GetComponent()
	{
		for (std::vector<Component*>::iterator iter = components.begin(); iter != components.end(); ++iter)
		{
			if ((*iter) && (*iter)->IsClassType(ComponentClass::type))
			{
				return (ComponentClass*)(*iter);

			}
		}
		return nullptr;
	}
	template< class ComponentType >
	bool RemoveComponent()
	{
		if (components.empty())
		{
			return false;
		}

		for (std::vector<Component*>::iterator iter = components.begin(); iter != components.end(); ++iter)
		{
			uint type_ = ComponentType::type;
			if ((*iter) && (*iter)->IsClassType(type_))
			{
				(*iter)->CleanUp();
				delete (*iter);
				components.erase(iter);
				return true;
			}
		}
	
		return false;
	}

	const char * GetName();

private:
	bool ActivateComponents();
	bool DeactivateComponents();

public:
	ComponentTransform* transform;

private:
	bool active = true;
	std::string name;
	std::vector<Component*> components;
	UID uid = INVALID_GAMEOBJECT_UID;
	friend class ComponentTransform;
	friend PanelProperties;
	friend PanelHierarchy;
	friend class ModuleScene;
	friend class ComponentMesh;
};
template<class ComponentClass>
inline ComponentClass * GameObject::CreateComponent()
{
	ComponentClass * new_component = new ComponentClass(this);
	components.push_back(new_component);
	return new_component;
}
#endif // !GAMEOBJECT_H_

