#include "./newListPopup.hpp"
#include "../ListEditorPopup/listEditorPopup.hpp"

NewListPopup* NewListPopup::create() {
    auto ret = new NewListPopup();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool NewListPopup::init() {
    if (!Popup::init(320.0f, 120.0f)) return false;

	this->setID("new-list-popup"_spr);
	this->setZOrder(100);
	this->setTitle("Enter list name");

    m_textInput = TextInput::create(200.0f, "List name");
    m_textInput->setMaxCharCount(32);
    m_textInput->setPosition(m_mainLayer->getContentSize() / 2.0f + ccp(0, 8));
    m_mainLayer->addChild(m_textInput);

    auto createBtnSpr = ButtonSprite::create("Create");
    auto createBtn = CCMenuItemExt::createSpriteExtra(createBtnSpr, [this](auto) { createList(); });
    createBtn->setPosition({m_mainLayer->getContentWidth() / 2.0f, 28.0f});
    m_buttonMenu->addChild(createBtn);

    return true;
}

void NewListPopup::createList() {
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
    if (data[listName].size() != 0) {
        FLAlertLayer::create(
            "Warning",
            "This list already exists. Create a new one or edit an existing one.",
            "Ok"
        )->show();
        return;
    }
    data[listName] = matjson::Value::object();

    Globals::setListsData(data);

    PopulateListEditorEvent().send();
    onClose(nullptr);
}