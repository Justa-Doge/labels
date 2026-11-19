#pragma once

#include "LabelData.hpp"

#include <vector>

std::vector<LabelData>& getLabels();
void saveLabels();
void refreshLabelsHud();
std::vector<LabelData> makeDefaultLabels();
