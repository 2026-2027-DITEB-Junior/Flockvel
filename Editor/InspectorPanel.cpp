#include "InspectorPanel.h"
#include "imgui.h"

void TransformPanel::Draw(clc::Transform& transform, bool& dirty)
{
	ImGui::TextUnformatted("Transform");
	if (ImGui::DragFloat3("Position", &transform.mPosition.x, 0.1f))
		dirty = true;
	if (ImGui::DragFloat3("Rotation", &transform.mRotation.x, 0.1f))
		dirty = true;
	if (ImGui::DragFloat3("Scale", &transform.mScale.x, 0.1f))
		dirty = true;
}

void InspectorPanel::Draw(clc::GameObject* selected, bool& visible, bool& dirty)
{
	if (ImGui::Begin("Inspector", &visible))
	{
		if (selected == nullptr)
		{
			ImGui::TextUnformatted("Select an object in the Hierarchy.");
		}
		else
		{
			char name[256] = {};
			strncpy_s(name, selected->mName.c_str(), _TRUNCATE);
			if (ImGui::InputText("Name", name, sizeof(name)))
			{
				selected->mName = name;
				dirty = true;
			}

			bool enabled = selected->IsEnabled();
			if (ImGui::Checkbox("Enabled", &enabled))
			{
				selected->SetEnabled(enabled);
				dirty = true;
			}

			ImGui::Separator();
			m_transformPanel.Draw(selected->mTransform, dirty);
			ImGui::Separator();

			const std::vector<clc::IComp*> components = selected->GetCompVec<clc::IComp>();
			for (clc::IComp* component : components)
			{
				if (component == nullptr || component->IsShutdown())
					continue;

				ImGui::PushID(component);
				std::string componentName = component->mName.empty() ? component->GetType() : component->mName;
				ImGui::TextUnformatted(componentName.c_str());
				component->Draw();
				ImGui::Separator();
				ImGui::PopID();
			}
		}
	}
	ImGui::End();
}
