#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class RouletteListCell;

class UpdateListEditorEvent : public Event<UpdateListEditorEvent, bool(RouletteListCell*)> {
public:
    using Event::Event;
};