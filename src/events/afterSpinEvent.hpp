#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class AfterSpinEvent : public Event<AfterSpinEvent, bool()> {
public:
    using Event::Event;
};