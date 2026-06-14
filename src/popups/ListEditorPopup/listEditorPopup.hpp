#pragma once

#include "./rouletteListCell.hpp"
#include "../NewListPopup/newListPopup.hpp"
#include "../../events/selectListEvent.hpp"

class NewListPopup;

class ListEditorPopup : public geode::Popup {
public:
    static ListEditorPopup* create(GJGameLevel* level, bool isLevelAddition);
protected:
    ListenerHandle m_updateListener;
    ListenerHandle m_populateListener;
    ListenerHandle m_selectListListener;
    bool m_isLevelAddition = false;
    ScrollLayer* m_scrollingLayer;
    CCLabelBMFont* m_emptyScrollLabel;
    GJGameLevel* m_level;
    std::set<std::string> m_selectedLists;

    bool init(GJGameLevel* level, bool isLevelAddition);
    void populateScroll();

    friend class NewListPopup;
};