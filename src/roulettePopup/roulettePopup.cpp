#include "./roulettePopup.hpp"
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace geode::prelude {
	std::vector<int> g_worstLevels;
	std::vector<int> g_usedWorstLevels;
	std::vector<int> g_lvls;
	LevelData g_currentLvl;
	LevelData g_pastCurrentLvl;
	std::unordered_map<std::string, LevelData> g_levelData;
	std::unordered_map<std::string, ccColor3B> g_quadrantColors = {
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
	std::unordered_map<std::string, std::string> g_spriteNames = {
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

	std::unordered_map<std::string, std::string> g_spriteNames1 = {
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

	int g_spinsCount = 0;
	int g_skipsCount = Mod::get()->getSettingValue<int>("skips-count");;
	int g_requirePercent = 1;
	int g_currentPercent = 0;
	bool g_afterSpin = true;

	RoulettePopup* g_popup = nullptr;
}

RoulettePopup* RoulettePopup::create() {
	auto ret = new RoulettePopup();
	if (ret && ret->initAnchored(320.0f, 240.0f)) {
		ret->autorelease();
		g_popup = ret;
		return ret;
	}
	delete ret;
	return nullptr;
}

RouletteLayer* RouletteLayer::create() {
	auto ret = new RouletteLayer();
	if (ret && ret->initAnchored(525.0f, 280.0f)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool RoulettePopup::setup() {
	this->setID("worst-demons-menu");
	this->setZOrder(100);
	this->setTitle("Try your unluck right now");

	auto bg = CCScale9Sprite::create("square02b_001.png");
	bg->setColor({ 0, 0, 0 });
	bg->setOpacity(50);
	bg->setContentSize({ 280.0f, 170.0f });
	bg->setPosition({ 160.0f, 105.0f });
	m_mainLayer->addChild(bg);

	m_closeBtn->setVisible(false);
	auto closeBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_closeBtn_001.png", 0.8f, [this](auto) {
		onClose(nullptr);
		g_popup = nullptr;
	});
	closeBtn->setPosition(m_closeBtn->getPosition());
	m_buttonMenu->addChild(closeBtn);

	m_spinsCount = CCLabelBMFont::create(fmt::format("Number of spins: {}", g_spinsCount).c_str(), "bigFont.fnt");
	m_spinsCount->setScale(0.4f);
	m_spinsCount->setPosition({ 160.0f, 202.0f });
	m_mainLayer->addChild(m_spinsCount);

	auto spinSpr = ButtonSprite::create("Start");
	m_spinBtn = CCMenuItemExt::createSpriteExtra(spinSpr, [this](auto) {
		if (!g_currentLvl.id.empty()) {
			g_spinsCount -= 1;
			afterSpinOnPopup();
		}
		else {
			RouletteLayer::create()->show();
		}
	});
	m_spinBtn->setPosition({ 160.0f, 110.0f });
	m_spinBtn->setScale(0.8f);
	m_spinBtn->m_baseScale = 0.8f;
	m_buttonMenu->addChild(m_spinBtn);

	return true;
}

bool RouletteLayer::setup() {
	auto winSize = CCDirector::get()->getWinSize();
	m_bgSprite->setVisible(false);
	m_closeBtn->setVisible(false);

	m_rouletteWheel = CCMenu::create();
	m_rouletteWheel->setContentSize({ 0.0f, 0.0f });
	m_rouletteWheel->setPosition({ 262.5f, 140.0f });

	m_quadrant1 = CCSprite::createWithSpriteFrameName("d_scaleFadeRing_02_001.png");
	m_quadrant1->setAnchorPoint({ 1.0f, 0.0f });
	m_quadrant1->setScale(3.65f);
	m_quadrant1->setRotation(180.0f);
	m_quadrant1->setColor({ 255, 0, 0 });
	m_quadrant1->setID("quadrant-1"_spr);

	m_quadrant2 = CCSprite::createWithSpriteFrameName("d_scaleFadeRing_02_001.png");
	m_quadrant2->setAnchorPoint({ 1.0f, 0.0f });
	m_quadrant2->setScale(3.65f);
	m_quadrant2->setRotation(90.0f);
	m_quadrant2->setColor({ 0, 255, 0 });
	m_quadrant2->setID("quadrant-2"_spr);

	m_quadrant3 = CCSprite::createWithSpriteFrameName("d_scaleFadeRing_02_001.png");
	m_quadrant3->setAnchorPoint({ 1.0f, 0.0f });
	m_quadrant3->setScale(3.65f);
	m_quadrant3->setColor({ 0, 0, 255 });
	m_quadrant3->setID("quadrant-3"_spr);

	m_quadrant4 = CCSprite::createWithSpriteFrameName("d_scaleFadeRing_02_001.png");
	m_quadrant4->setAnchorPoint({ 1.0f, 0.0f });
	m_quadrant4->setScale(3.65f);
	m_quadrant4->setRotation(270.0f);
	m_quadrant4->setID("quadrant-4"_spr);

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

	m_levelName1 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelName1->setRotation(315.0f);
	m_levelName1->setScale(0.5f);
	m_levelName1->setPosition({ 50.0f, -50.0f });

	m_levelName2 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelName2->setRotation(225.0f);
	m_levelName2->setScale(0.5f);
	m_levelName2->setPosition({ 50.0f, 50.0f });

	m_levelName3 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelName3->setRotation(135.0f);
	m_levelName3->setScale(0.5f);
	m_levelName3->setPosition({ -50.0f, 50.0f });

	m_levelName4 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelName4->setRotation(45.0f);
	m_levelName4->setScale(0.5f);
	m_levelName4->setPosition({ -50.0f, -50.0f });

	m_levelDislikes1 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelDislikes1->setRotation(315.0f);
	m_levelDislikes1->setScale(0.4f);
	m_levelDislikes1->setPosition({ 65.875f, -54.125f });

	m_levelDislikes2 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelDislikes2->setRotation(225.0f);
	m_levelDislikes2->setScale(0.4f);
	m_levelDislikes2->setPosition({ 54.125f, 65.875f });

	m_levelDislikes3 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelDislikes3->setRotation(135.0f);
	m_levelDislikes3->setScale(0.4f);
	m_levelDislikes3->setPosition({ -65.875f, 54.125f });

	m_levelDislikes4 = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelDislikes4->setRotation(45.0f);
	m_levelDislikes4->setScale(0.4f);
	m_levelDislikes4->setPosition({ -54.125f, -65.875f });

	m_dislikeSpr1 = CCSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
	m_dislikeSpr1->setScale(0.5f);
	m_dislikeSpr1->setRotation(315.0f);
	m_dislikeSpr1->setPosition({ 60.0f, -60.0f });

	m_dislikeSpr2 = CCSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
	m_dislikeSpr2->setScale(0.5f);
	m_dislikeSpr2->setRotation(225.0f);
	m_dislikeSpr2->setPosition({ 60.0f, 60.0f });

	m_dislikeSpr3 = CCSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
	m_dislikeSpr3->setScale(0.5f);
	m_dislikeSpr3->setRotation(135.0f);
	m_dislikeSpr3->setPosition({ -60.0f, 60.0f });

	m_dislikeSpr4 = CCSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
	m_dislikeSpr4->setScale(0.5f);
	m_dislikeSpr4->setRotation(45.0f);
	m_dislikeSpr4->setPosition({ -60.0f, -60.0f });

	auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
	arrow->setPosition({ 262.5f, 35.0f });
	arrow->setRotation(90.0f);
	arrow->setFlipY(true);
	arrow->setScaleX(0.8f);
	arrow->setScaleY(0.6f);
	arrow->setZOrder(1);

	m_rouletteWheel->addChild(m_quadrant1);
	m_rouletteWheel->addChild(m_quadrant2);
	m_rouletteWheel->addChild(m_quadrant3);
	m_rouletteWheel->addChild(m_quadrant4);
	m_rouletteWheel->addChild(wheelStroke1);
	m_rouletteWheel->addChild(wheelStroke2);
	m_rouletteWheel->addChild(wheelLine1);
	m_rouletteWheel->addChild(wheelLine2);
	m_rouletteWheel->addChild(m_levelName1);
	m_rouletteWheel->addChild(m_levelName2);
	m_rouletteWheel->addChild(m_levelName3);
	m_rouletteWheel->addChild(m_levelName4);
	m_rouletteWheel->addChild(m_levelDislikes1);
	m_rouletteWheel->addChild(m_levelDislikes2);
	m_rouletteWheel->addChild(m_levelDislikes3);
	m_rouletteWheel->addChild(m_levelDislikes4);
	m_rouletteWheel->addChild(m_dislikeSpr1);
	m_rouletteWheel->addChild(m_dislikeSpr2);
	m_rouletteWheel->addChild(m_dislikeSpr3);
	m_rouletteWheel->addChild(m_dislikeSpr4);

	m_mainLayer->addChild(m_rouletteWheel);
	m_mainLayer->addChild(arrow);

	readLevelData();
	spin();

	g_lvls.clear();

	return true;
}

void RouletteLayer::readLevelData() {
	if (!g_levelData.empty()) return;
	std::unordered_map<std::string, LevelData> worstLevelData;

	auto data = geode::utils::file::readJson(Mod::get()->getResourcesDir() / "levelData.json");
	if (data.isOk()) {
		matjson::Value level = data.ok().value();
		for (auto [key, value] : level) {
			LevelData data;

			data.likes = value["likes"].asInt().ok().value();
			data.name = value["name"].asString().ok().value();
			data.diff = value["diff"].asString().ok().value();
			data.id = key;

			worstLevelData[key] = data;
			g_worstLevels.push_back(std::stoi(key));
		}

		g_levelData = worstLevelData;
	}
	else {
		log::info("levelData can't read.");
	}
}

void RouletteLayer::spin() {
	static std::mt19937 mt(std::random_device{}());

	if (g_worstLevels.size() >= 4) {
		for (int i = 0; i < 4; i++) {
			if (g_lvls.size() != 4) g_lvls.push_back(g_worstLevels[std::uniform_int_distribution<int>(0, g_worstLevels.size() - 1)(mt)]);
		}
	}
	else {
		g_worstLevels.insert(g_worstLevels.end(), g_usedWorstLevels.begin(), g_usedWorstLevels.end());
		g_usedWorstLevels.clear();
		FLAlertLayer::create("Whoops!", "The levels are out! The list has been updated.", "Ok")->show();
		return;
	}
	
	float deltaAngle = std::uniform_real_distribution<float>(0.0f, 360.0f)(mt);

	int levelIndex = 0;
	for (int i = 0; i < 4; i++) {
		if (deltaAngle - (90.0f * i) < 90.0f) {
			g_pastCurrentLvl = g_levelData[std::to_string(g_lvls[i])];

			g_worstLevels.erase(std::find(g_worstLevels.begin(), g_worstLevels.end(), g_lvls[i]));
			g_usedWorstLevels.push_back(g_lvls[i]);

			break;
		}
	}

	if (!g_levelData.empty()) {
		for (int i = 0; i < 4; i++) {
			LevelData level = g_levelData[std::to_string(g_lvls[i])];
			if (i == 0) {
				m_quadrant1->setColor(g_quadrantColors[level.diff]);
				m_levelName1->setString(level.name.c_str());
				if (level.name.size() > 12) m_levelName1->setScale(0.35f);
				m_levelDislikes1->setString(std::to_string(level.likes).c_str());

				m_diffSpr1 = CCSprite::createWithSpriteFrameName(g_spriteNames[level.diff].c_str());
				m_diffSpr1->setRotation(315.0f);
				m_diffSpr1->setPosition({ 30.0f, -30.0f });
				m_rouletteWheel->addChild(m_diffSpr1);

				auto posX = m_levelDislikes1->getPositionX();
				auto posY = m_levelDislikes1->getPositionY();
				auto contW = m_levelDislikes1->getContentWidth();
				auto scale = m_levelDislikes1->getScale();
				auto dContW = m_dislikeSpr1->getContentWidth() * 0.5f * 0.7f;
				m_dislikeSpr1->setPosition({ posX - (contW * scale / 2.0f) * 0.7f - dContW, posY - (contW * scale / 2.0f) * 0.7f - dContW });
			}
			else if (i == 1) {
				m_quadrant2->setColor(g_quadrantColors[level.diff]);
				m_levelName2->setString(level.name.c_str());
				if (level.name.size() > 12) m_levelName2->setScale(0.35f);
				m_levelDislikes2->setString(std::to_string(level.likes).c_str());

				m_diffSpr2 = CCSprite::createWithSpriteFrameName(g_spriteNames[level.diff].c_str());
				m_diffSpr2->setRotation(225.0f);
				m_diffSpr2->setPosition({ 30.0f, 30.0f });
				m_rouletteWheel->addChild(m_diffSpr2);

				auto posX = m_levelDislikes2->getPositionX();
				auto posY = m_levelDislikes2->getPositionY();
				auto contW = m_levelDislikes2->getContentWidth();
				auto scale = m_levelDislikes2->getScale();
				auto dContW = m_dislikeSpr2->getContentWidth() * 0.5f * 0.7f;
				m_dislikeSpr2->setPosition({ posX + (contW * scale / 2.0f) * 0.7f + dContW, posY - (contW * scale / 2.0f) * 0.7f - dContW });
			}
			else if (i == 2) {
				m_quadrant3->setColor(g_quadrantColors[level.diff]);
				m_levelName3->setString(level.name.c_str());
				if (level.name.size() > 12) m_levelName3->setScale(0.35f);
				m_levelDislikes3->setString(std::to_string(level.likes).c_str());

				m_diffSpr3 = CCSprite::createWithSpriteFrameName(g_spriteNames[level.diff].c_str());
				m_diffSpr3->setRotation(135.0f);
				m_diffSpr3->setPosition({ -30.0f, 30.0f });
				m_rouletteWheel->addChild(m_diffSpr3);

				auto posX = m_levelDislikes3->getPositionX();
				auto posY = m_levelDislikes3->getPositionY();
				auto contW = m_levelDislikes3->getContentWidth();
				auto scale = m_levelDislikes3->getScale();
				auto dContW = m_dislikeSpr3->getContentWidth() * 0.5f * 0.7f;
				m_dislikeSpr3->setPosition({ posX + (contW * scale / 2.0f) * 0.7f + dContW, posY + (contW * scale / 2.0f) * 0.7f + dContW });
			}
			else {
				m_quadrant4->setColor(g_quadrantColors[level.diff]);
				m_levelName4->setString(level.name.c_str());
				if (level.name.size() > 12) m_levelName4->setScale(0.35f);
				m_levelDislikes4->setString(std::to_string(level.likes).c_str());

				m_diffSpr4 = CCSprite::createWithSpriteFrameName(g_spriteNames[level.diff].c_str());
				m_diffSpr4->setRotation(45.0f);
				m_diffSpr4->setPosition({ -30.0f, -30.0f });
				m_rouletteWheel->addChild(m_diffSpr4);

				auto posX = m_levelDislikes4->getPositionX();
				auto posY = m_levelDislikes4->getPositionY();
				auto contW = m_levelDislikes4->getContentWidth();
				auto scale = m_levelDislikes4->getScale();
				auto dContW = m_dislikeSpr4->getContentWidth() * 0.5f * 0.7f;
				m_dislikeSpr4->setPosition({ posX - (contW * scale / 2.0f) * 0.7f - dContW, posY + (contW * scale / 2.0f) * 0.7f + dContW });
			}
		}
		m_rouletteWheel->updateLayout();
	}

	auto rotate = CCRotateBy::create(4.0f, 6.0f * 360.0f + deltaAngle);
	auto ease = SmoothExponentialOut::create(rotate);
	auto callback1 = CCCallFunc::create(this, callfunc_selector(RouletteLayer::afterSpinOnLayer));
	auto callback2 = CCCallFunc::create(g_popup, callfunc_selector(RoulettePopup::afterSpinOnPopup));
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

void RoulettePopup::afterSpinOnPopup() {
	g_spinsCount += 1;
	g_currentLvl = g_pastCurrentLvl;
	if (g_spinsCount == 1) {
		m_spinBtn->setVisible(false);
		m_spinsCount->setString("Number of spins: 1");

		m_resetBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_updateBtn_001.png", 1.0f, [this](auto) {
			resetRoulette();
		});
		m_resetBtn->setPosition({ 320.0f, 240.0f });
		m_resetBtn->setScale(0.8f);
		m_resetBtn->m_baseScale = m_resetBtn->getScale();

		m_diffSpr = CCSprite::createWithSpriteFrameName(g_spriteNames1[g_currentLvl.diff].c_str());
		m_diffSpr->setPosition({ 75.0f, 105.0f });
		m_diffSpr->setZOrder(1);

		m_levelName = CCLabelBMFont::create(g_currentLvl.name.c_str(), "bigFont.fnt");
		if (g_currentLvl.name.size() > 12) m_levelName->setScale(0.5f);
		else m_levelName->setScale(0.7f);
		m_levelName->setPosition({ 160.0f, 170.0f });
		m_levelName->setZOrder(1);

		m_dislikeSpr = CCSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
		m_dislikeSpr->setPosition({ 224.75f + m_dislikeSpr->getContentWidth() / 2.0f, 105.0f });
		m_dislikeSpr->setScale(0.75f);
		m_dislikeSpr->setZOrder(1);

		m_levelDislikes = CCLabelBMFont::create(std::to_string(g_currentLvl.likes).c_str(), "bigFont.fnt");
		m_levelDislikes->setScale(0.35f);
		m_levelDislikes->setPosition({ m_dislikeSpr->getPositionX() + m_dislikeSpr->getContentWidth() / 2.0f + m_levelDislikes->getContentWidth() * m_levelDislikes->getScale() / 2.0f, 105.0f });
		m_levelDislikes->setZOrder(1);

		m_playBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_playBtn2_001.png", 0.8f, [this](auto) {
			auto searchObj = GJSearchObject::create(SearchType::Search, g_currentLvl.id);

			auto lbl = LevelBrowserLayer::scene(searchObj);
			CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, lbl));
			});
		m_playBtn->setPosition({ 160.0f, 110.0f });

		m_requirePercentBG = CCScale9Sprite::create("square02b_001.png");
		m_requirePercentBG->setColor({ 0, 0, 0 });
		m_requirePercentBG->setOpacity(50);
		m_requirePercentBG->setContentSize({ 90.0f, 60.0f });
		m_requirePercentBG->setScale(0.4f);
		m_requirePercentBG->setPosition({ 160.0f, 40.0f });
		m_requirePercentBG->setZOrder(1);

		m_requirePercent = CCLabelBMFont::create(fmt::format("{}%", g_requirePercent).c_str(), "bigFont.fnt");
		m_requirePercent->setScale(0.4f);
		m_requirePercent->setPosition({ 160.0f, 40.0f });
		m_requirePercent->setZOrder(1);

		auto skipSpr = ButtonSprite::create("Skip", "bigFont.fnt", "GJ_button_03.png");
		m_skipBtn = CCMenuItemExt::createSpriteExtra(skipSpr, [this](auto) {
			if (g_requirePercent == 100) {
				FLAlertLayer::create("Congrats", "You passed the roulette. =3", "Hurray!")->show();
			}
			else if (g_skipsCount == 0) {
				FLAlertLayer::create("Nope", "You spent all your skips. =3", "Ok")->show();
			}
			else {
				g_skipsCount -= 1;
				g_requirePercent += 1;
				RouletteLayer::create()->show();
			}
		});
		m_skipBtn->setScale(0.65f);
		m_skipBtn->m_baseScale = m_skipBtn->getScale();
		m_skipBtn->setPosition({ 105.0f, 40.0f });

		m_skipsCount = CCLabelBMFont::create(fmt::format("skips: {}", g_skipsCount).c_str(), "goldFont.fnt");
		m_skipsCount->setScale(0.4f);
		m_skipsCount->setPosition({ 160.0f, 64.0f });
		m_skipsCount->setZOrder(1);

		auto nextSpr = ButtonSprite::create("Next", "bigFont.fnt", "GJ_button_01.png");
		m_nextBtn = CCMenuItemExt::createSpriteExtra(nextSpr, [this](auto) {
			if (g_requirePercent > 100) {
				FLAlertLayer::create("Congrats", "You passed the roulette. =3", "Hurray!")->show();
			}
			else if (g_currentPercent < g_requirePercent) {
				FLAlertLayer::create("Nope", "You haven't reached the required percentage. =3", "Ok")->show();
			}
			else {
				g_requirePercent += 1;
				RouletteLayer::create()->show();
			}
		});
		m_nextBtn->setScale(0.65f);
		m_nextBtn->m_baseScale = m_nextBtn->getScale();
		m_nextBtn->setPosition({ 215.0f, 40.0f });

		m_mainLayer->addChild(m_diffSpr);
		m_mainLayer->addChild(m_dislikeSpr);
		m_mainLayer->addChild(m_levelDislikes);
		m_mainLayer->addChild(m_levelName);
		m_mainLayer->addChild(m_requirePercentBG);
		m_mainLayer->addChild(m_requirePercent);
		m_mainLayer->addChild(m_skipsCount);
		m_buttonMenu->addChild(m_playBtn);
		m_buttonMenu->addChild(m_skipBtn);
		m_buttonMenu->addChild(m_nextBtn);
		m_buttonMenu->addChild(m_resetBtn);
	}
	else {
		m_spinBtn->setVisible(false);
		g_currentPercent = 0;
		m_spinsCount->setString(fmt::format("Number of spins: {}", g_spinsCount).c_str());

		if (!m_resetBtn) {
			m_resetBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_updateBtn_001.png", 1.0f, [this](auto) {
				resetRoulette();
				});
			m_resetBtn->setPosition({ 320.0f, 240.0f });
			m_resetBtn->setScale(0.8f);
			m_resetBtn->m_baseScale = m_resetBtn->getScale();
		}

		if (m_diffSpr) m_mainLayer->removeChild(m_diffSpr);
		m_diffSpr = CCSprite::createWithSpriteFrameName(g_spriteNames1[g_currentLvl.diff].c_str());
		m_diffSpr->setPosition({ 75.0f, 105.0f });
		m_diffSpr->setZOrder(1);

		if (!m_levelName) {
			m_levelName = CCLabelBMFont::create("", "bigFont.fnt");
			if (g_currentLvl.name.size() > 12) m_levelName->setScale(0.5f);
			else m_levelName->setScale(0.7f);
			m_levelName->setPosition({ 160.0f, 170.0f });
		}
		m_levelName->setString(g_currentLvl.name.c_str());

		if (!m_dislikeSpr) {
			m_dislikeSpr = CCSprite::createWithSpriteFrameName("GJ_dislikesIcon_001.png");
			m_dislikeSpr->setPosition({ 224.75f + m_dislikeSpr->getContentWidth() / 2.0f, 105.0f });
			m_dislikeSpr->setScale(0.75f);
		}
		if (!m_levelDislikes) {
			m_levelDislikes = CCLabelBMFont::create("", "bigFont.fnt");
			m_levelDislikes->setScale(0.35f);
		}
		m_levelDislikes->setString(std::to_string(g_currentLvl.likes).c_str());
		m_levelDislikes->setPosition({ m_dislikeSpr->getPositionX() + m_dislikeSpr->getContentWidth() / 2.0f + m_levelDislikes->getContentWidth() * m_levelDislikes->getScale() / 2.0f, 105.0f });

		if (!m_playBtn) {
			m_playBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_playBtn2_001.png", 0.8f, [this](auto) {
				auto searchObj = GJSearchObject::create(SearchType::Search, g_currentLvl.id);
				log::info("suuus");

				auto lbl = LevelBrowserLayer::scene(searchObj);
				auto transitionFade = CCTransitionFade::create(0.5, lbl);
				CCDirector::sharedDirector()->pushScene(transitionFade);
			});
			m_playBtn->setPosition({ 160.0f, 110.0f });
		}

		if (!m_requirePercent) {
			m_requirePercent = CCLabelBMFont::create("", "bigFont.fnt");
			m_requirePercent->setScale(0.4f);
			m_requirePercent->setPosition({ 160.0f, 40.0f });
		}
		if (!m_requirePercentBG) {
			m_requirePercentBG = CCScale9Sprite::create("square02b_001.png");
			m_requirePercentBG->setColor({ 0, 0, 0 });
			m_requirePercentBG->setOpacity(50);
			m_requirePercentBG->setContentSize({ 90.0f, 60.0f });
			m_requirePercentBG->setScale(0.4f);
			m_requirePercentBG->setPosition({ 160.0f, 40.0f });
		}
		m_requirePercent->setString(fmt::format("{}%", g_requirePercent).c_str());

		if (!m_skipsCount) {
			m_skipsCount = CCLabelBMFont::create("", "goldFont.fnt");
			m_skipsCount->setScale(0.4f);
			m_skipsCount->setPosition({ 160.0f, 64.0f });
		}
		m_skipsCount->setString(fmt::format("skips: {}", g_skipsCount).c_str());

		if (!m_skipBtn) {
			auto skipSpr = ButtonSprite::create("Skip", "bigFont.fnt", "GJ_button_03.png");
			m_skipBtn = CCMenuItemExt::createSpriteExtra(skipSpr, [this](auto) {
				if (g_requirePercent == 100) {
					FLAlertLayer::create("Congrats", "You passed the roulette. =3", "Hurray!")->show();
				}
				else if (g_skipsCount == 0) {
					FLAlertLayer::create("Nope", "You spent all your skips. =3", "Ok")->show();
				}
				else {
					g_skipsCount -= 1;
					g_requirePercent += 1;
					RouletteLayer::create()->show();
				}
				});
			m_skipBtn->setScale(0.65f);
			m_skipBtn->m_baseScale = m_skipBtn->getScale();
			m_skipBtn->setPosition({ 105.0f, 40.0f });
		}

		if (!m_nextBtn) {
			auto nextSpr = ButtonSprite::create("Next", "bigFont.fnt", "GJ_button_01.png");
			m_nextBtn = CCMenuItemExt::createSpriteExtra(nextSpr, [this](auto) {
				if (g_requirePercent == 100) {
					FLAlertLayer::create("Congrats", "You passed the roulette. =3", "Hurray!")->show();
				}
				else if (g_currentPercent < g_requirePercent) {
					FLAlertLayer::create("Nope", "You haven't reached the required percentage. =3", "Ok")->show();
				}
				else {
					g_requirePercent += 1;
					RouletteLayer::create()->show();
				}
				});
			m_nextBtn->setScale(0.65f);
			m_nextBtn->m_baseScale = m_nextBtn->getScale();
			m_nextBtn->setPosition({ 215.0f, 40.0f });
		}

		if (g_currentLvl.name.size() > 12) m_levelName->setScale(0.5f);
		else m_levelName->setScale(0.7f);

		m_mainLayer->addChild(m_diffSpr);
		if (!m_levelName->getParent()) m_mainLayer->addChild(m_levelName);
		if (!m_levelDislikes->getParent()) m_mainLayer->addChild(m_levelDislikes);
		if (!m_dislikeSpr->getParent()) m_mainLayer->addChild(m_dislikeSpr);
		if (!m_requirePercent->getParent()) {
			m_mainLayer->addChild(m_requirePercentBG);
			m_mainLayer->addChild(m_requirePercent);
		}
		if (!m_skipsCount->getParent()) m_mainLayer->addChild(m_skipsCount);
		if (!m_playBtn->getParent()) m_buttonMenu->addChild(m_playBtn);
		if (!m_skipBtn->getParent()) m_buttonMenu->addChild(m_skipBtn);
		if (!m_nextBtn->getParent()) m_buttonMenu->addChild(m_nextBtn);
		if (!m_resetBtn->getParent()) m_buttonMenu->addChild(m_resetBtn);
	}
}

