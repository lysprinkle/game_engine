#pragma once
#include <unordered_map>
#include "SceneDB.h"
#include "Actor.h"

class TemplateDB
{
public:
	std::unordered_map<std::string, Actor> templates;
};

