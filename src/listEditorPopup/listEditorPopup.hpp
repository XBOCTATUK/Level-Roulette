#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RouletteListCell : public CCNode {
public:
    static RouletteListCell* create();
protected:
    bool init();
};

class ListEditorPopup : public geode::Popup {
public:
    static ListEditorPopup* create(bool isLevelAddition);
protected:
    bool init(bool isLevelAddition);
};