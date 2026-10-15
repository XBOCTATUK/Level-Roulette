#pragma once

namespace TailyUI {
    class RepeatBG : public cocos2d::CCSprite {
    public:
        static RepeatBG* create(const char* bgName);
        void setSpeed(float speed);

    protected:
        float m_speed = 32.0f;

        bool init(const char* bgName);
        virtual void update(float dt) override;

        virtual void onEnter() override;
        virtual void onExit() override;
    };
}