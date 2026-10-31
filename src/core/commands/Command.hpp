#pragma once
#include "core/util/Joaat.hpp"
#include "core/util/Localization.hpp"

#include <nlohmann/json.hpp>


namespace YimMenu
{
	class Command
	{
	private:
		int m_NumArgs = 0; // TODO: currently unused

	protected:
		virtual void OnCall() = 0;
		void MarkDirty();
		std::string m_Name;
		std::string m_Label;
		std::string m_Description;
		std::string m_LocalizedLabel;
		std::string m_LocalizedDescription;
		joaat_t m_Hash;

	public:
		Command(std::string name, std::string label, std::string description, int num_args = 0);
		virtual ~Command() = default;
		void Call();

		// Lua-created commands override this so runtime commands don't leave entries behind in the config file.
		virtual bool ShouldSaveState() const
		{
			return true;
		}

		virtual void SaveState(nlohmann::json& value) {}
		virtual void LoadState(nlohmann::json& value) {}

		const std::string& GetName()
		{
			return m_Name;
		}

		// Raw English label. Used for configuration keys and logging only.
		const std::string& GetRawLabel()
		{
			return m_Label;
		}

		// Localized (Simplified Chinese) label, shown in the UI.
		const std::string& GetLabel()
		{
			return m_LocalizedLabel;
		}

		// Localized (Simplified Chinese) description, shown in tooltips.
		const std::string& GetDescription()
		{
			return m_LocalizedDescription;
		}

		joaat_t GetHash()
		{
			return m_Hash;
		}
	};
}
