#include "./roulette.hpp"

RouletteLayer* RouletteLayer::create(RoulettePopup* popup) {
	auto ret = new RouletteLayer();
	if (ret && ret->init(popup)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteLayer::init(RoulettePopup* popup) {
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

	readLevelData();
	spin(popup);

	g_lvls.clear();

	return true;
}

void RouletteLayer::readLevelData() {
	if (!g_levelData.empty()) return;
	std::unordered_map<std::string, LevelData> levelData;

	auto data = geode::utils::file::readJson(Mod::get()->getResourcesDir() / "levelData.json");
	if (data.isOk()) {
		matjson::Value level = data.ok().value();
		for (auto [key, value] : level) {
			LevelData data;
			int levelID = numFromString<int>(key, 10).unwrapOrDefault();
			if (levelID == 0) continue;

			data.likes = value["likes"].asInt().ok().value();
			data.name = value["name"].asString().ok().value();
			data.diff = value["diff"].asString().ok().value();
			data.levelID = levelID;

			levelData[key] = data;
			g_levels.push_back(levelID);
		}

		g_levelData = levelData;
	}
	else log::info("levelData can't read.");
}

void RouletteLayer::spin(RoulettePopup* popup) {
	static std::mt19937 mt(std::random_device{}());

	if (g_levels.size() >= 4) {
		for (int i = 0; i < 4; i++) {
			if (g_lvls.size() != 4) g_lvls.push_back(g_levels[std::uniform_int_distribution<int>(0, g_levels.size() - 1)(mt)]);
		}
	}
	else {
		g_levels.insert(g_levels.end(), g_usedLevels.begin(), g_usedLevels.end());
		g_usedLevels.clear();
		FLAlertLayer::create("Whoops!", "The levels are out! The list has been updated.", "Ok")->show();
		return;
	}
	
	float deltaAngle = std::uniform_real_distribution<float>(0.0f, 360.0f)(mt);

	int levelIndex = 0;
	for (int i = 0; i < 4; i++) {
		if (deltaAngle - (90.0f * i) < 90.0f) {
			g_pastCurrentLvl = g_levelData[std::to_string(g_lvls[i])];

			g_levels.erase(std::find(g_levels.begin(), g_levels.end(), g_lvls[i]));
			g_usedLevels.push_back(g_lvls[i]);

			break;
		}
	}

	if (!g_levelData.empty()) {
		for (int i = 0; i < 4; i++) {
			LevelData level = g_levelData[std::to_string(g_lvls[i])];
			
			m_quadrants[i]->setColor(g_quadrantColors[level.diff]);
			m_levelNames[i]->setString(level.name.c_str());
			if (level.name.size() > 12) m_levelNames[i]->setScale(0.35f);

			auto diffSpr = CCSprite::createWithSpriteFrameName(g_spriteNames[level.diff].c_str());
			diffSpr->setRotation(315.0f - 90.0f * i);
			diffSpr->setPosition({ i > 1 ? -30.0f : 30.0f, i == 0 || i == 3 ? -30.0f : 30.0f });
			m_rouletteWheel->addChild(diffSpr);
		}
		m_rouletteWheel->updateLayout();
	}

	auto rotate = CCRotateBy::create(4.0f, 6.0f * 360.0f + deltaAngle);
	auto ease = SmoothExponentialOut::create(rotate);
	auto callback1 = CCCallFunc::create(this, callfunc_selector(RouletteLayer::afterSpinOnLayer));
	auto callback2 = CCCallFunc::create(popup, callfunc_selector(RoulettePopup::afterSpinOnPopup));
	auto seq = CCSequence::create(ease, callback1, callback2, nullptr);
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