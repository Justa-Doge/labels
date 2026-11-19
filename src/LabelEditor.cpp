#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include "LabelStore.hpp"

using namespace geode::prelude;

namespace {
class LabelEditorPopup : public geode::Popup {
protected:
    TextInput* m_nameInput = nullptr;
    TextInput* m_textInput = nullptr;
    CCLabelBMFont* m_counter = nullptr;
    CCLabelBMFont* m_status = nullptr;
    CCMenu* m_navigationMenu = nullptr;
    CCMenu* m_actionsMenu = nullptr;
    size_t m_selected = 0;
    bool m_isFilling = false;

    bool init() {
        if (!Popup::init({380.f, 330.f}, "GJ_square01.png")) return false;
        setTitle("Labels");

        m_counter = CCLabelBMFont::create("", "bigFont.fnt");
        m_counter->setPosition({190.f, 264.f});
        m_counter->setScale(0.45f);
        m_mainLayer->addChild(m_counter);

        m_nameInput = TextInput::create(320.f, "Label name");
        m_nameInput->setPosition({190.f, 222.f});
        m_nameInput->setMaxCharCount(48);
        m_nameInput->setCallback([this](std::string const& value) {
            if (m_isFilling || getLabels().empty()) return;
            getLabels()[m_selected].name = value;
            saveLabels();
        });
        m_mainLayer->addChild(m_nameInput);

        m_textInput = TextInput::createMultiline({320.f, 82.f}, "Label text with {expressions}");
        m_textInput->setPosition({190.f, 150.f});
        m_textInput->setMaxCharCount(2048);
        m_textInput->setCallback([this](std::string const& value) {
            if (m_isFilling || getLabels().empty()) return;
            getLabels()[m_selected].text = value;
            saveLabels();
        });
        m_mainLayer->addChild(m_textInput);

        m_navigationMenu = CCMenu::create();
        m_navigationMenu->setPosition({190.f, 101.f});
        m_mainLayer->addChild(m_navigationMenu);
        addButton(m_navigationMenu, "Prev", {-72.f, 0.f}, menu_selector(LabelEditorPopup::onPrev));
        addButton(m_navigationMenu, "Next", {72.f, 0.f}, menu_selector(LabelEditorPopup::onNext));

        m_actionsMenu = CCMenu::create();
        m_actionsMenu->setPosition({190.f, 68.f});
        m_mainLayer->addChild(m_actionsMenu);
        addButton(m_actionsMenu, "Add", {-120.f, 0.f}, menu_selector(LabelEditorPopup::onAdd));
        addButton(m_actionsMenu, "Delete", {-40.f, 0.f}, menu_selector(LabelEditorPopup::onDelete));
        addButton(m_actionsMenu, "Show/Hide", {40.f, 0.f}, menu_selector(LabelEditorPopup::onToggle));
        addButton(m_actionsMenu, "Import .ecl", {120.f, 0.f}, menu_selector(LabelEditorPopup::onImport));

m_status = CCLabelBMFont::create("Import your labels when you want them", "chatFont.fnt");
        m_status->setPosition({190.f, 17.f});
        m_status->setScale(0.42f);
        m_mainLayer->addChild(m_status);

        auto presetsMenu = CCMenu::create();
        presetsMenu->setPosition({190.f, 37.f});
        m_mainLayer->addChild(presetsMenu);
        addButton(presetsMenu, "Import Percentage", {-82.f, 0.f}, menu_selector(LabelEditorPopup::onImportPercentage));
        addButton(presetsMenu, "Import Noclip", {82.f, 0.f}, menu_selector(LabelEditorPopup::onImportNoclip));

        fillFields();
        return true;
    }

    void addButton(CCMenu* menu, char const* text, CCPoint position, SEL_MenuHandler selector) {
        auto item = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(text, "goldFont.fnt", "GJ_button_01.png", 0.62f),
            this,
            selector
        );
        item->setPosition(position);
        menu->addChild(item);
    }

