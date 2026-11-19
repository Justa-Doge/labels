#pragma once

#include <Geode/Geode.hpp>
#include <string>

struct LabelData {
    std::string name = "New label";
    std::string text = "{progress}%";
    std::string font = "bigFont.fnt";
    bool visible = true;
    float scale = 0.35f;
    int alignment = 0;
    bool absolutePosition = false;
    float offsetX = 0.f;
    float offsetY = 0.f;
};

template <>
struct matjson::Serialize<LabelData> {
    static matjson::Value toJson(LabelData const& label) {
        return matjson::makeObject({
            {"name", label.name},
            {"text", label.text},
            {"font", label.font},
            {"visible", label.visible},
            {"scale", label.scale},
            {"alignment", label.alignment},
            {"absolutePosition", label.absolutePosition},
            {"offsetX", label.offsetX},
            {"offsetY", label.offsetY}
        });
    }

    static geode::Result<LabelData> fromJson(matjson::Value const& value) {
        LabelData label;
        GEODE_UNWRAP_INTO(label.name, value["name"].as<std::string>());
        GEODE_UNWRAP_INTO(label.text, value["text"].as<std::string>());
        GEODE_UNWRAP_INTO(label.font, value["font"].as<std::string>());
        GEODE_UNWRAP_INTO(label.visible, value["visible"].as<bool>());
        GEODE_UNWRAP_INTO(label.scale, value["scale"].as<float>());
        GEODE_UNWRAP_INTO(label.alignment, value["alignment"].as<int>());
        GEODE_UNWRAP_INTO(label.absolutePosition, value["absolutePosition"].as<bool>());
        GEODE_UNWRAP_INTO(label.offsetX, value["offsetX"].as<float>());
        GEODE_UNWRAP_INTO(label.offsetY, value["offsetY"].as<float>());
        return geode::Ok(std::move(label));
    }
};
