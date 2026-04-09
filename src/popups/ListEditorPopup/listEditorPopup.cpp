#include "./listEditorPopup.hpp"

ListEditorPopup* ListEditorPopup::create(GJGameLevel* level, bool isLevelAddition) {
	auto ret = new ListEditorPopup();
	if (ret && ret->init(level, isLevelAddition)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool ListEditorPopup::init(GJGameLevel* level, bool isLevelAddition) {
    if (!Popup::init(280.0f, 280.0f)) return false;
	m_isLevelAddition = isLevelAddition;

    this->setID("list-editor-menu"_spr);
	this->setZOrder(100);
	this->setTitle(m_isLevelAddition ? "Select level list" : "Your level lists");
	m_level = level;

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

	auto addBtnSpr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
	addBtnSpr->setScale(0.8f);
	auto addBtn = CCMenuItemExt::createSpriteExtra(addBtnSpr, [this](auto) { NewListPopup::create()->show(); });
	addBtn->setPosition(m_buttonMenu->getContentSize());
	m_buttonMenu->addChild(addBtn);

	populateScroll();

	m_updateListener = UpdateListEditorEvent().listen(
		[this](RouletteListCell* cell) {
			auto content = m_scrollingLayer->m_contentLayer;

			if (!cell || !m_scrollingLayer)
				return ListenerResult::Propagate;

			cell->setVisible(false);
			cell->setContentSize({0, 0});
			content->updateLayout();
			m_scrollingLayer->scrollToTop();
			
			cell->removeFromParentAndCleanup(true);
			content->updateLayout();

			if (content->getChildrenCount() == 0)
				m_emptyScrollLabel->setVisible(true);

			return ListenerResult::Propagate;
		}
	);

	m_populateListener = PopulateListEditorEvent().listen(
		[this]() {
			populateScroll();
			return ListenerResult::Propagate;
		}
	);

    return true;
}

void ListEditorPopup::populateScroll() {
	auto* contentLayer = m_scrollingLayer->m_contentLayer;
	if (contentLayer->getChildrenCount() != 0) {
		for (int i = 0; i < contentLayer->getChildren()->count(); i++) {
			contentLayer->getChildByIndex(i)->removeFromParentAndCleanup(true);
		}
	}

    auto data = Globals::getListsData();
	if (data.size() == 0) {
		m_emptyScrollLabel->setVisible(true);
		return;
	}
	m_emptyScrollLabel->setVisible(false);
	
	for (auto& [key, value] : data) {
		auto cell = RouletteListCell::create(key, value.size(), m_level, m_isLevelAddition);
		contentLayer->addChild(cell);
		contentLayer->updateLayout();
	}
	m_scrollingLayer->moveToTop();
}