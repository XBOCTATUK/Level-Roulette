#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class RouletteLevelListCell;

class UpdateLevelListEvent : public Event<UpdateLevelListEvent, bool(RouletteLevelListCell*)> {
public:
    using Event::Event;
};