#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <imgui-cocos.hpp>
#include "ModuleManager.hpp"

using namespace geode::prelude;
using namespace keybinds;

// ImGui Cocos is a library that initializes Dear ImGui for Cocos2d-x.
// Special thanks to Prevter for giving me the example code to use ImGui with Cocos2d-x.

static ImFont* openSans = nullptr;

$execute {
    BindManager::get()->registerBindable({
        "view-managers"_spr,
        "View Libraries",
        "Toggles the library viewer window",
        { Keybind::create(KEY_Y, Modifier::Alt) },
        "Library Viewer"
    });

    new EventListener([=](InvokeBindEvent* event) {
        if (event->isDown()) {
            auto& imgui = ImGuiCocos::get();
            imgui.toggle();
            if (imgui.isVisible()) ModuleManager::get()->init();
        }
        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "view-managers"_spr));

    ImGuiCocos::get()
        .setup([]{
            openSans = ImGui::GetIO().Fonts->AddFontFromFileTTF((Mod::get()->getResourcesDir() / "opensans.ttf").string().c_str(), 20.0f);
        })
        .draw([]{
            if (ImGui::Begin("Library Viewer", nullptr, ImGuiWindowFlags_HorizontalScrollbar)) {
                ImGui::PushFont(openSans);

                auto moduleManager = ModuleManager::get();
                auto& modules = moduleManager->getModules();
                for (int i = 0; i < modules.size(); i++) {
                    auto& module = modules[i];
                    auto ptr = module.address != 0 ? fmt::format("0x{:x}", module.address) : "[NULL]";
                    ImGui::Text("%s: %s", module.name.c_str(), ptr.c_str());
                    ImGui::SameLine();
                    if (ImGui::Button(fmt::format("Copy Address##{}", i).c_str())) clipboard::write(ptr);
                    ImGui::SameLine();
                    if (ImGui::Button(fmt::format("Copy Name##{}", i).c_str())) clipboard::write(module.fullName);
                }

                if (ImGui::Button("Close")) ImGuiCocos::get().setVisible(false);

                ImGui::PopFont();
            }
            ImGui::End();
        })
        .setVisible(false);
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(LVMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto bottomMenu = getChildByID("bottom-menu");
        auto libraryViewerButton = CCMenuItemSpriteExtra::create(CircleButtonSprite::createWithSprite("LV_viewerBtn_001.png"_spr, 1.0f,
            CircleBaseColor::Green, CircleBaseSize::MediumAlt), this, menu_selector(LVMenuLayer::onLibraryViewer));
        libraryViewerButton->setID("library-viewer-button"_spr);
        bottomMenu->addChild(libraryViewerButton);
        bottomMenu->updateLayout();

        return true;
    }

    void onLibraryViewer(CCObject*) {
        auto& imgui = ImGuiCocos::get();
        imgui.toggle();
        if (imgui.isVisible()) ModuleManager::get()->init();
    }
};
