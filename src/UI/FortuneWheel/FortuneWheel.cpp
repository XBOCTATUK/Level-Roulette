#include "FortuneWheel.hpp"

using namespace geode::prelude;

namespace TailyUI {
    FortuneWheel* FortuneWheel::create() {
        auto ret = new FortuneWheel();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool FortuneWheel::init() {
        if (!CCNode::init()) return false;

        setContentSize({ 240.0f, 240.0f });
        setAnchorPoint({ 0.5f, 0.5f });

        m_drawNode = CCDrawNode::create();
        m_drawNode->drawCircle(
            getContentSize() / 2.0f,
            getContentWidth() / 2.0f,
            { 1.0f, 1.0f, 1.0f, 1.0f },
            0.0f,
            { 1, 1, 1, 1 },
            256
        );
        m_drawNode->drawCircle(
            getContentSize() / 2.0f,
            getContentWidth() / 2.0f - 5.0f,
            { 0.0f, 0.0f, 0.0f, 1.0f },
            0.0f,
            { 1.0f, 1.0f, 1.0f, 1.0f },
            256
        );

        addChild(m_drawNode);

        return true;
    }
}