#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class PopulateListEditorEvent : public Event<PopulateListEditorEvent, bool()> {
public:
    using Event::Event;
};