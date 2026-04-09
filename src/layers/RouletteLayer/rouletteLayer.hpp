#pragma once

#include "../../Globals.hpp"
#include "../../popups/RoulettePopup/roulettePopup.hpp"

class RouletteLayer : public geode::Popup {
public:
	static RouletteLayer* create(RoulettePopup* popup);

protected:
    RoulettePopup* m_popup;
    
	CCMenu* m_rouletteWheel;
    std::vector<CCSprite*> m_quadrants;
    std::vector<CCLabelBMFont*> m_levelNames;

    bool init(RoulettePopup* popup);
    void spin(RoulettePopup* popup);
    void afterSpinOnLayer();
    void keyDown(enumKeyCodes key, double d) override;
};

class SmoothExponentialOut : public CCActionEase {
public:
    static SmoothExponentialOut* create(CCActionInterval* pAction) {
        SmoothExponentialOut* ret = new SmoothExponentialOut();
        if (ret && ret->initWithAction(pAction)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
	    return nullptr;
    }

    virtual void update(float time) override {
        float easedTime = 1.0f - powf(2.5f, -8.0f * time);
        m_pInner->update(easedTime);
    }
};