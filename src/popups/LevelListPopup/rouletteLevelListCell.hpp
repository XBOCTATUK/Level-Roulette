#pragma once

#include "../../Globals.hpp"
#include "../../events/levelCountEvent.hpp"
#include "../../events/updateLevelListEvent.hpp"

class RouletteLevelListCell : public CCNode {
public:
    static RouletteLevelListCell* create(std::string listName, matjson::Value levelData);
protected:
    bool init(std::string listName, matjson::Value levelData);
    void deleteLevel(std::string listName, std::string levelID);
};