#include "./rouletteLayer.hpp"

RouletteLayer* RouletteLayer::create(float deltaAngle) {
	auto ret = new RouletteLayer();
	if (ret && ret->init(deltaAngle)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteLayer::init(float deltaAngle) {
	if (!Popup::init(525.0f, 280.0f)) return false;

	m_bgSprite->setVisible(false);
	m_closeBtn->setVisible(false);

	m_rouletteWheel = CCMenu::create();
	m_rouletteWheel->setContentSize({ 0.0f, 0.0f });
	m_rouletteWheel->setPosition({ m_size.width / 2.0f, 140.0f });

	auto wheelStroke1 = CCSprite::create("wheelStroke.png"_spr);
	wheelStroke1->setScale(0.7f);
	auto wheelStroke2 = CCSprite::create("wheelStroke.png"_spr);
	wheelStroke2->setColor({ 0, 0, 0 });
	wheelStroke2->setScale(0.68f);

	auto wheelLine1 = CCSprite::createWithSpriteFrameName("gridLine01_001.png");
	wheelLine1->setColor({ 0, 0, 0 });
	wheelLine1->setScaleX(7.0f);
	wheelLine1->setScaleY(5.0f);

	auto wheelLine2 = CCSprite::createWithSpriteFrameName("gridLine01_001.png");
	wheelLine2->setRotation(90.0f);
	wheelLine2->setColor({ 0, 0, 0 });
	wheelLine2->setScaleX(7.0f);
	wheelLine2->setScaleY(5.0f);

	for (int i = 0; i < 4; i++) {
		auto quadrant = CCSprite::createWithSpriteFrameName("d_scaleFadeRing_02_001.png");
		quadrant->setAnchorPoint({ 1.0f, 0.0f });
		quadrant->setScale(3.65f);
		quadrant->setRotation(180.0f - 90.0f * i);
		quadrant->setColor({ 255, 0, 0 });
		m_quadrants.push_back(quadrant);
		m_rouletteWheel->addChild(quadrant);
	}

	for (int i = 0; i < 4; i++) {
		auto levelName = CCLabelBMFont::create("", "bigFont.fnt");
		levelName->setRotation(315.0f - 90.0f * i);
		levelName->setScale(0.5f);
		levelName->setPosition({ i > 1 ? -50.0f : 50.0f, i == 0 || i == 3 ? -50.0f : 50.0f });
		m_levelNames.push_back(levelName);
		m_rouletteWheel->addChild(levelName);
	}

	auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
	arrow->setPosition({ 262.5f, 35.0f });
	arrow->setRotation(90.0f);
	arrow->setFlipY(true);
	arrow->setScaleX(0.8f);
	arrow->setScaleY(0.6f);
	arrow->setZOrder(1);

	m_rouletteWheel->addChild(wheelStroke1);
	m_rouletteWheel->addChild(wheelStroke2);
	m_rouletteWheel->addChild(wheelLine1);
	m_rouletteWheel->addChild(wheelLine2);

	m_mainLayer->addChild(m_rouletteWheel);
	m_mainLayer->addChild(arrow);

	spin(deltaAngle);

	m_afterSpinListener = AfterSpinEvent().listen(
		[this]() {
			afterSpinOnLayer();
		}
	);

	return true;
}

void RouletteLayer::spin(float deltaAngle) {
	static std::mt19937 mt(std::random_device{}());
	auto& selectedLevels = Globals::getSelectedLevelsMutable();
	auto& levelsData = Globals::getLevelData();

	if (!levelsData.empty()) {
		for (int i = 0; i < 4; i++) {
			auto diffColors = Globals::getDifficultyColors();
			auto spriteNames = Globals::getSpriteNames();
			LevelData level = levelsData[std::to_string(selectedLevels[i])];
			
			m_quadrants[i]->setColor(diffColors[level.diff]);
			m_levelNames[i]->setString(level.name.c_str());
			if (level.name.size() > 12) m_levelNames[i]->setScale(0.35f);

			auto diffSpr = CCSprite::createWithSpriteFrameName(spriteNames[level.diff].c_str());
			diffSpr->setRotation(315.0f - 90.0f * i);
			diffSpr->setPosition({ i > 1 ? -30.0f : 30.0f, i == 0 || i == 3 ? -30.0f : 30.0f });
			m_rouletteWheel->addChild(diffSpr);
		}
		m_rouletteWheel->updateLayout();
	}

	auto rotate = CCRotateBy::create(4.0f, 6.0f * 360.0f + deltaAngle);
	auto ease = SmoothExponentialOut::create(rotate);

	auto callback = CallFuncExt::create([this]() { AfterSpinEvent().send(); });
	auto seq = CCSequence::create(ease, callback, nullptr);
	m_rouletteWheel->runAction(seq);
}

void RouletteLayer::afterSpinOnLayer() {
	auto exitBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_achImage_001.png", 1.0f, [this](auto) {
		onClose(nullptr);
	});
	exitBtn->setPosition({ 262.5f, 140.0f });
	m_buttonMenu->addChild(exitBtn);
}

void RouletteLayer::keyDown(enumKeyCodes key, double d) {
	switch (key) {
	case KEY_Escape:
		break;
	default:
		Popup::keyDown(key, d);
		break;
	}
}