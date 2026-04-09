#pragma once

#include "./rouletteListCell.hpp"
#include "../NewListPopup/newListPopup.hpp"

class NewListPopup;

class ListEditorPopup : public geode::Popup {
public:
    static ListEditorPopup* create(GJGameLevel* level, bool isLevelAddition);
protected:
    ListenerHandle m_updateListener;
    ListenerHandle m_populateListener;
    bool m_isLevelAddition = false;
    ScrollLayer* m_scrollingLayer;
    CCLabelBMFont* m_emptyScrollLabel;
    GJGameLevel* m_level;

    bool init(GJGameLevel* level, bool isLevelAddition);
    void populateScroll();

    friend class NewListPopup;
};