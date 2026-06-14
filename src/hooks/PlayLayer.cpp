#include <Geode/modify/PlayLayer.hpp>
#include "../Globals.hpp"

using namespace geode::prelude;

class $modify(PlayLayer) {
	void destroyPlayer(PlayerObject* player, GameObject* object) {
		PlayLayer::destroyPlayer(player, object);

		if (m_level->m_levelID == Globals::getCurrentLevel().levelID && PlayLayer::getCurrentPercentInt() > Globals::getCurrentPercent()) {
			int& currentPercent = Globals::getCurrentPercent();
			currentPercent = PlayLayer::getCurrentPercentInt();
		}
	}
};