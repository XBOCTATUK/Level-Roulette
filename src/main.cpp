#include "./popups/RoulettePopup/roulettePopup.hpp"
#include "./popups/ListEditorPopup/listEditorPopup.hpp"
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

		if (m_level->m_levelID == Globals::getCurrentLevel().levelID && PlayLayer::getCurrentPercentInt() > Globals::getCurrentPercent()) {
			int& currentPercent = Globals::getCurrentPercent();
			currentPercent = PlayLayer::getCurrentPercentInt();
		}
	}
};

class $modify(MyLevelCell, LevelCell) {
	void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
		if (!Mod::get()->getSettingValue<bool>("list-editor-enabled") ||
			level->m_dailyID > 0 ||
			(level->m_levelType != GJLevelType::Saved && level->m_levelType != GJLevelType::SearchResult)) return;

		auto addSpr = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
		auto addBtn = CCMenuItemExt::createSpriteExtra(addSpr, [this, level](auto) { ListEditorPopup::create(level, true)->show(); });
		addBtnPositioning(addBtn);
		addBtn->setID("add-to-list-button"_spr);
		m_mainMenu->addChild(addBtn);
		updateLayout();
	}

	void addBtnPositioning(CCMenuItemSpriteExtra* btn) {
		auto selectToggler = m_mainMenu->getChildByID("select-toggler");
		auto viewBtn = m_mainMenu->getChildByID("view-button");
		auto percentLabel = m_mainLayer->getChildByID("percentage-label");
		auto compIcon = m_mainLayer->getChildByID("completed-icon");

		if (selectToggler) btn->setPosition({ selectToggler->getPositionX() - selectToggler->getContentWidth() - 8.0f, selectToggler->getPositionY() });
		else if (m_compactView && percentLabel) btn->setPosition({ percentLabel->getPositionX() - percentLabel->getScaledContentWidth() / 2.0f - btn->getContentWidth() / 2.0f - 8.0f - m_mainMenu->getContentWidth() / 2.0f, percentLabel->getPositionY() - m_mainMenu->getContentHeight() / 2.0f });
		else if (m_compactView && compIcon) btn->setPosition({ compIcon->getPositionX() - compIcon->getScaledContentWidth() / 2.0f - btn->getContentWidth() / 2.0f - 8.0f - m_mainMenu->getContentWidth() / 2.0f, compIcon->getPositionY() - m_mainMenu->getContentHeight() / 2.0f });
		else if (m_compactView) btn->setPosition({ viewBtn->getPositionX() - viewBtn->getContentWidth() / 2.0f - btn->getContentWidth() / 2.0f - 8.0f, viewBtn->getPositionY() });
		else btn->setPosition({ -20.5f, -115.0f });
	}
};