#pragma once

#include "./rouletteLevelListCell.hpp"

class LevelListPopup : public geode::Popup {
public:
    static LevelListPopup* create(std::string listName);
protected:
    ListenerHandle m_updateListener;
    ScrollLayer* m_scrollingLayer;
    CCLabelBMFont* m_emptyScrollLabel;

    bool init(std::string listName);
    void populateScroll(std::string listName);
    void clearList(std::string listName);

    friend class RouletteLevelListCell;
};