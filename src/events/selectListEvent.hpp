#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class SelectListEvent : public Event<SelectListEvent, bool(std::string)> {
public:
    using Event::Event;
};