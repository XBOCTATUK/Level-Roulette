#include "./rouletteListCell.hpp"

RouletteListCell* RouletteListCell::create(std::string listName, int levelCount, GJGameLevel* level, bool isLevelAddition) {
	auto ret = new RouletteListCell();
	if (ret && ret->init(listName, levelCount, level, isLevelAddition)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteListCell::init(std::string listName, int levelCount, GJGameLevel* level, bool isLevelAddition) {
	if (!CCNode::init()) return false;

	this->setContentSize({300.0f, 40.0f});
	m_isLevelAddition = isLevelAddition;
	m_levelCount = levelCount;

	auto bg = NineSlice::create("square02b_001.png");
	bg->setColor({0, 0, 0});
	bg->setOpacity(96);
	bg->setPosition(m_obContentSize / 2.0f);
	bg->setContentSize(m_obContentSize);
	addChild(bg);

	m_listNameLabel = CCLabelBMFont::create(listName.c_str(), "bigFont.fnt");
	m_listNameLabel->setScale(0.5f);
	m_listNameLabel->setScale(
		m_listNameLabel->getScaledContentWidth() > 120.0f ?
		120.0f / m_listNameLabel->getContentWidth() :
		0.5f
	);
	m_listNameLabel->setAnchorPoint({0.0f, 0.5f});
	m_listNameLabel->setPosition({10.0f, 28.0f});
	addChild(m_listNameLabel);

	m_levelCountLabel = CCLabelBMFont::create(
		levelCount == 0 ? "Empty list" :
		levelCount == 1 ? "1 level" :
		fmt::format("{} levels", levelCount).c_str(), "goldFont.fnt"
	);
	m_levelCountLabel->setScale(0.4f);
	m_levelCountLabel->setAnchorPoint({0.0f, 0.5f});
	m_levelCountLabel->setPosition({10.0f, 12.0f});
	addChild(m_levelCountLabel);

	m_menu = CCMenu::create();
	m_menu->setLayout(
		RowLayout::create()
		->setGap(5.0f)
		->setAxisReverse(true)
		->setAutoScale(false)
		->setAxisAlignment(AxisAlignment::End)
	);
	m_menu->setContentSize({ 180.0f, 25.0f });
	m_menu->setAnchorPoint({ 1.0f, 0.5f });
	m_menu->setPosition({m_obContentSize.width - 10.0f, m_obContentSize.height / 2.0f});
	m_menu->ignoreAnchorPointForPosition(false);
	addChild(m_menu);

	bool isListSelected = Mod::get()->getSavedValue<std::string>("current-list-name", "") == listName;
	auto btnSpr = ButtonSprite::create(
		m_isLevelAddition ? "Add" : isListSelected ? "Selected" : "Select",
		"bigFont.fnt",
		m_isLevelAddition || !isListSelected ? "GJ_button_01.png" :  "GJ_button_03.png"
	);
	m_btn = CCMenuItemExt::createSpriteExtra(btnSpr, [this, level, listName](auto) {
		if (m_isLevelAddition) onAdd(level, listName);
		else onSelect(listName);
	});
	m_btn->setScale(0.5f);
	m_btn->m_baseScale = m_btn->getScale();
	m_btn->m_scaleMultiplier = 1.15f;
	m_btn->setPosition({m_menu->getContentWidth() - m_btn->getScaledContentWidth() / 2.0f - 10.0f, 20.0f});
	m_menu->addChild(m_btn);

	auto levelsBtnSpr = CCSprite::createWithSpriteFrameName("GJ_viewListsBtn_001.png");
	m_levelsBtn = CCMenuItemExt::createSpriteExtra(levelsBtnSpr, [this, listName](auto) { LevelListPopup::create(listName)->show(); });
	m_levelsBtn->setScale(0.45f);
	m_levelsBtn->m_baseScale = m_levelsBtn->getScale();
	m_levelsBtn->m_scaleMultiplier = 1.15f;
	m_levelsBtn->setPosition({m_btn->getPositionX() - m_btn->getScaledContentWidth() / 2.0f - m_levelsBtn->getScaledContentWidth() / 2.0f - 5.0f, 20.0f});
	m_menu->addChild(m_levelsBtn);

	auto deleteBtnSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    m_deleteBtn = CCMenuItemExt::createSpriteExtra(deleteBtnSpr, [this, listName](auto) {
        deleteList(listName);
    });
    m_deleteBtn->setScale(0.5f);
    m_deleteBtn->m_baseScale = m_deleteBtn->getScale();
	m_deleteBtn->m_scaleMultiplier = 1.15f;
	m_deleteBtn->setPosition({m_levelsBtn->getPositionX() - m_levelsBtn->getScaledContentWidth() / 2.0f - m_deleteBtn->getScaledContentWidth() / 2.0f - 5.0f, 20.0f});
    m_menu->addChild(m_deleteBtn);

	m_toggler = CCMenuItemExt::createTogglerWithStandardSprites(0.5f, [this, listName](CCMenuItemToggler* toggler) {
		SelectListEvent().send(listName);
	});
	m_menu->addChild(m_toggler);

    m_menu->updateLayout();

	m_levelCountListener = LevelCountEvent(listName).listen(
		[this](int count) {
			auto str =
				count == 0 ? "Empty list" :
				count == 1 ? "1 level" :
				fmt::format("{} levels", count);

			if (m_levelCountLabel) {
				m_levelCountLabel->setString(str.c_str());
			}
			m_levelCount = count;

			return ListenerResult::Propagate;
		}
	);

	m_selectionListener = ListSelectionEvent().listen(
		[this, listName]() {
			bool isListSelected = Mod::get()->getSavedValue<std::string>("current-list-name", "") == listName;
			auto btnSpr = static_cast<ButtonSprite*>(m_btn->getNormalImage());
			if (isListSelected) {
				btnSpr->setString("Selected");
				btnSpr->updateBGImage("GJ_button_03.png");
			}
			else {
				btnSpr->setString("Select");
				btnSpr->updateBGImage("GJ_button_01.png");
			}

			m_btn->setPositionX(m_menu->getContentWidth() - m_btn->getScaledContentWidth() / 2.0f - 10.0f);
			m_levelsBtn->setPositionX(m_btn->getPositionX() - m_btn->getScaledContentWidth() / 2.0f - m_levelsBtn->getScaledContentWidth() / 2.0f - 5.0f);
			m_deleteBtn->setPositionX(m_levelsBtn->getPositionX() - m_levelsBtn->getScaledContentWidth() / 2.0f - m_deleteBtn->getScaledContentWidth() / 2.0f - 5.0f);

			m_menu->updateLayout();
		}
	);

	return true;
}

void RouletteListCell::onAdd(GJGameLevel* level, std::string listName) {
	auto data = Globals::getListsData();

	auto levelData = matjson::Value::object();
	levelData["name"] = std::string(level->m_levelName);
	levelData["levelID"] = level->m_levelID.value();
	levelData["creator"] = std::string(level->m_creatorName);
	levelData["diff"] = Globals::getDiff(level);

	auto idStr = std::to_string(level->m_levelID.value());
	data[listName][idStr] = levelData;

	Globals::saveListsData(data);
	
	LevelCountEvent(listName).send(data[listName].size());
}

void RouletteListCell::onSelect(std::string listName) {
	bool isListSelected = Mod::get()->getSavedValue<std::string>("current-list-name", "") == listName;
	if (isListSelected) return;

	if (m_levelCount < 4) {
		FLAlertLayer::create(
			"Levels too few",
			"You cannot select a list that has less than 4 levels.",
			"Ok"
		)->show();
	}
	else if (m_levelCount < 103) {
		createQuickPopup("Few levels", "You don't have enough levels to complete the roulette. Select a list?", "No", "Yes", [this, listName](auto, bool yesBtn) {
			if (yesBtn) {
				Globals::setCurrentListName(listName);
				Globals::getLevelData().clear();
				Mod::get()->setSavedValue<std::string>("current-list-name", listName);
	
				ListSelectionEvent().send();
			}
		});
	}
}

void RouletteListCell::deleteList(std::string listName) {
	createQuickPopup("Are you sure?", "The list cannot be restored once deleted.", "No", "Yes", [this, listName](auto, bool yesBtn) {
		if (yesBtn) {
			auto data = Globals::getListsData();

			if (data.size() == 0) return;
			data.erase(listName);

			Globals::saveListsData(data);

			UpdateListEditorEvent().send(this);
		}
	});
}