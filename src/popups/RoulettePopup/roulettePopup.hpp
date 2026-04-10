#pragma once

#include "../../Globals.hpp"
#include "../../events/afterSpinEvent.hpp"

class RoulettePopup : public geode::Popup {
public:
	static RoulettePopup* create();

protected:
    ListenerHandle m_spinListener;
    ListenerHandle m_afterSpinListener;

	CCLabelBMFont* m_spinsCount;
    CCMenuItemSpriteExtra* m_spinBtn;
    CCMenuItemSpriteExtra* m_playBtn;
    CCMenuItemSpriteExtra* m_skipBtn;
    CCMenuItemSpriteExtra* m_nextBtn;
    CCMenuItemSpriteExtra* m_resetBtn;
    CCMenuItemSpriteExtra* m_listsBtn;

    CCSprite* m_diffSpr;
    CCLabelBMFont* m_levelName;
    CCLabelBMFont* m_requirePercent;
    CCLabelBMFont* m_skipsCount;

    CCScale9Sprite* m_requirePercentBG;

	bool init();
    bool readLevelData();
    void afterSpinOnPopup();
    void resetRoulette();
    void onListsBtn();
    void levelChoice();
    void keyDown(enumKeyCodes key, double d) override;

    friend class RouletteLayer;
};