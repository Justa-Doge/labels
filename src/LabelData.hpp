#pragma once

#include <Geode/Geode.hpp>
#include <optional>
#include <string>
#include <vector>

struct LabelEvent {
    bool enabled = false;
    int type = 0;
    std::string condition;
    float delay = 0.f;
    float duration = 0.f;
    float easing = 0.f;
    std::optional<bool> visible;
    std::optional<float> scale;
    std::optional<std::string> color;
    std::optional<float> opacity;
    std::optional<std::string> font;
};

template <>
struct matjson::Serialize<LabelEvent> {
    static matjson::Value toJson(LabelEvent const& event) {
        auto json = matjson::makeObject({
            {"enabled", event.enabled},
            {"type", event.type},
            {"condition", event.condition},
            {"delay", event.delay},
            {"duration", event.duration},
            {"easing", event.easing}
        });
        if (event.visible) json["visible"] = *event.visible;
        if (event.scale) json["scale"] = *event.scale;
        if (event.color) json["color"] = *event.color;
        if (event.opacity) json["opacity"] = *event.opacity;
        if (event.font) json["font"] = *event.font;
        return json;
    }

    static geode::Result<LabelEvent> fromJson(matjson::Value const& value) {
        LabelEvent event;
        GEODE_UNWRAP_INTO(event.enabled, value["enabled"].as<bool>());
        GEODE_UNWRAP_INTO(event.type, value["type"].as<int>());
        GEODE_UNWRAP_INTO(event.condition, value["condition"].as<std::string>());
        GEODE_UNWRAP_INTO(event.delay, value["delay"].as<float>());
        GEODE_UNWRAP_INTO(event.duration, value["duration"].as<float>());
        GEODE_UNWRAP_INTO(event.easing, value["easing"].as<float>());
        GEODE_UNWRAP_INTO(event.visible, value["visible"].as<std::optional<bool>>());
        GEODE_UNWRAP_INTO(event.scale, value["scale"].as<std::optional<float>>());
        GEODE_UNWRAP_INTO(event.color, value["color"].as<std::optional<std::string>>());
        GEODE_UNWRAP_INTO(event.opacity, value["opacity"].as<std::optional<float>>());
        GEODE_UNWRAP_INTO(event.font, value["font"].as<std::optional<std::string>>());
        return geode::Ok(std::move(event));
    }
};

struct LabelData {
    std::string name = "New label";
    std::string text = "{progress}%";
    std::string font = "bigFont.fnt";
    float scale = 0.35f;
    std::string color = "FFFFFFFF";
    bool visible = true;
    bool absolutePosition = false;
    float offsetX = 0.f;
    float offsetY = 0.f;
    int alignment = 0;
    int fontAlignment = 0;
    std::vector<LabelEvent> events;
};

template <>
struct matjson::Serialize<LabelData> {
    static matjson::Value toJson(LabelData const& label) {
        return matjson::makeObject({
            {"name", label.name},
            {"text", label.text},
            {"font", label.font},
            {"scale", label.scale},
            {"color", label.color},
            {"visible", label.visible},
            {"absolutePosition", label.absolutePosition},
            {"offset-x", label.offsetX},
            {"offset-y", label.offsetY},
            {"alignment", label.alignment},
            {"fontAlignment", label.fontAlignment},
            {"events", label.events}
        });
    }

    static geode::Result<LabelData> fromJson(matjson::Value const& value) {
        LabelData label;
        GEODE_UNWRAP_INTO(label.name, value["name"].as<std::string>());
        GEODE_UNWRAP_INTO(label.text, value["text"].as<std::string>());
        GEODE_UNWRAP_INTO(label.font, value["font"].as<std::string>());
        GEODE_UNWRAP_INTO(label.scale, value["scale"].as<float>());
        GEODE_UNWRAP_INTO(label.color, value["color"].as<std::string>());
        GEODE_UNWRAP_INTO(label.visible, value["visible"].as<bool>());
        GEODE_UNWRAP_INTO(label.absolutePosition, value["absolutePosition"].as<bool>());
        GEODE_UNWRAP_INTO(label.offsetX, value["offset-x"].as<float>());
        GEODE_UNWRAP_INTO(label.offsetY, value["offset-y"].as<float>());
        GEODE_UNWRAP_INTO(label.alignment, value["alignment"].as<int>());
        GEODE_UNWRAP_INTO(label.fontAlignment, value["fontAlignment"].as<int>());
        GEODE_UNWRAP_INTO(label.events, value["events"].as<std::vector<LabelEvent>>());
        return geode::Ok(std::move(label));
    }
};
