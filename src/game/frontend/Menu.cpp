#include "Menu.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "core/renderer/Renderer.hpp"
#include "game/frontend/fonts/Fonts.hpp"
#include "submenus/Self.hpp"
#include "submenus/Vehicle.hpp"
#include "submenus/Teleport.hpp"
#include "submenus/Network.hpp"
#include "submenus/Players.hpp"
#include "submenus/Recovery.hpp"
#include "submenus/Settings.hpp"
#include "submenus/Debug.hpp"
#include "submenus/World.hpp"
#include "Onboarding.hpp"

namespace YimMenu
{
	void Menu::Init()
	{
		// Arguably the only place this file should be edited at for more menus
		UIManager::AddSubmenu(std::make_shared<Submenus::Self>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Vehicle>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Teleport>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Network>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Players>());
		UIManager::AddSubmenu(std::make_shared<Submenus::World>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Recovery>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Settings>());
		UIManager::AddSubmenu(std::make_shared<Submenus::Debug>());

		Renderer::AddRendererCallback(
		    [&] {
			    ProcessOnboarding();
			    if (!GUI::IsOpen())
				    return;

			    ImGui::PushFont(Menu::Font::g_DefaultFont);
			    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImU32(ImColor(15, 15, 15)));

			    UIManager::Draw();

			    ImGui::PopStyleColor();
			    ImGui::PopFont();
		    },
		    -1);
	}

	static const ImWchar* GetGlyphRangesCyrillicOnly()
	{
		static const ImWchar ranges[] =
		    {
		        0x0400,
		        0x052F, // Cyrillic + Cyrillic Supplement
		        0x2DE0,
		        0x2DFF, // Cyrillic Extended-A
		        0xA640,
		        0xA69F, // Cyrillic Extended-B
		        0,
		    };
		return &ranges[0];
	}

	// Simplified Chinese UI: cover CJK Unified Ideographs, CJK punctuation and
	// fullwidth forms together with the Japanese ranges used previously.
	static const ImWchar* GetGlyphRangesChineseUi()
	{
		static const ImWchar ranges[] =
		    {
		        0x0020,
		        0x00FF, // Basic Latin + Latin Supplement
		        0x2000,
		        0x206F, // General Punctuation
		        0x3000,
		        0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		        0x31F0,
		        0x31FF, // Katakana Phonetic Extensions
		        0x4E00,
		        0x9FFF, // CJK Unified Ideographs
		        0xFF00,
		        0xFFEF, // Half-width characters
		        0,
		    };
		return &ranges[0];
	}

	static ImFont* CreateFontWithCyrillicSupport(ImGuiIO& io, float size)
	{
		ImFontConfig FontCfg{};
		FontCfg.FontDataOwnedByAtlas = false;

		auto font = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::MainFont), sizeof(Fonts::MainFont), size, &FontCfg, io.Fonts->GetGlyphRangesDefault());

		// just use Arial for Cyrillic

		FontCfg.MergeMode = true;
		io.Fonts->AddFontFromFileTTF((std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts" / "arial.ttf").string().c_str(), size, &FontCfg, GetGlyphRangesCyrillicOnly());

		// Chinese glyphs: msyh.ttc (Microsoft YaHei) ships with Windows 7+, with
		// simhei/simsun as fallbacks. meiryo.ttc is kept last for the Japanese ranges.
		const auto fontDir = std::filesystem::path(std::getenv("SYSTEMROOT")) / "Fonts";
		bool cjkLoaded = false;
		for (auto& candidate : {"msyh.ttc", "msyh.ttf", "simhei.ttf", "simsun.ttc", "meiryo.ttc"})
		{
			auto fullPath = fontDir / candidate;
			if (!std::filesystem::exists(fullPath))
				continue;

			if (io.Fonts->AddFontFromFileTTF(fullPath.string().c_str(), size, &FontCfg, GetGlyphRangesChineseUi()) != nullptr)
			{
				cjkLoaded = true;
				break;
			}
		}

		if (!cjkLoaded)
			io.Fonts->AddFontFromFileTTF((fontDir / "meiryo.ttc").string().c_str(), size, &FontCfg, io.Fonts->GetGlyphRangesJapanese());

		io.Fonts->Build();

		return font;
	}

	void Menu::SetupFonts()
	{
		auto& IO = ImGui::GetIO();
		auto file_path = std::filesystem::path(std::getenv("appdata")) / "YimMenuV2" / "imgui.ini";
		static auto path = file_path.string();
		IO.IniFilename = path.c_str();
		IO.LogFilename = NULL;
		ImFontConfig FontCfg{};
		FontCfg.FontDataOwnedByAtlas = false;

		IO.Fonts->Clear();
		Menu::Font::g_DefaultFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_DefaultFontSize);
		Menu::Font::g_OptionsFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_OptionsFontSize);
		Menu::Font::g_ChildTitleFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_ChildTitleFontSize);
		Menu::Font::g_ChatFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_ChatFontSize);
		Menu::Font::g_OverlayFont = CreateFontWithCyrillicSupport(IO, Menu::Font::g_OverlayFontSize);
		static const ImWchar full_range[] = {0x0020, 0xFFFF, 0};
		Menu::Font::g_AwesomeFont = IO.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Fonts::IconFont), sizeof(Fonts::IconFont), Menu::Font::g_AwesomeFontSize, &FontCfg, full_range);

		UIManager::SetOptionsFont(Menu::Font::g_OptionsFont);
		Renderer::SetFontsUpdated();
	}
}
