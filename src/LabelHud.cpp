#include <Geode/Geode.hpp>
#include <Geode/modify/UILayer.hpp>

#include <rift.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "LabelStore.hpp"

using namespace geode::prelude;

namespace {
std::function<void()> s_rebuildHud;
struct RuntimeLabel {
    LabelData data;
    CCLabelBMFont* node = nullptr;
    std::unique_ptr<rift::Script> script;
    std::string compileError;
    int stackIndex = 0;
};

std::string formatLevelTime(double time) {
    int totalSeconds = std::max(0, static_cast<int>(time));
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int millis = std::max(0, static_cast<int>(time * 1000.0) % 1000);

    std::ostringstream out;
    if (minutes > 0) {
        out << minutes << ':' << std::setw(2) << std::setfill('0') << seconds;
    } else {
        out << seconds;
    }
    out << '.' << std::setw(3) << std::setfill('0') << millis;
    return out.str();
}

std::string localClock() {
    std::time_t now = std::time(nullptr);
    std::tm local{};
    localtime_s(&local, &now);
    std::ostringstream out;
    out << std::put_time(&local, "%I:%M:%S %p");
    return out.str();
}

ccColor3B parseColor(std::string const& color) {
    if (color.size() < 6) {
        return ccWHITE;
    }
    auto component = [&color](size_t offset) -> GLubyte {
        try {
            return static_cast<GLubyte>(std::stoul(color.substr(offset, 2), nullptr, 16));
        } catch (...) {
            return 255;
        }
    };
    return {component(0), component(2), component(4)};
}

GLubyte parseOpacity(std::string const& color) {
    if (color.size() < 8) return 255;
    try {
        return static_cast<GLubyte>(std::stoul(color.substr(6, 2), nullptr, 16));
    } catch (...) {
        return 255;
    }
}

std::unordered_map<std::string, rift::Value> makeVariables(
    GJBaseGameLayer* game,
    float fps,
    float bestRun
) {
    std::unordered_map<std::string, rift::Value> vars;
    auto put = [&vars](std::string const& key, auto value) {
        vars.emplace(key, rift::Value::from(value));
    };

    put("fps", fps);
    put("realFps", fps);
    put("clock", localClock());
    put("attempt", game ? game->m_attempts : 0);
    put("isTestMode", game ? game->m_isTestMode : false);
    put("isPracticeMode", game ? game->m_isPracticeMode : false);
    put("isPlatformer", game ? game->m_isPlatformer : false);
    put("isDead", game && game->m_player1 ? game->m_player1->m_isDead : false);
    put("isDualMode", game && game->m_player2 != nullptr);
    put("bestRun", bestRun);
    // These defaults keep Eclipse label packs readable when the separate
    // Eclipse gameplay settings that provide these values are not installed.
    put("noclip", false);
    put("noclipDeaths", 0);
    put("noclipAccuracy", 100.0);

    if (!game || !game->m_level || !game->m_player1) {
        put("progress", 0.0);
        put("realProgress", 0);
        put("time", std::string("0.000"));
        put("best", 0);
        put("levelName", std::string{});
        return vars;
    }

    double progress = 0.0;
    if (game->m_level->m_timestamp > 0) {
        progress = game->m_gameState.m_levelTime * 240.0 /
            static_cast<double>(game->m_level->m_timestamp) * 100.0;
    } else if (game->m_levelLength > 0.f) {
        progress = game->m_player1->getPositionX() /
            static_cast<double>(game->m_levelLength) * 100.0;
    }
    progress = std::clamp(progress, 0.0, 100.0);

    put("progress", progress);
    auto* playLayer = typeinfo_cast<PlayLayer*>(game);
    put("realProgress", playLayer ? playLayer->getCurrentPercent() : static_cast<int>(progress));
    put("time", formatLevelTime(game->m_gameState.m_levelTime));
    put("levelTime", game->m_gameState.m_levelTime);
    put("levelLength", game->m_levelLength);
    put("playerX", game->m_player1->getPositionX());
    put("playerY", game->m_player1->getPositionY());
    put("levelName", std::string(game->m_level->m_levelName));

    if (game->m_isPlatformer) {
        put("best", formatLevelTime(game->m_level->m_bestTime));
    } else {
        put("best", game->m_level->m_normalPercent.value());
    }
    return vars;
}

void applyLabelPosition(RuntimeLabel& label, CCSize const& winSize, int stackCount) {
    int alignment = std::clamp(label.data.alignment, 0, 8);
    int column = alignment % 3;
    int row = alignment / 3;
    float xFactor = column == 0 ? 0.f : column == 1 ? 0.5f : 1.f;
    float yFactor = row == 0 ? 1.f : row == 1 ? 0.5f : 0.f;
    CCPoint anchor = {xFactor, yFactor};

    label.node->setAnchorPoint(anchor);
    if (label.data.fontAlignment >= 0 && label.data.fontAlignment <= 2) {
        label.node->setAlignment(static_cast<CCTextAlignment>(label.data.fontAlignment));
    }

    float line = std::max(18.f, label.node->getContentSize().height * label.data.scale + 4.f);
    float x = winSize.width * xFactor + label.data.offsetX;
    float y = winSize.height * yFactor + label.data.offsetY;

    if (row == 0) y -= 12.f + line * label.stackIndex;
    else if (row == 2) y += 12.f + line * label.stackIndex;
    else y += (static_cast<float>(stackCount - 1) / 2.f - label.stackIndex) * line;

    if (column == 0) x += 12.f;
    else if (column == 2) x -= 12.f;

    label.node->setPosition(label.data.absolutePosition
        ? CCPoint{label.data.offsetX, label.data.offsetY}
        : CCPoint{x, y});
}
}

