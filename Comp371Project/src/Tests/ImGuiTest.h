#pragma once
#include "../Dependencies/imgui-1.76/imgui.h"
#include "../Core/Script.h"

class ImGuiTest : public Script
{
public:
	void OnImGuiRender()
	{
		ImGui::Begin("Test ImGui Window");
		ImGui::Text("Text works");

		// Buttons return true when clicked (most widgets return true when edited/activated)
		if (ImGui::Button("More"))
			more = !more;

		if (more)
		{
			ImGui::Text("You pressed the button");
		}

		ImGui::End();
	}

private:
	bool more = false;
};