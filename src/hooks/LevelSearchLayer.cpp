#include "../popups/RoulettePopup/roulettePopup.hpp"
#include <Geode/modify/LevelSearchLayer.hpp>

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
	bool init(int type) {
		if (!LevelSearchLayer::init(type)) return false;

		auto filterMenu = this->getChildByID("other-filter-menu");

		auto rouletteSpr = CircleButtonSprite::createWithSprite("logo-spr.png"_spr);
		rouletteSpr->getTopNode()->setScale(rouletteSpr->getScaledContentWidth() / rouletteSpr->getTopNode()->getContentWidth() * 0.7f);
		rouletteSpr->setScale(0.8f);
		auto rouletteBtn = CCMenuItemSpriteExtra::create(rouletteSpr, this, menu_selector(MyLevelSearchLayer::onBtn));
		rouletteBtn->setID("level-roulette-button"_spr);
		filterMenu->addChild(rouletteBtn);
		filterMenu->updateLayout();

		return true;
	}

	void onBtn(CCObject* sender) {
		RoulettePopup::create()->show();
	}
};