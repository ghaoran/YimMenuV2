#include "Items.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/widgets/toggle/imgui_toggle.hpp"

namespace YimMenu
{
	ListCommandItem::ListCommandItem(joaat_t id, std::optional<std::string> label_override) :
	    m_Command(Commands::GetCommand<ListCommand>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void ListCommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text( LBL("Unknown list!"));
			return;
		}

		int current_val = m_Command->GetState();
		auto& list = m_Command->GetList();
		const char* largest_string = "";
		std::size_t largest_string_len = 0;

		if (!m_SelectedItem.has_value() || !m_ItemWidth.has_value())
		{
			for (auto& item : list)
			{
				if (item.first == current_val)
				{
					m_SelectedItem = item.second;
				}

				int length = strlen(item.second);
				if (length > largest_string_len)
				{
					largest_string = item.second;
					largest_string_len = length;
				}
			}

			if (!m_SelectedItem.has_value())
				m_SelectedItem = "";

			auto size = ImGui::CalcTextSize(Localization::Tr(largest_string));
			m_ItemWidth = size.x + 40.0f;
		}

		ImGui::SetNextItemWidth(m_ItemWidth.value());
		if (ImGui::BeginCombo(m_LabelOverride.value_or(m_Command->GetLabel()).c_str(), Localization::Tr(m_SelectedItem.value()).data()))
		{
			for (auto& el : list)
			{
				if (ImGui::Selectable((std::string(Localization::Tr(el.second)) + "##opt_" + el.second).c_str(), el.first == current_val))
				{
					current_val = el.first;
					m_Command->SetState(el.first);
				}

				if (el.first == current_val)
				{
					m_SelectedItem = el.second; // just in case
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}