#include "./newListPopup.hpp"
#include "../ListEditorPopup/listEditorPopup.hpp"

NewListPopup* NewListPopup::create(CCArray* levelList, std::string listName) {
    auto ret = new NewListPopup();
	if (ret && ret->init(levelList, listName)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool NewListPopup::init(CCArray* levelList, std::string listName) {
    if (!Popup::init(320.0f, 120.0f)) return false;

	this->setID("new-list-popup"_spr);
	this->setZOrder(100);
	this->setTitle("Enter list name");

    m_textInput = TextInput::create(200.0f, "List name");
    m_textInput->setMaxCharCount(32);
    m_textInput->setPosition(m_mainLayer->getContentSize() / 2.0f + ccp(0, 8));
    m_textInput->setString(listName);
    m_mainLayer->addChild(m_textInput);

    auto createBtnSpr = ButtonSprite::create("Create");
    auto createBtn = CCMenuItemExt::createSpriteExtra(createBtnSpr, [this, levelList](auto) { createList(levelList); });
    createBtn->setPosition({m_mainLayer->getContentWidth() / 2.0f, 28.0f});
    m_buttonMenu->addChild(createBtn);

    return true;
}

void NewListPopup::createList(CCArray* levelList) {
    std::string listName = m_textInput->getString();
    if (listName.empty()) {
        FLAlertLayer::create(
            "Warning",
            "Please enter a list name.",
            "Ok"
        )->show();
        
        return;
    }

    auto data = Globals::getListsData();
    if (data.contains(listName) && data[listName].size() != 0) {
        FLAlertLayer::create(
            "Warning",
            "This list already exists. Create a new one or edit an existing one.",
            "Ok"
        )->show();
        return;
    }

    data[listName] = matjson::Value::object();
    if (levelList) {
        for (int i = 0; i < levelList->count(); i++) {
            auto level = typeinfo_cast<GJGameLevel*>(levelList->objectAtIndex(i));
            if (!level) continue;

            auto levelData = matjson::Value::object();
            levelData["name"] = std::string(level->m_levelName);
            levelData["levelID"] = level->m_levelID.value();
            levelData["creator"] = std::string(level->m_creatorName);
            levelData["diff"] = Globals::getDiff(level);

            auto idStr = std::to_string(level->m_levelID.value());
            data[listName][idStr] = levelData;
        }
    }

    Globals::saveListsData(data);
    FLAlertLayer::create(
        "Creating a level list",
        "The list has been created successfully.",
        "Ok"
    )->show();

    PopulateListEditorEvent().send();
    onClose(nullptr);
}