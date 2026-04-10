#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class StartSpinEvent : public Event<StartSpinEvent, bool(float)> {
public:
    using Event::Event;
};