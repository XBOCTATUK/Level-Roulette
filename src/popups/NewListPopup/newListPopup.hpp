#pragma once

#include "../../Globals.hpp"
#include "../../events/updateListEditorEvent.hpp"
#include "../../events/populateListEditorEvent.hpp"

class ListEditorPopup;

class NewListPopup : public geode::Popup {
public:
    static NewListPopup* create(CCArray* levelList = {}, std::string listName = "");
private:
    TextInput* m_textInput;

    bool init(CCArray* levelList, std::string listName);
    void createList(CCArray* levelList);
};