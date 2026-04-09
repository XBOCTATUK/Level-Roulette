#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class LevelCountEvent : public Event<LevelCountEvent, bool(int), std::string> {
public:
    using Event::Event;
};