#include "./levelListPopup.hpp"

LevelListPopup* LevelListPopup::create(std::string listName) {
	auto ret = new LevelListPopup();
	if (ret && ret->init(listName)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool LevelListPopup::init(std::string listName) {
    if (!Popup::init(280.0f, 280.0f)) return false;

    this->setID("list-editor-menu"_spr);
	this->setZOrder(100);
	this->setTitle(fmt::format("\"{}\" level list", listName));

	auto scrollBG = CCLayerColor::create({0, 0, 0, 96});
	scrollBG->setContentSize({240.0f, 220.0f});
	scrollBG->setAnchorPoint({0.5f, 0.5f});
	scrollBG->setPosition(m_size / 2.0f + ccp(0, -10));
	scrollBG->ignoreAnchorPointForPosition(false);
	m_mainLayer->addChild(scrollBG);

	m_scrollingLayer = ScrollLayer::create(scrollBG->getContentSize() - ccp(20, 20));
    m_scrollingLayer->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

	m_scrollingLayer->setAnchorPoint({0.5f, 0.5f});
	m_scrollingLayer->setPosition(scrollBG->getPosition());
	m_scrollingLayer->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChild(m_scrollingLayer);

	auto scrollBorder = ListBorders::create();
    scrollBorder->setContentSize(scrollBG->getContentSize());
	scrollBorder->setPosition(scrollBG->getPosition());
    m_mainLayer->addChild(scrollBorder);

	m_emptyScrollLabel = CCLabelBMFont::create("You don't have lists.\nBut you can create them.", "bigFont.fnt");
	m_emptyScrollLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
	m_emptyScrollLabel->setPosition(scrollBG->getPosition());
	m_emptyScrollLabel->setScale(0.5f);
	m_mainLayer->addChild(m_emptyScrollLabel);

	auto clearBtnSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
	clearBtnSpr->setScale(0.8f);
	auto clearBtn = CCMenuItemExt::createSpriteExtra(clearBtnSpr, [this, listName](auto) { clearList(listName); });
	clearBtn->setPosition(m_buttonMenu->getContentSize());
	m_buttonMenu->addChild(clearBtn);

	populateScroll(listName);

	m_updateListener = UpdateLevelListEvent().listen(
		[this](RouletteLevelListCell* cell) {
			auto content = m_scrollingLayer->m_contentLayer;
			float offset = content->getPositionY();

			if (!cell || !m_scrollingLayer)
				return ListenerResult::Propagate;

			cell->setVisible(false);
			cell->setContentSize({0, 0});
			content->updateLayout();
			
			cell->removeFromParentAndCleanup(true);
			content->updateLayout();
			m_scrollingLayer->setContentOffset({0.0f, offset}, false);

			if (content->getChildrenCount() == 0)
				m_emptyScrollLabel->setVisible(true);

			return ListenerResult::Propagate;
		}
	);

    return true;
}

void LevelListPopup::populateScroll(std::string listName) {
	auto* content = m_scrollingLayer->m_contentLayer;

    auto data = Globals::getListsData();
	if (data.size() == 0 || data[listName].size() == 0) {
		m_emptyScrollLabel->setVisible(true);
		return;
	}
	m_emptyScrollLabel->setVisible(false);

	for (auto& [key, value] : data[listName]) {
		auto cell = RouletteLevelListCell::create(listName, value);
		content->addChild(cell);
		content->updateLayout();
	}
	m_scrollingLayer->moveToTop();
}

void LevelListPopup::clearList(std::string listName) {
	createQuickPopup("Are you sure?", "The list cannot be restored once deleted.", "No", "Yes", [this, listName](auto, bool yesBtn) {
		if (yesBtn) {
			auto data = Globals::getListsData();

			if (data.size() == 0 || data[listName].size() == 0) return;
			data[listName] = matjson::Value::object();

			Globals::setListsData(data);
		}
	});
}