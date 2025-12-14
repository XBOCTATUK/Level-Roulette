#include "./roulettePopup/roulettePopup.hpp"
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelCell.hpp>

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
	bool init(int type) {
		if (!LevelSearchLayer::init(type)) return false;

		auto filterMenu = this->getChildByID("other-filter-menu");

		auto rouletteSpr = CircleButtonSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
		rouletteSpr->getTopNode()->setPosition({ 23.375f, 29.5f });
		rouletteSpr->getTopNode()->setScale(1.0f);
		rouletteSpr->setScale(0.8f);
		auto rouletteBtn = CCMenuItemSpriteExtra::create(rouletteSpr, this, menu_selector(MyLevelSearchLayer::onBtn));
		rouletteBtn->setID("worst-demons-button");
		filterMenu->addChild(rouletteBtn);
		filterMenu->updateLayout();

		return true;
	}

	void onBtn(CCObject* sender) {
		RoulettePopup::create()->show();
	}
};