    void fillFields() {
        m_isFilling = true;
        auto& items = getLabels();
        if (items.empty()) {
            m_selected = 0;
            m_counter->setString("No labels yet");
            m_nameInput->setString("");
            m_textInput->setString("");
        } else {
            m_selected %= items.size();
            auto const& selected = items[m_selected];
            m_counter->setString(fmt::format(
                "{} / {}  |  {}{}",
                m_selected + 1,
                items.size(),
                selected.name,
                selected.visible ? "" : " (hidden)"
            ).c_str());
            m_nameInput->setString(selected.name);
            m_textInput->setString(selected.text);
        }
        m_isFilling = false;
    }

    void onPrev(CCObject*) {
        auto& items = getLabels();
        if (items.empty()) return;
        m_selected = (m_selected + items.size() - 1) % items.size();
        fillFields();
    }

    void onNext(CCObject*) {
        auto& items = getLabels();
        if (items.empty()) return;
        m_selected = (m_selected + 1) % items.size();
        fillFields();
    }

    void onAdd(CCObject*) {
        LabelData label;
        label.name = "New label";
        label.text = "{progress}%";
        getLabels().push_back(std::move(label));
        m_selected = getLabels().size() - 1;
        saveLabels();
        fillFields();
    }

    void onDelete(CCObject*) {
        auto& items = getLabels();
        if (items.empty()) return;
        items.erase(items.begin() + static_cast<std::ptrdiff_t>(m_selected));
        if (!items.empty()) m_selected %= items.size();
        saveLabels();
        fillFields();
    }

    void onToggle(CCObject*) {
        auto& items = getLabels();
        if (items.empty()) return;
        items[m_selected].visible = !items[m_selected].visible;
        saveLabels();
        fillFields();
    }

    void importPreset(char const* fileName) {
        auto path = Mod::get()->getResourcesDir() / "presets" / fileName;
        auto imported = geode::utils::file::readFromJson<LabelData>(path);
        if (!imported) {
            FLAlertLayer::create("Preset unavailable", "The bundled Eclipse label preset could not be loaded.", "OK")->show();
            return;
        }
        getLabels().push_back(std::move(imported).unwrap());
        m_selected = getLabels().size() - 1;
        saveLabels();
        fillFields();
    }

    void onImportPercentage(CCObject*) { importPreset("Percentage.ecl"); }
    void onImportNoclip(CCObject*) { importPreset("Noclip.ecl"); }

    void onImport(CCObject*) {
        geode::utils::file::FilePickOptions::Filter filter;
        filter.description = "Eclipse Label (*.ecl)";
        filter.files.insert("*.ecl");

        geode::Ref<LabelEditorPopup> self = this;
        geode::async::spawn(
            geode::utils::file::pick(
                geode::utils::file::PickMode::OpenFile,
                {Mod::get()->getSaveDir(), {std::move(filter)}}
            ),
            [self](geode::utils::file::PickResult result) mutable {
                if (!result) return;
                auto picked = std::move(result).unwrapOr(std::nullopt);
                if (!picked) return;

                geode::queueInMainThread([self, path = *picked] {
                    auto imported = geode::utils::file::readFromJson<LabelData>(path);
                    if (!imported) {
                        FLAlertLayer::create(
                            "Import failed",
                            "That file could not be read as an Eclipse label (.ecl).",
                            "OK"
                        )->show();
                        return;
                    }

                    getLabels().push_back(std::move(imported).unwrap());
                    saveLabels();
                    self->m_selected = getLabels().size() - 1;
                    self->fillFields();
                    self->m_status->setString(
                        "Imported; Eclipse-only variables need Eclipse to supply their values"
                    );
                });
            }
        );
    }

public:
    static LabelEditorPopup* create() {
        auto* popup = new LabelEditorPopup();
        if (popup->init()) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }
};

void openLabelEditor(CCObject*) {
    if (auto* popup = LabelEditorPopup::create()) popup->show();
}
}

class $modify(LabelsPauseLayer, PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto item = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Labels", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(LabelsPauseLayer::onOpenLabels)
        );

        auto menu = CCMenu::create(item, nullptr);
        menu->setPosition({winSize.width / 2.f, winSize.height / 2.f - 102.f});
        menu->setID("labels-pause-menu"_spr);
        addChild(menu, 1000);
    }

    void onOpenLabels(CCObject* sender) {
        openLabelEditor(sender);
    }
};

