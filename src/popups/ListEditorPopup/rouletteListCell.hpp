#pragma once

#include "../../Globals.hpp"
#include "../LevelListPopup/levelListPopup.hpp"
#include "../../events/levelCountEvent.hpp"
#include "../../events/updateListEditorEvent.hpp"
#include "../../events/populateListEditorEvent.hpp"
#include "../../events/listSelectionEvent.hpp"

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

    bool init(std::string listName, int levelCount, GJGameLevel* level, bool isLevelAddition);
    void onAdd(GJGameLevel* level, std::string listName);
    void onSelect(std::string listName);
	void deleteList(std::string listName);

    std::string getDiff(GJGameLevel* level) {
		if (level->m_stars == 10 && level->m_demonDifficulty == 3) return "EasyDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 4) return "MediumDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 0) return "HardDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 5) return "InsaneDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 6) return "ExtremeDemon";
		else if (level->getAverageDifficulty() == 0) return "N/A";
        else if (level->getAverageDifficulty() == -1) return "Auto";
		else if (level->getAverageDifficulty() == 1) return "Easy";
		else if (level->getAverageDifficulty() == 2) return "Normal";
		else if (level->getAverageDifficulty() == 3) return "Hard";
		else if (level->getAverageDifficulty() == 4) return "Harder";
		else if (level->getAverageDifficulty() == 5) return "Insane";
        else return "N/A";
    }
};