void RoulettePopup::resetRoulette() {
	createQuickPopup("Confirm reset", "Are you sure you want to reset the roulette?", "No", "Yes", [this](auto, bool yesBtn) {
		if (yesBtn) {
			g_worstLevels.insert(g_worstLevels.end(), g_usedWorstLevels.begin(), g_usedWorstLevels.end());
			g_usedWorstLevels.clear();
			g_lvls.clear();
			g_currentLvl = LevelData();

			g_spinsCount = 0;
			g_skipsCount = Mod::get()->getSettingValue<int>("skips-count");;
			g_requirePercent = 1;
			g_currentPercent = 0;

			m_spinBtn->setVisible(true);
			m_spinsCount->setString("Number of spins: 0");

			if (m_levelName) m_mainLayer->removeChild(m_levelName);
			if (m_diffSpr) m_mainLayer->removeChild(m_diffSpr);
			if (m_levelDislikes) m_mainLayer->removeChild(m_levelDislikes);
			if (m_dislikeSpr) m_mainLayer->removeChild(m_dislikeSpr);
			if (m_skipsCount) m_mainLayer->removeChild(m_skipsCount);
			if (m_requirePercent) m_mainLayer->removeChild(m_requirePercent);
			if (m_requirePercentBG) m_mainLayer->removeChild(m_requirePercentBG);
			if (m_playBtn) m_buttonMenu->removeChild(m_playBtn, true);
			if (m_nextBtn) m_buttonMenu->removeChild(m_nextBtn, true);
			if (m_skipBtn) m_buttonMenu->removeChild(m_skipBtn, true);
			if (m_resetBtn) m_buttonMenu->removeChild(m_resetBtn, true);
		}
	});
}

