#include <Geode/modify/PlayLayer.hpp>
#include "../RoulettePopup/roulettePopup.hpp"
#include "../ListEditorPopup/listEditorPopup.hpp"
#include "../../layers/RouletteLayer/rouletteLayer.hpp"

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

	m_spinsCount = CCLabelBMFont::create(fmt::format("Number of spins: {}", Globals::getSpinsCount()).c_str(), "bigFont.fnt");
	m_spinsCount->setScale(0.4f);
	m_spinsCount->setPosition({ m_size.width / 2.0f, 202.0f });
	m_mainLayer->addChild(m_spinsCount);

	auto spinSpr = ButtonSprite::create("Start");
	m_spinBtn = CCMenuItemExt::createSpriteExtra(spinSpr, [this](auto) {
		if (!readLevelData()) return;

		LevelData empty;
		if (Globals::getCurrentLevel() != empty) {
			Globals::setSpinsCount(Globals::getSpinsCount()-1);
			afterSpinOnPopup();
		}
		else levelChoice();
	});
	m_spinBtn->setPosition({ m_size.width / 2.0f, 110.0f });
	m_spinBtn->setScale(0.8f);
	m_spinBtn->m_baseScale = 0.8f;
	m_buttonMenu->addChild(m_spinBtn);

	auto listsBtnSpr = CCSprite::createWithSpriteFrameName("GJ_viewListsBtn_001.png");
	listsBtnSpr->setScale(0.8f);
	m_listsBtn = CCMenuItemExt::createSpriteExtra(listsBtnSpr, [this](auto) { onListsBtn(); });
	m_listsBtn->setPosition(m_buttonMenu->getContentSize());
	m_buttonMenu->addChild(m_listsBtn);

	m_afterSpinListener = AfterSpinEvent().listen(
		[this]() {
			afterSpinOnPopup();
		}
	);

	return true;
}

bool RoulettePopup::readLevelData() {
	if (!Globals::getLevelData().empty()) return true;
	if (Globals::getCurrentListName().empty()) {
		FLAlertLayer::create(
			"No list selected",
			"You must select a list of levels before using the roulette.",
			"Ok"
		)->show();
		return false;
	}

	std::unordered_map<std::string, LevelData> levelDataList;

	auto data = Globals::getListsData();
	if (data.size() != 0) {
		auto currentList = data[Globals::getCurrentListName()];
		for (auto [key, value] : currentList) {
			LevelData levelData;
			int levelID = numFromString<int>(key, 10).unwrapOrDefault();
			if (levelID == 0) continue;

			levelData.name = value["name"].asString().unwrap();
			levelData.creator = value["creator"].asString().unwrap();
			levelData.diff = value["diff"].asString().unwrap();
			levelData.levelID = levelID;

			levelDataList[key] = levelData;
			Globals::getLevelsMutable().push_back(levelID);
		}

		Globals::setLevelData(levelDataList);
		return true;
	}
	else return false;
}

void RoulettePopup::levelChoice() {
	static std::mt19937 mt(std::random_device{}());
	auto& levels = Globals::getLevelsMutable();
	auto& usedLevels = Globals::getUsedLevelsMutable();
	auto& selectedLevels = Globals::getSelectedLevelsMutable();
	auto& levelsData = Globals::getLevelData();

	if (levels.size() >= 4) {
		Globals::getSelectedLevelsMutable().clear();
		std::shuffle(levels.begin(), levels.end(), mt);
		selectedLevels.assign(levels.begin(), levels.begin() + 4);
	}
	else {
		levels.insert(levels.end(), usedLevels.begin(), usedLevels.end());
		usedLevels.clear();
		FLAlertLayer::create(
			"Whoops!",
			"The levels are out! The list has been restored.",
			"Ok"
		)->show();
		resetRoulette();

		return;
	}

	float deltaAngle = std::uniform_real_distribution<float>(0.0f, 360.0f)(mt);

	int levelIndex = 0;
	for (int i = 0; i < 4; i++) {
		if (deltaAngle - (90.0f * i) < 90.0f) {
			Globals::setCurrentLevel(levelsData[std::to_string(selectedLevels[i])]);

			levels.erase(std::find(levels.begin(), levels.end(), selectedLevels[i]));
			usedLevels.push_back(selectedLevels[i]);

			break;
		}
	}

	RouletteLayer::create(deltaAngle)->show();
}

