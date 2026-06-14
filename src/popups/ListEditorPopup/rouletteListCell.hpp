#pragma once

#include "../../Globals.hpp"
#include "../LevelListPopup/levelListPopup.hpp"
#include "../../events/levelCountEvent.hpp"
#include "../../events/updateListEditorEvent.hpp"
#include "../../events/populateListEditorEvent.hpp"
#include "../../events/listSelectionEvent.hpp"
#include "../../events/selectListEvent.hpp"

class RouletteListCell : public CCNode {
public:
    static RouletteListCell* create(std::string listName, int levelCount, GJGameLevel* level, bool isLevelAddition);
protected:
	ListenerHandle m_levelCountListener;
	ListenerHandle m_selectionListener;
	bool m_isLevelAddition = false;
	int m_levelCount;
	CCLabelBMFont* m_listNameLabel;
	CCLabelBMFont* m_levelCountLabel;
	CCMenu* m_menu;
	CCMenuItemSpriteExtra* m_btn;
	CCMenuItemSpriteExtra* m_levelsBtn;
	CCMenuItemSpriteExtra* m_deleteBtn;
	CCMenuItemToggler* m_toggler;

    bool init(std::string listName, int levelCount, GJGameLevel* level, bool isLevelAddition);
    void onAdd(GJGameLevel* level, std::string listName);
    void onSelect(std::string listName);
	void deleteList(std::string listName);
};