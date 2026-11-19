#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

using namespace geode::prelude;

namespace {
using Clock = std::chrono::steady_clock;
Clock::time_point s_levelStart;

std::string number(double value, int decimals = 2) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(decimals) << value;
    return out.str();
}

std::string resolveValue(std::string_view name, PlayLayer* layer, float dt) {
    if (name == "progress") {
        return number(layer->getCurrentPercent(), 2);
    }
    if (name == "fps") {
        return dt > 0.f ? number(1.0 / dt, 1) : "0";
    }
    if (name == "time") {
        auto seconds = std::chrono::duration<double>(Clock::now() - s_levelStart).count();
        return number(seconds, 1);
    }
    return {};
}

std::string formatLabel(std::string const& format, PlayLayer* layer, float dt) {
    std::string result;
    result.reserve(format.size() + 16);

    for (size_t i = 0; i < format.size();) {
        if (format[i] != '{') {
            result.push_back(format[i++]);
            continue;
        }

        auto close = format.find('}', i + 1);
        if (close == std::string::npos) {
            result.append(format.substr(i));
            break;
        }

        auto key = format.substr(i + 1, close - i - 1);
        auto first = key.find_first_not_of(" \t\r\n");
        auto last = key.find_last_not_of(" \t\r\n");
        if (first == std::string::npos) {
            result.append(format, i, close - i + 1);
        } else {
            key = key.substr(first, last - first + 1);
            auto value = resolveValue(key, layer, dt);
            result += value.empty() ? format.substr(i, close - i + 1) : value;
        }
        i = close + 1;
    }

    return result;
}
}

class $modify(LabelsPlayLayer, PlayLayer) {
    bool init(G等JGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        s_levelStart = Clock::now();

        if (Mod::get()->getSettingValue<bool>("overlay")) {
            auto label = CCLabelBMFont::create("", "bigFont.fnt");
            if (label) {
                label->setID("labels-overlay"_spr);
                label->setAnchorPoint({0.f, 1.f});
                label->setPosition({12.f, CCDirector::sharedDirector()->getWinSize().height - 12.f});
                label->setScale(Mod::get()->getSettingValue<double>("scale"));
                this->addChild(label, 10000);
            }
        }
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto label = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("labels-overlay"_spr));
        if (!label) {
            return;
        }

        auto format = Mod::get()->getSettingValue<std::string>("format");
        label->setString(formatLabel(format, this, dt).c_str());
        label->setScale(Mod::get()->getSettingValue<double>("scale"));
    }
};
