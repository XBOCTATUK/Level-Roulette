#pragma once

#include "../../Globals.hpp"
#include "../../popups/RoulettePopup/roulettePopup.hpp"
#include "../../events/startSpinEvent.hpp"
#include "../../events/afterSpinEvent.hpp"

class RouletteLayer : public geode::Popup {
public:
	static RouletteLayer* create(float deltaAngle);

protected:
    ListenerHandle m_afterSpinListener;
    
	CCMenu* m_rouletteWheel;
    std::vector<CCSprite*> m_quadrants;
    std::vector<CCLabelBMFont*> m_levelNames;

    bool init(float deltaAngle);
    void spin(float deltaAngle);
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