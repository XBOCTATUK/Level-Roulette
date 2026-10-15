#pragma once

namespace TailyUI {
    class FortuneWheel : public cocos2d::CCNode {
    public:
        static FortuneWheel* create();
    
    protected:
        cocos2d::CCDrawNode* m_drawNode = nullptr;
    
        bool init();
    };
}