void RoulettePopup::keyDown(enumKeyCodes key) {
	switch (key) {
	case KEY_Escape:
		onClose(nullptr);
		g_popup = nullptr;

		break;
	default:
		Popup::keyDown(key);
		break;
	}
}

void RouletteLayer::keyDown(enumKeyCodes key) {
	switch (key) {
	case KEY_Escape:
		break;
	default:
		Popup::keyDown(key);
		break;
	}
}

class $modify(PlayLayer) {
	void destroyPlayer(PlayerObject* player, GameObject* object) {
		PlayLayer::destroyPlayer(player, object);

		if (std::to_string(m_level->m_levelID) == g_currentLvl.id && PlayLayer::getCurrentPercentInt() > g_currentPercent)
			g_currentPercent = PlayLayer::getCurrentPercentInt();
	}
};

// Добавил кнопку сброса рулетки
// Добавил оформление мода
// Изменил спрайты сложностей на колесе
// Исправил отображение всплывающего окна после выхода из него
// Теперь мод успешно работает на всех платформах

//class $modify(LevelCell) {
//	void loadFromLevel(GJGameLevel * level) {
//		LevelCell::loadFromLevel(level);
//		
//		if (level->m_likes < 0 && !level->isPlatformer()) {
//			matjson::Value levelEntry;
//			levelEntry["likes"] = level->m_likes;
//			levelEntry["name"] = level->m_levelName;
//			if (level->m_stars == 1) levelEntry["diff"] = "Auto";
//			else if (level->m_stars == 2) levelEntry["diff"] = "Easy";
//			else if (level->m_stars == 3) levelEntry["diff"] = "Normal";
//			else if (level->m_stars == 4 || level->m_stars == 5) levelEntry["diff"] = "Hard";
//			else if (level->m_stars == 6 || level->m_stars == 7) levelEntry["diff"] = "Harder";
//			else if (level->m_stars == 8 || level->m_stars == 9) levelEntry["diff"] = "Insane";
//			else if (level->m_stars == 10 && level->m_demonDifficulty == 3) levelEntry["diff"] = "EasyDemon";
//			else if (level->m_stars == 10 && level->m_demonDifficulty == 4) levelEntry["diff"] = "MediumDemon";
//			else if (level->m_stars == 10 && level->m_demonDifficulty == 0) levelEntry["diff"] = "HardDemon";
//			else if (level->m_stars == 10 && level->m_demonDifficulty == 5) levelEntry["diff"] = "InsaneDemon";
//			else if (level->m_stars == 10 && level->m_demonDifficulty == 6) levelEntry["diff"] = "ExtremeDemon";
//
//			Mod::get()->setSavedValue<matjson::Value>(std::to_string(level->m_levelID), levelEntry);
//		}
//	}
//};