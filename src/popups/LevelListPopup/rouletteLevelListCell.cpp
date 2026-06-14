#include "./rouletteLevelListCell.hpp"

RouletteLevelListCell* RouletteLevelListCell::create(std::string listName, matjson::Value levelData) {
	auto ret = new RouletteLevelListCell();
	if (ret && ret->init(listName, levelData)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteLevelListCell::init(std::string listName, matjson::Value levelData) {
    if (!CCNode::init()) return false;
    this->setContentSize({220.0f, 40.0f});

    auto bg = NineSlice::create("square02b_001.png");
	bg->setColor({0, 0, 0});
	bg->setOpacity(96);
	bg->setPosition(m_obContentSize / 2.0f);
	bg->setContentSize(m_obContentSize);
	addChild(bg);

    auto spriteNames = Globals::getSpriteNames();
    auto diff = levelData["diff"].asString().unwrap();
    auto diffIcon = CCSprite::createWithSpriteFrameName(spriteNames[diff].c_str());
    diffIcon->setScale(0.75f);
    diffIcon->setPosition({this->getContentHeight() / 2.0f + 2.0f, this->getContentHeight() / 2.0f});
    addChild(diffIcon);   

    auto levelName = levelData["name"].asString().unwrap();
    auto levelNameLabel = CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
	levelNameLabel->setScale(0.4f);
	levelNameLabel->setAnchorPoint({0.0f, 0.5f});
	levelNameLabel->setPosition({44.0f, m_obContentSize.height / 3 * 2});
	addChild(levelNameLabel);

    auto creatorName = levelData["creator"].asString().unwrap();
	auto creatorNameLabel = CCLabelBMFont::create(fmt::format("By {}", creatorName).c_str(), "goldFont.fnt");
	creatorNameLabel->setScale(0.4f);
	creatorNameLabel->setAnchorPoint({0.0f, 0.5f});
	creatorNameLabel->setPosition({44.0f, m_obContentSize.height / 3});
	addChild(creatorNameLabel);

    auto menu = CCMenu::create();
    menu->setLayout(RowLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
        ->setGap(4.0f)
        ->setAutoScale(false));
    menu->setContentSize({72.0f, 20.0f});
    menu->setAnchorPoint({1.0f, 0.0f});
    menu->setPosition({m_obContentSize.width, 0.0f});
    addChild(menu);

    auto deleteBtnSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    auto deleteBtn = CCMenuItemExt::createSpriteExtra(deleteBtnSpr, [this, listName, levelData](auto) {
        std::string levelID = std::to_string(levelData["levelID"].asInt().unwrap());
        deleteLevel(listName, levelID);
    });
    deleteBtn->setScale(menu->getContentHeight() / m_obContentSize.height);
    deleteBtn->m_baseScale = deleteBtn->getScale();
    deleteBtn->m_scaleMultiplier = 1.15f;
    menu->addChild(deleteBtn);
    menu->updateLayout();

    return true;
}

void RouletteLevelListCell::deleteLevel(std::string listName, std::string levelID) {
    createQuickPopup("Are you sure?", "The level cannot be restored once deleted.", "No", "Yes", [this, listName, levelID](auto, bool yesBtn) {
		if (yesBtn) {
			auto data = Globals::getListsData();

            if (data.size() == 0 || data[listName].size() == 0) return;
            data[listName].erase(levelID);

            Globals::saveListsData(data);

            UpdateLevelListEvent().send(this);
            LevelCountEvent(listName).send(data[listName].size());
		}
	});
}