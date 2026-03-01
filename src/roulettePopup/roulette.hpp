#include <Geode/Geode.hpp>
#include <Geode/binding/LevelManagerDelegate.hpp>
#include <Geode/binding/SetIDPopupDelegate.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <random>

using namespace geode::prelude;

struct LevelData {
    std::string name;
    std::string diff;
    int levelID;
    int likes;
};

class RoulettePopup : public geode::Popup {
public:
	static RoulettePopup* create();

protected:
	CCLabelBMFont* m_spinsCount;
    CCMenuItemSpriteExtra* m_spinBtn;
    CCMenuItemSpriteExtra* m_playBtn;
    CCMenuItemSpriteExtra* m_skipBtn;
    CCMenuItemSpriteExtra* m_nextBtn;
    CCMenuItemSpriteExtra* m_resetBtn;

    CCSprite* m_diffSpr;
    CCLabelBMFont* m_levelName;
    CCLabelBMFont* m_requirePercent;
    CCLabelBMFont* m_skipsCount;

    CCScale9Sprite* m_requirePercentBG;

	bool init();
    void afterSpinOnPopup();
    void resetRoulette();
    void keyDown(enumKeyCodes key, double d) override;

    friend class RouletteLayer;
};

class RouletteLayer : public geode::Popup {
public:
	static RouletteLayer* create(RoulettePopup* popup);

protected:
    RoulettePopup* m_popup;
    
	CCMenu* m_rouletteWheel;
    std::vector<CCSprite*> m_quadrants;
    std::vector<CCLabelBMFont*> m_levelNames;

    bool init(RoulettePopup* popup);
    void readLevelData();
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

inline std::vector<int> g_levels;
inline std::vector<int> g_usedLevels;
inline std::vector<int> g_lvls;
inline LevelData g_currentLvl;
inline LevelData g_pastCurrentLvl;
inline int g_spinsCount = 0;
inline int g_skipsCount = Mod::get()->getSettingValue<int>("skips-count");
inline int g_requirePercent = 1;
inline int g_currentPercent = 0;
inline bool g_afterSpin = true;
inline std::unordered_map<std::string, LevelData> g_levelData;

inline std::unordered_map<std::string, ccColor3B> g_quadrantColors = {
    {"Auto", {234, 173, 86}},
    {"Easy", {0, 212, 255}},
    {"Normal", {0, 255, 37}},
    {"Hard", {255, 187, 0}},
    {"Harder", {255, 2, 7}},
    {"Insane", {235, 110, 198}},
    {"EasyDemon", {148, 71, 249}},
    {"MediumDemon", {239, 26, 166}},
    {"HardDemon", {255, 57, 80}},
    {"InsaneDemon", {242, 29, 24}},
    {"ExtremeDemon", {176, 0, 0}}
};
inline std::unordered_map<std::string, std::string> g_spriteNames = {
    {"Auto", "diffIcon_auto_btn_001.png"},
    {"Easy", "diffIcon_01_btn_001.png"},
    {"Normal", "diffIcon_02_btn_001.png"},
    {"Hard", "diffIcon_03_btn_001.png"},
    {"Harder", "diffIcon_04_btn_001.png"},
    {"Insane", "diffIcon_05_btn_001.png"},
    {"EasyDemon", "diffIcon_07_btn_001.png"},
    {"MediumDemon", "diffIcon_08_btn_001.png"},
    {"HardDemon", "diffIcon_06_btn_001.png"},
    {"InsaneDemon", "diffIcon_09_btn_001.png"},
    {"ExtremeDemon", "diffIcon_10_btn_001.png"}
};
inline std::unordered_map<std::string, std::string> g_spriteNames1 = {
    {"Auto", "difficulty_auto_btn_001.png"},
    {"Easy", "difficulty_01_btn_001.png"},
    {"Normal", "difficulty_02_btn_001.png"},
    {"Hard", "difficulty_03_btn_001.png"},
    {"Harder", "difficulty_04_btn_001.png"},
    {"Insane", "difficulty_05_btn_001.png"},
    {"EasyDemon", "difficulty_07_btn2_001.png"},
    {"MediumDemon", "difficulty_08_btn2_001.png"},
    {"HardDemon", "difficulty_06_btn2_001.png"},
    {"InsaneDemon", "difficulty_09_btn2_001.png"},
    {"ExtremeDemon", "difficulty_10_btn2_001.png"}
};