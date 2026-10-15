#include <Geode/modify/LevelSearchLayer.hpp>
#include "../Layers/RouletteLayer/RouletteLayer.hpp"

using namespace geode::prelude;

class $modify(LevelSearchLayer) {
    bool init(int type) {
        if (!LevelSearchLayer::init(type)) return false;
        
        auto rightFilterMenu = getChildByID("other-filter-menu");
        if (!rightFilterMenu) return false;

        auto spr = CCSprite::create("logo-spr.png"_spr);
        auto btnSpr = CircleButtonSprite::create(
            spr, CircleBaseColor::Green, CircleBaseSize::Small
        );

        auto btn = CCMenuItemExt::createSpriteExtra(
            btnSpr, [this](auto) {
                CCDirector::get()->pushScene(
                    CCTransitionFade::create(0.5f, RouletteLayer::scene())
                );
            }
        );

        rightFilterMenu->addChild(btn);
        rightFilterMenu->updateLayout();

        return true;
    }
};