#include <modules/UIComponent.hpp>
#include <modules/UIComponentManager.hpp>

namespace TLOT
{
	class IButtonCallback
	{
	public:
		virtual void OnResize () = 0;
		virtual void OnHover () = 0;
		virtual void OnPress () = 0;
	};

	template<std::derived_from<IButtonCallback> ButtonCallback>
	class IButton : public UIComponent
	{
	public:
		virtual std::map<std::string, std::vector<std::map<std::string, std::vector<std::string>>> const RequestTextures () const = 0;

		IButton (glm::vec3 defaultColor, glm::vec3 hoverColor, glm::vec3 pressedColor):
			m_defaultColor {defaultColor}, m_hoverColor {hoverColor}, m_pressedColor {pressedColor}
		{

		}

		~IButton ()
		{

		}

		void OnResize ()
		{
			m_callback.OnResize ();
		}

		void OnHover ()
		{
			m_callback.OnHover ();
		}

		void OnPressed ()
		{
			m_callback.OnPress ();
		}

	private:
		glm::vec3 m_defaultColor = glm::vec3 {1.0};
		glm::vec3 m_hoverColor   = glm::vec3 {1.0};
		glm::vec3 m_pressedColor = glm::vec3 {1.0};

		ButtonCallback m_callback;
	};
}
