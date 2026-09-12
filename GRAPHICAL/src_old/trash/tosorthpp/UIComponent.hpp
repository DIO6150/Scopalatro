#pragma once

#include <modules/Sprite.hpp>

namespace TLOT
{

	class UIComponent
	{
	public:
		virtual std::map<std::string, std::vector<std::map<std::string, std::vector<std::string>>>> const RequestTextures () const = 0;
		
		~UIComponent ()
		{
			for (auto & [key, sprite] : m_sprites)
			{
				sprite.Kill ();
			}

			for (auto & child : m_children)
			{
				child->m_parent = nullptr;
			}
		}

		void SetParent (UIComponent & parent)
		{
			m_parent = &parent;
			m_parent->AddChild (*this);
		}

		void AddChild (UIComponent & child)
		{
			m_children.emplace (&child);
			child.SetParent (*this);
		}

		glm::vec3 GetEffectivePosition ()
		{
			return GetPosition () + glm::vec3 {pivot, 0.0};
		}

		glm::vec3 GetPosition ()
		{
			if (m_parent)
			{
				return position + m_parent->GetPosition ();
			}

			return position;
		}

		void SetPosition (glm::vec3 newPosition)
		{
			position = newPosition;
		}

		void SetAnchor (glm::vec2 anchor)
		{
			pivot = anchor;
		}

	protected:

		auto const & Children () const { return m_children; }
		auto Parent () { return m_parent; }

		glm::vec3 position; // x, y, z-index
		glm::vec2 pivot; // x, y

	private:
		friend class UIComponentManager;
		
		std::map<std::string, Sprite> m_sprites;

		std::set<UIComponent *> m_children;
		UIComponent * m_parent = nullptr;
	};
}
