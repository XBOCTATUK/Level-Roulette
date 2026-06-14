#include <Geode/modify/LevelCell.hpp>
#include "../popups/ListEditorPopup/listEditorPopup.hpp"

using namespace geode::prelude;

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
		else btn->setPosition({ viewBtn->getPositionX() - viewBtn->getContentWidth() / 2.0f - btn->getContentWidth() / 2.0f - 8.0f, viewBtn->getPositionY() });
	}
};