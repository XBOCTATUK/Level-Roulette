#include "./listEditorPopup.hpp"

RouletteListCell* RouletteListCell::create() {
	auto ret = new RouletteListCell();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

ListEditorPopup* ListEditorPopup::create(bool isLevelAddition) {
	auto ret = new ListEditorPopup();
	if (ret && ret->init(isLevelAddition)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool ListEditorPopup::init(bool isLevelAddition) {
    if (!Popup::init(160.0f, 120.0f)) return false;

    this->setID("list-editor-menu"_spr);
	this->setZOrder(100);
	this->setTitle(isLevelAddition ? "Select level list" : "Your level lists");



    return true;
}