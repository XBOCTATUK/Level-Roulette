#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class ListSelectionEvent : public Event<ListSelectionEvent, bool()> {
public:
    using Event::Event;
};