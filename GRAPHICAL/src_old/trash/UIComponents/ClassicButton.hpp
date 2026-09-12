#include <modules/UIComponents/IButton.hpp>
#include <modules/UIComponentManager.hpp>

class ClassicButtonCallback : public TLOT::IButtonCallback
{
	inline void OnHover () override
	{

	}

	inline void OnPress () override
	{

	}

	inline void OnResize () override
	{

	}
};

class ClassicButton : public TLOT::IButton<ClassicButtonCallback>
{
public:
	ClassicButton (glm::vec2 position, float zIndex, glm::vec3 color):
		IButton {color, color, color}
	{
		this->position = glm::vec3 {position, zIndex};
	}
	
	inline std::map<std::string, std::vector<std::map<std::string, std::vector<std::string>>>> const RequestTextures () const override
	{
		return {
			{"topLeft",     { { "default", {"ui_button_corner_top_left" } } }},
			{"topRight",    { { "default", {"ui_button_corner_top_right" } } }},
			{"bottomLeft",  { { "default", {"ui_button_corner_bottom_left" } } }},
			{"bottomRight", { { "default", {"ui_button_corner_bottom_right" } } }},
			{"center",      { { "default", {"ui_button_center"}}}
		};
	}
};