#include "RepeatBG.hpp"
#include "../../Utils/CalculateCoverScale.hpp"

using namespace geode::prelude;

namespace TailyUI {
    RepeatBG* RepeatBG::create(const char* bgName) {
        auto ret = new RepeatBG();
        if (ret->init(bgName)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool RepeatBG::init(const char* bgName) {
        if (!CCSprite::initWithFile(bgName)) return false;

        auto winSize = CCDirector::get()->getWinSize();

        setAnchorPoint({ 0.0f, 0.0f });
        setContentSize({ 1024.0f, 512.0f });
        setTextureRect({ 0.0f, 0.0f, 1024.0f, 512.0f });
        setScale(winSize.width / 480.0f);
        setColor({ 40, 125, 255 });

        return true;
    }

    void RepeatBG::onEnter() {
        CCSprite::onEnter();
        scheduleUpdate();
    }

    void RepeatBG::onExit() {
        unscheduleUpdate();
        CCSprite::onExit();
    }

    void RepeatBG::setSpeed(float speed) {
        m_speed = speed;
    }

    void RepeatBG::update(float dt) {
        CCSprite::update(dt);

        setPositionX(getPositionX() - dt * m_speed);

        if (getPositionX() <= -512.0f * getScale()) {
            setPositionX(0.0f);
        }
    }
}