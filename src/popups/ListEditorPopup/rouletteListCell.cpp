#include "./rouletteListCell.hpp"

RouletteListCell* RouletteListCell::create(std::string listName, int levelsCount, GJGameLevel* level, bool isLevelAddition) {
	auto ret = new RouletteListCell();
	if (ret && ret->init(listName, levelsCount, level, isLevelAddition)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteListCell::init(std::string listName, int levelsCount, GJGameLevel* level, bool isLevelAddition) {
	if (!CCNode::init()) return false;

	this->setContentSize({220.0f, 40.0f});
	m_isLevelAddition = isLevelAddition;

	auto bg = NineSlice::create("square02b_001.png");
	bg->setColor({0, 0, 0});
	bg->setOpacity(96);
	bg->setPosition(m_obContentSize / 2.0f);
	bg->setContentSize(m_obContentSize);
	addChild(bg);

	m_listNameLabel = CCLabelBMFont::create(listName.c_str(), "bigFont.fnt");
	m_listNameLabel->setScale(
		m_listNameLabel->getContentWidth() > 200.0f ?
		0.5f * 200.0f / m_listNameLabel->getContentWidth() :
		0.5f
	);
	m_listNameLabel->setAnchorPoint({0.0f, 0.5f});
	m_listNameLabel->setPosition({10.0f, 28.0f});
	addChild(m_listNameLabel);

	m_levelsCountLabel = CCLabelBMFont::create(
		levelsCount == 0 ? "Empty list" :
		levelsCount == 1 ? "1 level" :
		fmt::format("{} levels", levelsCount).c_str(), "goldFont.fnt"
	);
	m_levelsCountLabel->setScale(0.4f);
	m_levelsCountLabel->setAnchorPoint({0.0f, 0.5f});
	m_levelsCountLabel->setPosition({10.0f, 12.0f});
	addChild(m_levelsCountLabel);

	auto menu = CCMenu::create();
	menu->setContentSize(m_obContentSize);
	menu->setPosition(m_obContentSize / 2.0f);
	menu->ignoreAnchorPointForPosition(false);
	addChild(menu);

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
	m_btn->setPosition({menu->getContentWidth() - m_btn->getScaledContentWidth() / 2.0f - 10.0f, 20.0f});
	menu->addChild(m_btn);

	auto levelsBtnSpr = CCSprite::createWithSpriteFrameName("GJ_viewListsBtn_001.png");
	auto levelsBtn = CCMenuItemExt::createSpriteExtra(levelsBtnSpr, [this, listName](auto) { LevelListPopup::create(listName)->show(); });
	levelsBtn->setScale(0.45f);
	levelsBtn->m_baseScale = levelsBtn->getScale();
	levelsBtn->m_scaleMultiplier = 1.15f;
	levelsBtn->setPosition({m_btn->getPositionX() - m_btn->getScaledContentWidth() / 2.0f - levelsBtn->getScaledContentWidth() / 2.0f - 5.0f, 20.0f});
	menu->addChild(levelsBtn);

	auto deleteBtnSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    auto deleteBtn = CCMenuItemExt::createSpriteExtra(deleteBtnSpr, [this, listName](auto) {
        deleteList(listName);
    });
    deleteBtn->setScale(0.5f);
    deleteBtn->m_baseScale = deleteBtn->getScale();
	deleteBtn->m_scaleMultiplier = 1.15f;
	deleteBtn->setPosition({levelsBtn->getPositionX() - levelsBtn->getScaledContentWidth() / 2.0f - deleteBtn->getScaledContentWidth() / 2.0f - 5.0f, 20.0f});
    menu->addChild(deleteBtn);

    menu->updateLayout();

	m_listener = LevelCountEvent(listName).listen(
		[this](int count) {
			log::info("Event | {}", count);
			auto str =
				count == 0 ? "Empty list" :
				count == 1 ? "1 level" :
				fmt::format("{} levels", count);

			if (m_levelsCountLabel)
				m_levelsCountLabel->setString(str.c_str());

			return ListenerResult::Propagate;
		}
	);

	return true;
}

void RouletteListCell::onAdd(GJGameLevel* level, std::string listName) {
	auto data = Globals::getListsData();

	auto levelData = matjson::Value::object();
	levelData["name"] = level->m_levelName;
	levelData["levelID"] = level->m_levelID.value();
	levelData["creator"] = level->m_creatorName;
	levelData["diff"] = getDiff(level);

	auto idStr = std::to_string(level->m_levelID.value());
	auto& levelList = data[listName][idStr] = levelData;

	Globals::setListsData(data);
	
	log::info("{}", data[listName].size());
	LevelCountEvent(listName).send(data[listName].size());
}

void RouletteListCell::onSelect(std::string listName) {
	Globals::setCurrentListName(listName);
	Globals::getLevelData().clear();
	Mod::get()->setSavedValue<std::string>("current-list-name", listName);

	auto btnSpr = static_cast<ButtonSprite*>(m_btn->getNormalImage());
	btnSpr->setString("Selected");
	btnSpr->updateBGImage("GJ_button_03.png");
}

void RouletteListCell::deleteList(std::string listName) {
	createQuickPopup("Are you sure?", "The list cannot be restored once deleted.", "No", "Yes", [this, listName](auto, bool yesBtn) {
		if (yesBtn) {
			auto data = Globals::getListsData();

			if (data.size() == 0) return;
			data.erase(listName);

			Globals::setListsData(data);

			UpdateListEditorEvent().send(this);
		}
	});
}