#include "RouletteLayer.hpp"

using namespace geode::prelude;
using namespace TailyUI;

RouletteLayer* RouletteLayer::create() {
	auto ret = new RouletteLayer();
	if (ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

CCScene* RouletteLayer::scene() {
	auto ret = CCScene::create();
	AppDelegate::get()->m_runningScene = ret;
	ret->addChild(RouletteLayer::create());
	return ret;
}

bool RouletteLayer::init() {
    if (!CCLayer::init()) return false;

	setID("RouletteLayer");
	setPosition(getContentSize() / 2.0f);

	m_bg = TailyUI::RepeatBG::create("game_bg_01_001.png");
	addChildAtPosition(
		m_bg, Anchor::BottomLeft, {}
	);

	m_mainContent = CCNode::create();
	m_mainContent->setContentSize({
		std::min(getContentWidth(), 569.9f),
		getContentHeight()
	});
	m_mainContent->setAnchorPoint({ 0.5f, 0.5f });
	addChildAtPosition(
		m_mainContent, Anchor::Center, {}
	);

	m_leftContent = CCNode::create();
	m_leftContent->setContentSize({
		m_mainContent->getContentWidth() * 0.6f,
		m_mainContent->getContentHeight()
	});
	m_leftContent->setAnchorPoint({ 0.0f, 0.5f });
	m_mainContent->addChildAtPosition(
		m_leftContent, Anchor::Left, {}
	);

	m_rightContent = CCNode::create();
	m_rightContent->setContentSize({
		m_mainContent->getContentWidth() * 0.4f,
		m_mainContent->getContentHeight()
	});
	m_rightContent->setAnchorPoint({ 1.0f, 0.5f });
	m_mainContent->addChildAtPosition(
		m_rightContent, Anchor::Right, {}
	);

	auto fortuneWheel = FortuneWheel::create();
	m_leftContent->addChildAtPosition(
		fortuneWheel, Anchor::Center, { 0.0f, -25.0f }
	);

	m_titleBG = NineSlice::create("square02b_001.png");
	m_titleBG->setContentSize({
		(m_leftContent->getContentWidth() - 20.0f) * 2.0f,
		40.0f * 2.0f
	});
	m_titleBG->setScale(0.5f);
	m_titleBG->setColor({ 0, 0, 0 });
	m_titleBG->setOpacity(51);
	m_leftContent->addChildAtPosition(
		m_titleBG, Anchor::Top, { 0.0f, -30.0f }
	);

	m_levelName = Label::create(
		"Artemis 13", "bigFont.fnt"
	);
	m_levelName->setScale(1.0f);
	m_leftContent->addChildAtPosition(
		m_levelName, Anchor::Top, { 0.0f, -22.0f }
	);

	m_creatorName = Label::create(
		"By mvngo", "goldFont.fnt"
	);
	m_creatorName->setScale(0.5f);
	m_leftContent->addChildAtPosition(
		m_creatorName, Anchor::Top, { 0.0f, -42.0f }
	);

	m_mainMenu = CCMenu::create();
	m_mainMenu->setLayout(
		SimpleColumnLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::End)
		->setCrossAxisAlignment(CrossAxisAlignment::Center)
		->setMainAxisDirection(AxisDirection::BottomToTop)
		->setCrossAxisScaling(AxisScaling::None)
		->setGap(5.0f)
	);
	m_mainMenu->setContentSize(m_rightContent->getContentSize() - ccp( 20.0f, 20.0f ));
	m_mainMenu->ignoreAnchorPointForPosition(false);
	m_rightContent->addChildAtPosition(
		m_mainMenu, Anchor::Center, {}
	);

	int btnWidth = static_cast<int>(m_mainMenu->getContentWidth()) - 16;
	int halfBtnWidth = btnWidth / 2 - 10;

	auto closeBtnSpr = ButtonSprite::create(
		"Close", btnWidth, btnWidth, 0.8f, false,
		"goldFont.fnt", "GJ_button_06.png"
	);

	m_closeBtn = CCMenuItemExt::createSpriteExtra(
		closeBtnSpr, [this](auto) {
			CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
		}
	);
	m_mainMenu->addChild(m_closeBtn);

	auto spinBtnSpr = ButtonSprite::create(
		"Spin", halfBtnWidth, halfBtnWidth, 0.8f, false,
		"bigFont.fnt", "GJ_button_01.png"
	);

	m_spinBtn = CCMenuItemExt::createSpriteExtra(
		spinBtnSpr, [this](auto) {
			
		}
	);

	auto skipBtnSpr = ButtonSprite::create(
		"Skip", halfBtnWidth, halfBtnWidth, 0.8f, false,
		"bigFont.fnt", "GJ_button_03.png"
	);

	m_skipBtn = CCMenuItemExt::createSpriteExtra(
		skipBtnSpr, [this](auto) {
			
		}
	);

	auto rouletteBtnMenu = CCMenu::create(m_spinBtn, m_skipBtn, nullptr);
	rouletteBtnMenu->setContentSize({ m_mainMenu->getContentWidth(), 40.0f });
	rouletteBtnMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Between)
		->setMainAxisScaling(AxisScaling::None)
		->setCrossAxisScaling(AxisScaling::None)
		->setGap(10.0f)
	);
	m_mainMenu->addChild(rouletteBtnMenu);


	m_levelListNode = CCNode::create();
	m_levelListNode->setContentSize(
		m_mainMenu->getContentSize() - ccp( 0.0f, m_closeBtn->getContentHeight() + rouletteBtnMenu->getContentHeight() + 10.0f )
	);
	m_mainMenu->addChild(m_levelListNode);

	m_levelListBG = NineSlice::create("square01_001.png");
	m_levelListBG->setContentSize(m_levelListNode->getContentSize());
	m_levelListNode->addChildAtPosition(
		m_levelListBG, Anchor::Center, {}
	);

	m_levelScroll = ScrollLayer::create(m_levelListNode->getContentSize() - ccp( 10.0f, 10.0f ));
	auto content = m_levelScroll->m_contentLayer;

	content->setLayout(
		ScrollLayer::createDefaultListLayout()
	);
	m_levelListNode->addChildAtPosition(
		m_levelScroll, Anchor::Center, {}
	);

	m_mainMenu->updateLayout();

    return true;
}