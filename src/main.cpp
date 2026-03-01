#include "./roulettePopup/roulette.hpp"
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/PlayLayer.hpp>

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
	bool init(int type) {
		if (!LevelSearchLayer::init(type)) return false;

		auto filterMenu = this->getChildByID("other-filter-menu");

		auto rouletteSpr = CircleButtonSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
		rouletteSpr->getTopNode()->setPosition({ 23.375f, 29.5f });
		rouletteSpr->getTopNode()->setScale(1.0f);
		rouletteSpr->setScale(0.8f);
		auto rouletteBtn = CCMenuItemSpriteExtra::create(rouletteSpr, this, menu_selector(MyLevelSearchLayer::onBtn));
		rouletteBtn->setID("levels-roulette-button"_spr);
		filterMenu->addChild(rouletteBtn);
		filterMenu->updateLayout();

		return true;
	}

	void onBtn(CCObject* sender) {
		RoulettePopup::create()->show();
	}
};

class $modify(PlayLayer) {
	void destroyPlayer(PlayerObject* player, GameObject* object) {
		PlayLayer::destroyPlayer(player, object);

		if (m_level->m_levelID == g_currentLvl.levelID && PlayLayer::getCurrentPercentInt() > g_currentPercent)
			g_currentPercent = PlayLayer::getCurrentPercentInt();
	}
};

class $modify(LevelCell) {
	void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
		if (!Mod::get()->getSettingValue<bool>("list-editor-enabled")) return;
		auto selectToggler = m_mainMenu->getChildByID("select-toggler");
		auto viewBtn = m_mainMenu->getChildByID("view-button");
		auto percentLabel = m_mainLayer->getChildByID("percentage-label");
		auto compIcon = m_mainLayer->getChildByID("completed-icon");

		auto addSpr = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
		auto addBtn = CCMenuItemExt::createSpriteExtra(addSpr, [this](auto) {});
		if (selectToggler) addBtn->setPosition({ selectToggler->getPositionX() - selectToggler->getContentWidth() - 8.0f, selectToggler->getPositionY() });
		else if (m_compactView && percentLabel) addBtn->setPosition({ percentLabel->getPositionX() - percentLabel->getContentWidth() * percentLabel->getScale() / 2.0f - addBtn->getContentWidth() / 2.0f - 8.0f - m_mainMenu->getContentWidth() / 2.0f, percentLabel->getPositionY() - m_mainMenu->getContentHeight() / 2.0f });
		else if (m_compactView && compIcon) addBtn->setPosition({ compIcon->getPositionX() - compIcon->getContentWidth() * compIcon->getScale() / 2.0f - addBtn->getContentWidth() / 2.0f - 8.0f - m_mainMenu->getContentWidth() / 2.0f, compIcon->getPositionY() - m_mainMenu->getContentHeight() / 2.0f });
		else if (m_compactView) addBtn->setPosition({ viewBtn->getPositionX() - viewBtn->getContentWidth() / 2.0f - addBtn->getContentWidth() / 2.0f - 8.0f, viewBtn->getPositionY() });
		else addBtn->setPosition({ -20.5f, -115.0f });
		addBtn->setID("add-to-list-button"_spr);
		m_mainMenu->addChild(addBtn);
		updateLayout();
	}
};