#include "./roulette.hpp"
#include <Geode/modify/PlayLayer.hpp>

RoulettePopup* RoulettePopup::create() {
	auto ret = new RoulettePopup();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RoulettePopup::init() {
	if (!Popup::init(320.0f, 240.0f)) return false;

	this->setID("worst-demons-menu"_spr);
	this->setZOrder(100);
	this->setTitle("Let's see what we get today?");

	auto bg = CCScale9Sprite::create("square02b_001.png");
	bg->setColor({ 0, 0, 0 });
	bg->setOpacity(50);
	bg->setContentSize({ 280.0f, 170.0f });
	bg->setPosition({ m_size.width / 2.0f, 105.0f });
	m_mainLayer->addChild(bg);

	m_spinsCount = CCLabelBMFont::create(fmt::format("Number of spins: {}", g_spinsCount).c_str(), "bigFont.fnt");
	m_spinsCount->setScale(0.4f);
	m_spinsCount->setPosition({ m_size.width / 2.0f, 202.0f });
	m_mainLayer->addChild(m_spinsCount);

	auto spinSpr = ButtonSprite::create("Start");
	m_spinBtn = CCMenuItemExt::createSpriteExtra(spinSpr, [this](auto) {
		if (g_currentLvl.levelID != 0) {
			g_spinsCount -= 1;
			afterSpinOnPopup();
		}
		else RouletteLayer::create(this)->show();
	});
	m_spinBtn->setPosition({ m_size.width / 2.0f, 110.0f });
	m_spinBtn->setScale(0.8f);
	m_spinBtn->m_baseScale = 0.8f;
	m_buttonMenu->addChild(m_spinBtn);

	return true;
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

		m_playBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_playBtn2_001.png", 0.8f, [this](auto) {
			auto searchObj = GJSearchObject::create(SearchType::Search, std::to_string(g_currentLvl.levelID));

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
				g_currentPercent = 0;
				RouletteLayer::create(this)->show();
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
				g_currentPercent = 0;
				RouletteLayer::create(this)->show();
			}
		});
		m_nextBtn->setScale(0.65f);
		m_nextBtn->m_baseScale = m_nextBtn->getScale();
		m_nextBtn->setPosition({ 215.0f, 40.0f });

		m_mainLayer->addChild(m_diffSpr);
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

		if (!m_playBtn) {
			m_playBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_playBtn2_001.png", 0.8f, [this](auto) {
				auto searchObj = GJSearchObject::create(SearchType::Search, std::to_string(g_currentLvl.levelID));
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
					g_currentPercent = 0;
					RouletteLayer::create(this)->show();
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
					g_currentPercent = 0;
					RouletteLayer::create(this)->show();
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
			g_levels.insert(g_levels.end(), g_usedLevels.begin(), g_usedLevels.end());
			g_usedLevels.clear();
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

void RoulettePopup::keyDown(enumKeyCodes key, double d) {
	switch (key) {
	case KEY_Escape:
		onClose(nullptr);
		break;
	default:
		Popup::keyDown(key, d);
		break;
	}
}