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
    if (!Popup::init(360.0f, 280.0f)) return false;
	m_isLevelAddition = isLevelAddition;

    this->setID("list-editor-menu"_spr);
	this->setZOrder(100);
	this->setTitle(m_isLevelAddition ? "Select level list" : "Your level lists");
	m_level = level;

	auto scrollBG = CCLayerColor::create({0, 0, 0, 96});
	scrollBG->setContentSize({320.0f, 200.0f});
	scrollBG->setAnchorPoint({0.5f, 0.5f});
	scrollBG->setPosition(m_size / 2.0f);
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

	auto bottomBtnsMenu = CCMenu::create();
	bottomBtnsMenu->setLayout(
		RowLayout::create()
		->setAutoScale(false)
		->setAxisAlignment(AxisAlignment::Center)
		->setGap(45.0f)
	);
	bottomBtnsMenu->setContentSize({ 320.0f, 36.0f });
	bottomBtnsMenu->setPosition({ m_size.width / 2.0f, 24.0f });
	m_mainLayer->addChild(bottomBtnsMenu);

	auto exportBtnSpr = ButtonSprite::create("Export");
	exportBtnSpr->setScale(0.85f);
	auto exportBtn = CCMenuItemExt::createSpriteExtra(exportBtnSpr, [this](auto) {
		if (m_selectedLists.empty()) {
			FLAlertLayer::create(
				"No selected lists",
				"Select lists to export and try again",
				"Ok"
			)->show();
		}
		else {
			async::spawn(
				file::pick(file::PickMode::SaveFile, file::FilePickOptions {
					.filters = { file::FilePickOptions::Filter {
						.description = "Select save folder",
						.files = { "*.json" },
					}}
				}),
				[this](Result<std::optional<std::filesystem::path>> result) {
					if (result.isOk() && result.unwrap().has_value()) {
						auto path = result.unwrap().value();

						auto data = Globals::getListsData();
						auto selectedData = matjson::Value::object();

						for (auto listName : m_selectedLists) {
							selectedData[listName] = data[listName];
						}

						auto writeResult = file::writeToJson(path, selectedData);
						if (writeResult.isErr()) {
							FLAlertLayer::create(
								"Error",
								"Failed to save the list. Please try again later.",
								"Ok"
							)->show();
						}
					}
					else {
						FLAlertLayer::create(
							"Save error",
							"Failed to save file, try selecting a different path",
							"Ok"
						)->show();
					}
				}
			);
		}
	});
	bottomBtnsMenu->addChild(exportBtn);

	auto importBtnSpr = ButtonSprite::create("Import");
	importBtnSpr->setScale(0.85f);
	auto importBtn = CCMenuItemExt::createSpriteExtra(importBtnSpr, [this](auto) {
		async::spawn(
			file::pick(file::PickMode::OpenFile, file::FilePickOptions {
				.filters = { file::FilePickOptions::Filter {
					.description = "Select file with list data",
					.files = { "*.json" },
				}}
			}),
			[this](Result<std::optional<std::filesystem::path>> result) {
				if (result.isOk() && result.unwrap().has_value()) {
					auto path = result.unwrap().value();

					auto readImportedData = file::readJson(path);
					if (readImportedData.isErr()) {
						FLAlertLayer::create(
							"Error",
							"Failed to open the lists file. Please try again later.",
							"Ok"
						)->show();
						return;
					}
					auto importedData = readImportedData.unwrap();
					if (!isValidImport(importedData)) {
						FLAlertLayer::create(
							"Error",
							"Incorrect file content, make sure you selected the correct file.",
							"Ok"
						)->show();
						
						return;
					}

					createQuickPopup("Import Lists", "How should duplicate list names be handled?", "Copy", "Replace",
						[importedData](auto, bool replace) {
							auto data = Globals::getListsData();
							bool changed = false;

							for (auto const& [key, value] : importedData) {
								if (!data.contains(key)) {
									data[key] = value;
									changed = true;
									continue;
								}

								if (replace) {
									data[key] = value;
								}
								else {
									int copyNum = 1;
									std::string copyName;

									do copyName = fmt::format("{} (copy {})", key, copyNum++);
									while (data.contains(copyName));

									data[copyName] = value;
								}

								changed = true;
							}

							if (changed) {
								Globals::saveListsData(data);
								PopulateListEditorEvent().send();
							}
						}
					);
				}
				else {
					FLAlertLayer::create(
						"Read error",
						"Failed to read file, try again later.",
						"Ok"
					)->show();
				}
			}
		);
	});
	bottomBtnsMenu->addChild(importBtn);
	bottomBtnsMenu->updateLayout();

	populateScroll();

	m_updateListener = UpdateListEditorEvent().listen(
		[this](RouletteListCell* cell) {
			auto content = m_scrollingLayer->m_contentLayer;
			float offset = content->getPositionY();

			if (!cell || !m_scrollingLayer) {
				return ListenerResult::Propagate;
			}

			cell->setVisible(false);
			cell->setContentSize({0, 0});
			content->updateLayout();
			
			cell->removeFromParentAndCleanup(true);
			content->updateLayout();
			m_scrollingLayer->setContentOffset({0.0f, offset}, false);

			if (content->getChildrenCount() == 0) {
				m_emptyScrollLabel->setVisible(true);
			}

			return ListenerResult::Propagate;
		}
	);

	m_populateListener = PopulateListEditorEvent().listen(
		[this]() {
			populateScroll();
			return ListenerResult::Propagate;
		}
	);

	m_selectListListener = SelectListEvent().listen(
		[this](std::string listName) {
			if (!m_selectedLists.erase(listName)) {
				m_selectedLists.insert(listName);
			}

			return ListenerResult::Propagate;
		}
	);

    return true;
}

void ListEditorPopup::populateScroll() {
	auto* content = m_scrollingLayer->m_contentLayer;
	if (content->getChildrenCount() != 0) {
		content->getChildren()->removeAllObjects();
	}

    auto data = Globals::getListsData();
	if (data.size() == 0) {
		m_emptyScrollLabel->setVisible(true);
		return;
	}
	m_emptyScrollLabel->setVisible(false);
	
	for (auto& [key, value] : data) {
		auto cell = RouletteListCell::create(key, value.size(), m_level, m_isLevelAddition);
		content->addChild(cell);
		content->updateLayout();
	}
	m_scrollingLayer->moveToTop();
}

bool ListEditorPopup::isValidLevel(matjson::Value const& level) {
	if (!level.isObject()) return false;

    return
        level.contains("name") && level["name"].isString() &&

        level.contains("levelID") && level["levelID"].isNumber() &&

        level.contains("creator") && level["creator"].isString() &&

        level.contains("diff") && level["diff"].isString();
}

bool ListEditorPopup::isValidImport(matjson::Value const& data) {
    if (!data.isObject())
        return false;

    for (auto const& [listName, levels] : data) {
        if (!levels.isObject()) return false;

        for (auto const& [levelID, levelData] : levels) {
            auto parse = utils::numFromString<int>(levelID);
			if (parse.isErr()) return false;

            if (!isValidLevel(levelData)) return false;
        }
    }

    return true;
}