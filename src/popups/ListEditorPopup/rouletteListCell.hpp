#pragma once

#include "../../Globals.hpp"
#include "../LevelListPopup/levelListPopup.hpp"
#include "../../events/levelCountEvent.hpp"
#include "../../events/updateListEditorEvent.hpp"
#include "../../events/populateListEditorEvent.hpp"

class RouletteListCell : public CCNode {
public:
    static RouletteListCell* create(std::string listName, int levelsCount, GJGameLevel* level, bool isLevelAddition);
protected:
	ListenerHandle m_listener;
    bool m_isLevelAddition = false;
	CCLabelBMFont* m_listNameLabel;
	CCLabelBMFont* m_levelsCountLabel;
	CCMenuItemSpriteExtra* m_btn;

    bool init(std::string listName, int levelsCount, GJGameLevel* level, bool isLevelAddition);
    void onAdd(GJGameLevel* level, std::string listName);
    void onSelect(std::string listName);
	void deleteList(std::string listName);

    std::string getDiff(GJGameLevel* level) {
		if (level->m_stars == 10 && level->m_demonDifficulty == 3) return "EasyDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 4) return "MediumDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 0) return "HardDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 5) return "InsaneDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 6) return "ExtremeDemon";
        else if (level->m_stars == 1 || level->getAverageDifficulty() == 0) return "Auto";
		else if (level->m_stars == 2 || level->getAverageDifficulty() == 1) return "Easy";
		else if (level->m_stars == 3 || level->getAverageDifficulty() == 2) return "Normal";
		else if (level->m_stars == 4 || level->m_stars == 5 || level->getAverageDifficulty() == 3) return "Hard";
		else if (level->m_stars == 6 || level->m_stars == 7 || level->getAverageDifficulty() == 4) return "Harder";
		else if (level->m_stars == 8 || level->m_stars == 9 || level->getAverageDifficulty() == 5) return "Insane";
        else return "N/A";
    }
};