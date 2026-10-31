#pragma once
#include <string>
#include <string_view>

namespace YimMenu::Localization
{
	// Returns the Simplified Chinese text for `text` when a translation exists,
	// otherwise the original text unchanged. Never used for internal command names.
	const char* Tr(std::string_view text);

	// Convenience wrapper for std::string members (Submenu/Category names).
	std::string TrStr(std::string_view text);

	// Returns "中文##English" so ImGui keeps a unique, stable widget id while
	// displaying only the Chinese part. Result is cached; pointer stays valid.
	const char* Label(std::string_view english);

	// Number of loaded entries.
	std::size_t EntryCount();
}

// Wraps a label literal for ImGui calls that use the string as a widget id.
#define LBL(s) ::YimMenu::Localization::Label(s)
