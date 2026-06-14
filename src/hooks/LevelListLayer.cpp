#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include "../popups/NewListPopup/newListPopup.hpp"

using namespace geode::prelude;

class $modify(MyLevelListLayer, LevelListLayer) {
    bool init(GJLevelList* list) {
        if (!LevelListLayer::init(list)) return false;
        
        auto rightSideMenu = getChildByID("right-side-menu");
        if (!rightSideMenu) return false;

        auto rouletteSpr = CircleButtonSprite::createWithSprite("logo-spr.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);
		auto rouletteBtn = CCMenuItemExt::createSpriteExtra(rouletteSpr, [this, list](auto) {
            NewListPopup::create(m_levels, list->m_listName)->show();
        });
		rouletteBtn->setID("level-roulette-button"_spr);
		rightSideMenu->addChild(rouletteBtn);
		rightSideMenu->updateLayout();

        return true;
    }
};