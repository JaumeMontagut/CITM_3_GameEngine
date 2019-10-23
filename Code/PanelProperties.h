#ifndef __PANEL_PROPERTIES_H__
#define __PANEL_PROPERTIES_H__

#include "Panel.h"
#include <string>
class ComponentTransform;

class PanelProperties : public Panel
{
private:
	ComponentTransform* selected_transform = nullptr;
public:
	PanelProperties(std::string name, bool state, std::vector<SDL_Scancode> shortcuts = {});
	void SetSelectedTransform(ComponentTransform* gameobject);
	void Draw() override;

	friend class PanelHierarchy;
	friend class ModuleGui;
};



#endif // !PANEL_PROPERTIES_H_

