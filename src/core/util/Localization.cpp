#include "Localization.hpp"

#include "LocalizationData.hpp"

#include <algorithm>
#include <mutex>
#include <string>
#include <unordered_map>

namespace YimMenu::Localization
{
	namespace
	{
		constexpr bool Less(const Entry& e, std::string_view key)
		{
			return e.en < key;
		}

		const Entry* Find(std::string_view text)
		{
			auto it = std::lower_bound(std::begin(kEntries), std::end(kEntries), text, Less);
			if (it != std::end(kEntries) && it->en == text)
				return &*it;
			return nullptr;
		}

		// Cache for Label(): every entry is small and bounded by the dictionary size,
		// so it never grows without limit. Needed because ImGui keeps the pointer.
		std::unordered_map<std::string, std::string>& Cache()
		{
			static std::unordered_map<std::string, std::string> cache;
			return cache;
		}
	}

	const char* Tr(std::string_view text)
	{
		if (text.empty())
			return text.data();

		// ImGui internal identifiers ("##suffix") must keep their original ASCII form;
		// translating them would change widget identity.
		if (text.find("##") != std::string_view::npos)
			return text.data();

		if (auto entry = Find(text))
			return entry->zh.data();

		return text.data();
	}

	std::string TrStr(std::string_view text)
	{
		if (auto entry = Find(text))
			return std::string(entry->zh);

		return std::string(text);
	}

	const char* Label(std::string_view english)
	{
		if (english.find("##") != std::string_view::npos)
			return english.data();

		auto entry = Find(english);
		if (!entry)
			return english.data();

		auto& cache = Cache();
		auto key = std::string(english);
		auto it = cache.find(key);
		if (it != cache.end())
			return it->second.c_str();

		std::string value;
		value.reserve(entry->zh.size() + english.size() + 3);
		value.append(entry->zh);
		value.append("##");
		value.append(english);
		return cache.emplace(std::move(key), std::move(value)).first->second.c_str();
	}

	std::size_t EntryCount()
	{
		return kEntries.size();
	}
}