class $modify(LabelsUILayer, UILayer) {
    struct Fields {
        GJBaseGameLayer* game = nullptr;
        std::vector<RuntimeLabel> labels;
        float fpsElapsed = 0.f;
        int fpsFrames = 0;
        float fpsAverage = 0.f;
        float bestRun = 0.f;
    };

    bool init(GJBaseGameLayer* game) {
        if (!UILayer::init(game)) return false;

        m_fields->game = game;
        m_fields->bestRun = 0.f;
        rebuildLabels();
        schedule(schedule_selector(LabelsUILayer::updateLabels));

        return true;
    }

    void onExit() {
        s_rebuildHud = {};
        UILayer::onExit();
    }

    void rebuildLabels() {
        auto* root = getChildByID("labels-root"_spr);
        if (root) {
            root->removeFromParent();
        }

        auto newRoot = CCNode::create();
        newRoot->setID("labels-root"_spr);
        addChild(newRoot, 1000);

        m_fields->labels.clear();
        auto& stored = getLabels();

        for (auto const& data : stored) {
            RuntimeLabel runtime;
            runtime.data = data;
            runtime.node = CCLabelBMFont::create("", data.font.c_str());
            if (!runtime.node) {
                log::warn("Could not create label '{}' using font '{}'", data.name, data.font);
                continue;
            }

            runtime.node->setScale(std::clamp(data.scale, 0.1f, 4.f));
            runtime.node->setColor(parseColor(data.color));
            runtime.node->setOpacity(parseOpacity(data.color));
            runtime.node->setVisible(data.visible);
            newRoot->addChild(runtime.node);

            auto compiled = rift::compile(data.text);
            if (compiled.isError()) {
                runtime.compileError = compiled.getMessage();
            } else {
                runtime.script.reset(compiled.unwrap());
            }
            m_fields->labels.push_back(std::move(runtime));
        }

        std::array<int, 9> counts{};
        for (auto& label : m_fields->labels) {
            int alignment = std::clamp(label.data.alignment, 0, 8);
            label.stackIndex = counts[alignment]++;
        }
        for (auto& label : m_fields->labels) {
            applyLabelPosition(label, CCDirector::sharedDirector()->getWinSize(), counts[
                std::clamp(label.data.alignment, 0, 8)
            ]);
        }

        s_rebuildHud = [this] { rebuildLabels(); };
    }

    void updateLabels(float dt) {
        auto* fields = m_fields.self();
        fields->fpsElapsed += dt;
        ++fields->fpsFrames;

        if (dt > 0.f && fields->game && fields->game->m_player1) {
            fields->bestRun = std::max(fields->bestRun, static_cast<float>(
                std::clamp(
                    fields->game->m_level->m_timestamp > 0
                        ? fields->game->m_gameState.m_levelTime * 240.0 /
                            fields->game->m_level->m_timestamp * 100.0
                        : fields->game->m_levelLength > 0.f
                            ? fields->game->m_player1->getPositionX() /
                                fields->game->m_levelLength * 100.0
                            : 0.0,
                    0.0,
                    100.0
                )
            ));
        }

        if (fields->fpsElapsed < 0.1f) return;

        fields->fpsAverage = fields->fpsElapsed > 0.f
            ? static_cast<float>(fields->fpsFrames) / fields->fpsElapsed
            : 0.f;
        fields->fpsElapsed = 0.f;
        fields->fpsFrames = 0;

        auto vars = makeVariables(fields->game, fields->fpsAverage, fields->bestRun);
        for (auto& label : fields->labels) {
            if (!label.node || !label.data.visible) continue;
            if (label.script) {
                label.node->setString(label.script->run(vars).c_str());
            } else {
                label.node->setString(("RIFT: " + label.compileError).c_str());
            }
        }

        std::array<int, 9> counts{};
        for (auto& label : fields->labels) {
            label.stackIndex = counts[std::clamp(label.data.alignment, 0, 8)]++;
        }
        for (auto& label : fields->labels) {
            applyLabelPosition(label, CCDirector::sharedDirector()->getWinSize(), counts[
                std::clamp(label.data.alignment, 0, 8)
            ]);
        }
    }
};

void refreshLabelsHud() {
    if (s_rebuildHud) s_rebuildHud();
}

