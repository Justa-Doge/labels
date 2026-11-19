#include <Geode/Geode.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("Labels loaded. Open a level pause menu to edit the HUD labels.");
}
