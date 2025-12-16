#include <Geode/Geode.hpp>
#include <Geode/binding/LevelManagerDelegate.hpp>
#include <Geode/binding/SetIDPopupDelegate.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <random>

using namespace geode::prelude;

struct LevelData {
    std::string name;
    std::string id;
    std::string diff;
    int likes;
};

class RoulettePopup : public geode::Popup<> {
public:
	static RoulettePopup* create();
    void keyDown(enumKeyCodes key) override;

protected:
	CCLabelBMFont* m_spinsCount;
    CCMenuItemSpriteExtra* m_spinBtn;
    CCMenuItemSpriteExtra* m_playBtn;
    CCMenuItemSpriteExtra* m_skipBtn;
    CCMenuItemSpriteExtra* m_nextBtn;
    CCMenuItemSpriteExtra* m_resetBtn;

    CCSprite* m_diffSpr;
    CCSprite* m_dislikeSpr;
    CCLabelBMFont* m_levelName;
    CCLabelBMFont* m_levelDislikes;
    CCLabelBMFont* m_requirePercent;
    CCLabelBMFont* m_skipsCount;

    CCScale9Sprite* m_requirePercentBG;

	bool setup() override;
    void afterSpinOnPopup();
    void resetRoulette();

    friend class RouletteLayer;
};

class RouletteLayer : public geode::Popup<> {
public:
	static RouletteLayer* create();
    void keyDown(enumKeyCodes key) override;

protected:
	CCMenu* m_rouletteWheel;
    CCSprite* m_quadrant1;
    CCSprite* m_quadrant2;
    CCSprite* m_quadrant3;
    CCSprite* m_quadrant4;

    CCSprite* m_diffSpr1;
    CCSprite* m_diffSpr2;
    CCSprite* m_diffSpr3;
    CCSprite* m_diffSpr4;

    CCSprite* m_dislikeSpr1;
    CCSprite* m_dislikeSpr2;
    CCSprite* m_dislikeSpr3;
    CCSprite* m_dislikeSpr4;

    CCLabelBMFont* m_levelName1;
    CCLabelBMFont* m_levelName2;
    CCLabelBMFont* m_levelName3;
    CCLabelBMFont* m_levelName4;

    CCLabelBMFont* m_levelDislikes1;
    CCLabelBMFont* m_levelDislikes2;
    CCLabelBMFont* m_levelDislikes3;
    CCLabelBMFont* m_levelDislikes4;

    bool setup() override;
    void readLevelData();
    void spin();
    void afterSpinOnLayer();
};

class SmoothExponentialOut : public CCActionEase {
public:
    static SmoothExponentialOut* create(CCActionInterval* pAction) {
        SmoothExponentialOut* pRet = new SmoothExponentialOut();
        if (pRet && pRet->initWithAction(pAction)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }

    virtual void update(float time) override {
        float easedTime = 1.0f - powf(2.5f, -8.0f * time);
        m_pInner->update(easedTime);
    }
};