void RoulettePopup::afterSpinOnPopup() {
	Globals::setSpinsCount(Globals::getSpinsCount() + 1);

	for (auto& node : m_uiEls) {
		if (node && node->getParent()) {
			node->removeFromParentAndCleanup(true);
		}
		node = nullptr;
	}
	m_uiEls.clear();

	m_spinBtn->setVisible(false);
	m_listsBtn->setVisible(false);
	m_spinsCount->setString(fmt::format("Number of spins: {}", Globals::getSpinsCount()).c_str());

	m_resetBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_updateBtn_001.png", 1.0f, [this](auto) {
		createQuickPopup("Confirm reset", "Are you sure you want to reset the roulette?", "No", "Yes", [this](auto, bool yesBtn) {
			if (yesBtn) { resetRoulette(); }
		});
	});
	m_resetBtn->setPosition({ 320.0f, 240.0f });
	m_resetBtn->setScale(0.8f);
	m_resetBtn->m_baseScale = m_resetBtn->getScale();
	m_resetBtn->setZOrder(1);
	m_uiEls.push_back(m_resetBtn);

	auto altSpriteNames = Globals::getAltSpriteNames();
	m_diffSpr = CCSprite::createWithSpriteFrameName(altSpriteNames[Globals::getCurrentLevel().diff].c_str());
	m_diffSpr->setPosition({ 75.0f, 105.0f });
	m_diffSpr->setZOrder(1);
	m_uiEls.push_back(m_diffSpr);
	
	m_levelName = CCLabelBMFont::create("", "bigFont.fnt");
	m_levelName->setPosition({ 160.0f, 170.0f });
	m_levelName->setString(Globals::getCurrentLevel().name.c_str());
	m_levelName->setScale(Globals::getCurrentLevel().name.size() > 12 ? 0.5f : 0.7f);
	m_levelName->setZOrder(1);
	m_uiEls.push_back(m_levelName);

	m_playBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_playBtn2_001.png", 0.8f, [this](auto) {
		auto searchObj = GJSearchObject::create(SearchType::Search, std::to_string(Globals::getCurrentLevel().levelID));

		auto lbl = LevelBrowserLayer::scene(searchObj);
		auto transitionFade = CCTransitionFade::create(0.5, lbl);
		CCDirector::sharedDirector()->pushScene(transitionFade);
	});
	m_playBtn->setPosition({ 160.0f, 110.0f });
	m_playBtn->setZOrder(1);
	m_uiEls.push_back(m_playBtn);

	m_requirePercent = CCLabelBMFont::create("", "bigFont.fnt");
	m_requirePercent->setScale(0.4f);
	m_requirePercent->setPosition({ 160.0f, 40.0f });
	m_requirePercent->setString(fmt::format("{}%", Globals::getRequirePercent()).c_str());
	m_requirePercent->setZOrder(2);
	m_uiEls.push_back(m_requirePercent);
	
	m_requirePercentBG = CCScale9Sprite::create("square02b_001.png");
	m_requirePercentBG->setColor({ 0, 0, 0 });
	m_requirePercentBG->setOpacity(50);
	m_requirePercentBG->setContentSize({ 90.0f, 60.0f });
	m_requirePercentBG->setScale(0.4f);
	m_requirePercentBG->setPosition({ 160.0f, 40.0f });
	m_requirePercentBG->setZOrder(1);
	
	m_uiEls.push_back(m_requirePercentBG);

	m_skipsCount = CCLabelBMFont::create("", "goldFont.fnt");
	m_skipsCount->setScale(0.4f);
	m_skipsCount->setPosition({ 160.0f, 64.0f });
	m_skipsCount->setString(fmt::format("skips: {}", Globals::getSkipsCount()).c_str());
	m_skipsCount->setZOrder(1);
	m_uiEls.push_back(m_skipsCount);

	auto skipSpr = ButtonSprite::create("Skip", "bigFont.fnt", "GJ_button_03.png");
	m_skipBtn = CCMenuItemExt::createSpriteExtra(skipSpr, [this](auto) {
		if (Globals::getRequirePercent() == 100) {
			FLAlertLayer::create(
				"Congrats",
				"You passed the roulette. =3",
				"Hurray!"
			)->show();
		}
		else if (Globals::getSkipsCount() == 0) {
			FLAlertLayer::create(
				"Nope",
				"You spent all your skips. =3",
				"Ok"
			)->show();
		}
		else {
			Globals::setSkipsCount(Globals::getSkipsCount() - 1);
			Globals::setRequirePercent(Globals::getRequirePercent() + 1);
			Globals::setCurrentPercent(0);

			levelChoice();
		}
	});
	m_skipBtn->setScale(0.65f);
	m_skipBtn->m_baseScale = m_skipBtn->getScale();
	m_skipBtn->setPosition({ 105.0f, 40.0f });
	m_skipBtn->setZOrder(1);
	m_uiEls.push_back(m_skipBtn);

	auto nextSpr = ButtonSprite::create("Next", "bigFont.fnt", "GJ_button_01.png");
	m_nextBtn = CCMenuItemExt::createSpriteExtra(nextSpr, [this](auto) {
		if (Globals::getRequirePercent() == 100) {
			FLAlertLayer::create(
				"Congrats",
				"You passed the roulette. =3",
				"Hurray!"
			)->show();
		}
		else if (Globals::getCurrentPercent() < Globals::getRequirePercent()) {
			FLAlertLayer::create(
				"Nope",
				"You haven't reached the required percentage. =3", 
				"Ok"
			)->show();
		}
		else {
			Globals::setRequirePercent(Globals::getRequirePercent() + 1);
			Globals::setCurrentPercent(0);

			levelChoice();
		}
	});
	m_nextBtn->setScale(0.65f);
	m_nextBtn->m_baseScale = m_nextBtn->getScale();
	m_nextBtn->setPosition({ 215.0f, 40.0f });
	m_nextBtn->setZOrder(1);
	m_uiEls.push_back(m_nextBtn);

	for (auto node : m_uiEls) {
		if (auto menuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
			m_buttonMenu->addChild(menuItem);
		}
		else m_mainLayer->addChild(node);
	}
}

void RoulettePopup::resetRoulette() {
	auto& levels = Globals::getLevelsMutable();
	auto& usedLevels = Globals::getUsedLevelsMutable();
	auto& selectedLevels = Globals::getSelectedLevelsMutable();
	levels.insert(levels.end(), usedLevels.begin(), usedLevels.end());
	usedLevels.clear();
	selectedLevels.clear();
	Globals::getCurrentLevel().setDefault();

	Globals::setSpinsCount(0);
	Globals::setSkipsCount(Mod::get()->getSettingValue<int>("skips-count"));
	Globals::setRequirePercent(1);
	Globals::setCurrentPercent(0);

	m_spinBtn->setVisible(true);
	m_listsBtn->setVisible(true);
	m_spinsCount->setString("Number of spins: 0");

	for (auto& node : m_uiEls) {
		if (node && node->getParent()) {
			node->removeFromParentAndCleanup(true);
		}
		node = nullptr;
	}
	m_uiEls.clear();
}

void RoulettePopup::onListsBtn() {
	ListEditorPopup::create(nullptr, false)->show();
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