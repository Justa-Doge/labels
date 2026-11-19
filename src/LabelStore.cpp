#include "LabelStore.hpp"

using namespace geode::prelude;

std::vector<LabelData> makeDefaultLabels() {
    // Imported labels belong to the user. Start clean; the editor exposes
    // Percentage and Noclip as optional Eclipse-compatible presets.
    return {};
}

std::vector<LabelData>& getLabels() {
    static auto stored = Mod::get()->getSavedValue<std::vector<LabelData>>(
        "labels",
        makeDefaultLabels()
    );
    return stored;
}

void saveLabels() {
    Mod::get()->setSavedValue("labels", getLabels());
    refreshLabelsHud();
}
