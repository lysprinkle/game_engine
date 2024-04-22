#pragma once

#include <string>
#include <Vector>
#include <iostream>
#include <unordered_map>
#include <set>
#include "glm/glm.hpp"
#include "Actor.h"

class SceneDB
{

public:
	std::unordered_map<std::string, std::unordered_map<uint64_t, std::pair<std::vector<Actor*>, int>>> scenes;
	std::unordered_map<std::string, std::vector<Actor*>> actors;
	std::unordered_map<std::string, std::vector<Actor*>> active_actor_pointers;

	bool scene_loaded(std::string scene) {
		if (scenes.empty()) return false;
		if (scenes.find(scene) != scenes.end()) {
			return true;
		}
		return false;
	}
};

