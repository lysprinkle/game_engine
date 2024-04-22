#include<iostream>
#include<unordered_map>
#include<set>
#include <utility>
#include <vector>
#include <chrono>
#include "SceneDB.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "Utils.h"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ImageDB.h"
#include "TextDB.h"


#pragma once

class GameStart
{
private:

	const std::string resources_path = "resources/";
	const std::string game_config_file_path = "resources/game.config";
	const std::string rendering_config_file_path = "resources/rendering.config";
	const std::string scene_config_file_path = "resources/scenes/";
	const std::string templates_path = "resources/actor_templates/"; 

	rapidjson::Document game_json;
	rapidjson::Document render_json;
	rapidjson::Document scenes_json;

	std::string initial_scene = "";

	std::string game_title = "";

	SDL_Window* window;

	TTF_Font* font;
	Mix_Chunk* mix_chunk;

	int clear_color_r = 255;
	int clear_color_g = 255;
	int clear_color_b = 255;

	float ori_camera_offset_x = 0;
	float ori_camera_offset_y = 0;

	SceneDB* scenedb;
	std::unordered_map<std::string, Actor*> templates;

	lua_State* lua_state;

public:
	static inline SDL_Renderer* renderer;
	static inline std::vector<Actor*> actors_loaded;
	static inline int cur_id = 0;

	static inline std::vector<Actor*> actors;
	std::vector<Actor*> actors_sorted;

	static inline bool new_scene = false;
	static inline std::string next_scene_name = "";
	static inline std::string cur_scene_name = "";

	static inline glm::vec2 mouse_position = {0.0, 0.0};
	static glm::vec2 GetMousePosition1();

	static inline float  scroll_delta = 0.0f;
	static float GetScrollDelta1();

	// Process JSON
	void check_json_file();

	void game_initialization();
	void load_rendering();
	void load_image();
	void load_scene_json(bool, std::string);
	void scene_initialization(bool, std::string scene_name);

	void game_running();

	int get_action(std::string);

	static Actor* Find1(const std::string&);

	static std::vector<Actor*> FindAll1(const std::string&);

	static inline std::vector<Actor*> actor_to_be_deleted;

	static inline float camera_offset_x = 0;
	static inline float camera_offset_y = 0;

	static inline float camera_dimension_x = 0;
	static inline float camera_dimension_y = 0;

	static inline float zoom_factor = 2.0f;

	static inline int x_resolution_screen = 640;
	static inline int y_resolution_screen = 360;

	static inline std::vector<int> ActorDontDestroy;

	static inline auto start = std::chrono::high_resolution_clock::now();

	static inline int left_x_axis = 0;
	static inline int left_y_axis = 0;
	static inline int right_x_axis = 0;
	static inline int right_y_axis = 0;
};

