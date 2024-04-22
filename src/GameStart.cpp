#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <set>
#include <algorithm>
#include <thread>
#include "GameStart.h"
#include "TemplateDB.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "Utils.h"
#include "Helper.h"
#include "AudioHelper.h"
#include "Input.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Renderer.h"

std::unordered_map<uint64_t, std::pair<std::vector<Actor*>, int>> actor_locations;
std::vector<Actor*> actors;

void GameStart::check_json_file() {

	if (!std::filesystem::exists(resources_path)) {
		std::cout << "error: resources/ missing";
		std::exit(0);
	}

	if (!std::filesystem::exists(game_config_file_path)) {
		std::cout << "error: resources/game.config missing";
		std::exit(0);
	}
}

Actor* GameStart::Find1(const std::string& name) {
	for (Actor* actor : actors) {
		if (actor->actor_name == name && actor->live) {
			return actor;
		}
	}
	return nullptr;
}

luabridge::LuaRef Find(const std::string& name, lua_State* lua_state) {
	Actor* actor = GameStart::Find1(name);
	if (actor) {
		luabridge::push(lua_state, actor);
		return luabridge::LuaRef(lua_state, actor);
	}
	return luabridge::LuaRef(lua_state);
}

glm::vec2 GetMousePosition() {
	return GameStart::GetMousePosition1();
}

glm::vec2 GameStart::GetMousePosition1() {
	return mouse_position;
}
float GetScrollDelta() {
	return GameStart::GetScrollDelta1();
}

float GameStart::GetScrollDelta1() {
	return scroll_delta;
}

std::vector<Actor*> GameStart::FindAll1(const std::string& name) {
	std::vector<Actor*> cur_actors;
	int i = 0;
	for (Actor* actor : actors) {
		if (actor->actor_name == name && actor->live) {
			cur_actors.push_back(actor);
		}
	}

	return cur_actors;
}

luabridge::LuaRef FindAll(const std::string& name, lua_State* lua_state) {
	std::vector<Actor*> actors = GameStart::FindAll1(name);
	luabridge::LuaRef table = luabridge::newTable(lua_state);

	int i = 0;
	if (!actors.empty()) {
		for (Actor* actor : actors) {
			table[++i] = actor;
		}
	}
	return table;
}

void DestroyActor(Actor& actor) {
	for (auto a : GameStart::actors) {
		if (a->id == actor.id) {
			GameStart::actor_to_be_deleted.push_back(a);
			for (auto comp : a->componenet_instances) {
				comp.second["enabled"] = false;
			}
			a->live = false;
		}
	}
}

void Play(int channel, std::string clip_name, bool does_loop) {
	Mix_Chunk* clip = AudioHelper::Mix_LoadWAV498(clip_name.c_str());
	if (does_loop) {
		AudioHelper::Mix_PlayChannel498(channel, clip, -1);
	} 
	else {
		AudioHelper::Mix_PlayChannel498(channel, clip, 0);
	} 
	
}

void Halt(int channel) {
	AudioHelper::Mix_HaltChannel498(channel);
}

void SetVolume(int channel, float volume) {
	int intVolume = static_cast<int>(floor(volume));
	AudioHelper::Mix_Volume498(channel, intVolume);
}

void SetPosition(float x, float y) {
	GameStart::camera_offset_x = x;
	GameStart::camera_offset_y = y;
}

float GetPositionX() {
	return GameStart::camera_offset_x;
}

float GetPositionY() {
	return GameStart::camera_offset_y;
}

void SetZoom(float zoom_factor) {
	GameStart::zoom_factor = zoom_factor;
}

float GetZoom() {
	return GameStart::zoom_factor;
}

void Load(std::string str) {
	GameStart::new_scene = true;
	GameStart::next_scene_name = str;
}

std::string GetCurrent() { 
	return GameStart::cur_scene_name;
}

void DontDestroy(Actor& actor) {
	GameStart::ActorDontDestroy.push_back(actor.id);

}

int GetLeftXAxis() {
	return GameStart::left_x_axis;
}

int GetLeftYAxis() {
	return GameStart::left_y_axis;
}

int GetRightXAxis() {
	return GameStart::right_x_axis;
}

int GetRightYAxis() {
	return GameStart::right_y_axis;
}

luabridge::LuaRef Instantiate(std::string actor_template_name, lua_State* lua_state) {

	std::string template_path = "resources/actor_templates/" + actor_template_name + ".template";

	if (!std::filesystem::exists(template_path)) {
		std::cout << "error: template " << actor_template_name << " is missing";
		std::exit(0);
	}

	rapidjson::Document template_json;
	ReadJsonFile(template_path, template_json);

	Actor* cur_actor = new Actor();
	cur_actor->id = GameStart::cur_id;
	GameStart::cur_id++;

	if (template_json.HasMember("name")) cur_actor->actor_name = template_json["name"].GetString();

	if (template_json.HasMember("components")) {
		const rapidjson::Value& components = template_json["components"];

		for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
			const std::string key = itr->name.GetString();
			const rapidjson::Value& component = itr->value;

			luabridge::LuaRef templateInstance = luabridge::newTable(lua_state);
			for (rapidjson::Value::ConstMemberIterator itr1 = component.MemberBegin(); itr1 != component.MemberEnd(); ++itr1) {
				const std::string key1 = itr1->name.GetString();

				std::string component1;
				int component2;
				float component3;
				bool component4;

				if (itr1->value.IsString()) {
					component1 = itr1->value.GetString();
				}
				else if (itr1->value.IsInt()) {
					component2 = itr1->value.GetInt();
				}
				else if (itr1->value.IsDouble()) {
					component3 = itr1->value.GetDouble();
				}
				else if (itr1->value.IsBool()) {
					component4 = itr1->value.GetBool();
				}

				if (key1 == "type") {

					if (!std::filesystem::exists(get_lua_file_location(component1))) {
						std::cout << "error: failed to locate component " << component1;
						exit(0);
					}

					if (luaL_dofile(lua_state, get_lua_file_location(component1).c_str()) != 0) {
						std::cout << "problem with lua file " << component1;
						exit(0);
					}

					luabridge::LuaRef componentDef = luabridge::getGlobal(lua_state, component1.c_str());

					establishInheritance(templateInstance, componentDef, lua_state);
					cur_actor->componenet_types[key] = component1;

				}
				else {
					if (itr1->value.IsString()) {
						templateInstance[key1] = component1;
					}
					else if (itr1->value.IsInt()) {
						templateInstance[key1] = component2;
					}
					else if (itr1->value.IsDouble()) {
						templateInstance[key1] = component3;
					}
					else if (itr1->value.IsBool()) {
						templateInstance[key1] = component4;
					}
				}
			}

			luabridge::LuaRef componentInstance = luabridge::newTable(lua_state);
			establishInheritance(componentInstance, templateInstance, lua_state);

			cur_actor->template_instances1.push_back(key);
			cur_actor->template_instances2.push_back(componentInstance);

			componentInstance["key"] = key;
			componentInstance["enabled"] = true;

			luabridge::getGlobalNamespace(lua_state)
				.beginClass<Actor>("Actor")
				.addFunction("GetName", &Actor::GetName)
				.addFunction("GetID", &Actor::GetID)
				.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
				.addFunction("GetComponent", &Actor::GetComponent)
				.addFunction("GetComponents", &Actor::GetComponents)
				.addFunction("AddComponent", &Actor::AddComponent)
				.addFunction("RemoveComponent", &Actor::RemoveComponent)
				.endClass();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Actor")
				.addFunction("Find", &Find)
				.addFunction("FindAll", &FindAll)
				.addFunction("Instantiate", &Instantiate)
				.addFunction("Destroy", &DestroyActor)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Application")
				.addFunction("Quit", &Quit)
				.addFunction("Sleep", &Sleep)
				.addFunction("GetFrame", &GetFrame)
				.addFunction("OpenURL", &OpenURL)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Input")
				.addFunction("GetKey", &input::GetKey)
				.addFunction("GetKeyDown", &input::GetKeyDown)
				.addFunction("GetKeyUp", &input::GetKeyUp)
				.addFunction("GetButton", &input::GetButton)
				.addFunction("GetButtonDown", &input::GetButtonDown)
				.addFunction("GetButtonUp", &input::GetButtonUp)
				.addFunction("GetMousePosition", &GetMousePosition)
				.addFunction("GetMouseButton", &input::GetMouse)
				.addFunction("GetMouseButtonDown", &input::GetMouseDown)
				.addFunction("GetMouseButtonUp", &input::GetMouseUp)
				.addFunction("GetMouseScrollDelta", &GetScrollDelta)

				.addFunction("GetLeftXAxis", &GetLeftXAxis)
				.addFunction("GetLeftYAxis", &GetLeftYAxis)
				.addFunction("GetRightXAxis", &GetRightXAxis)
				.addFunction("GetRightYAxis", &GetRightYAxis)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginClass<glm::vec2>("vec2")
				.addProperty("x", &glm::vec2::x)
				.addProperty("y", &glm::vec2::y)
				.endClass();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Text")
				.addFunction("Draw", &renderer::DrawText)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Image")
				.addFunction("DrawUI", &renderer::DrawUI)
				.addFunction("DrawUIEx", &renderer::DrawUIEx)
				.addFunction("Draw", &renderer::DrawImage)
				.addFunction("DrawEx", &renderer::DrawEx)
				.addFunction("DrawPixel", &renderer::DrawPixel)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Audio")
				.addFunction("Play", &Play)
				.addFunction("Halt", &Halt)
				.addFunction("SetVolume", &SetVolume)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Camera")
				.addFunction("SetPosition", &SetPosition)
				.addFunction("GetPositionX", &GetPositionX)
				.addFunction("GetPositionY", &GetPositionY)
				.addFunction("SetZoom", &SetZoom)
				.addFunction("GetZoom", &GetZoom)
				.endNamespace();

			luabridge::getGlobalNamespace(lua_state)
				.beginNamespace("Scene")
				.addFunction("Load", &Load)
				.addFunction("GetCurrent", &GetCurrent)
				.addFunction("DontDestroy", &DontDestroy)
				.endNamespace();

			std::shared_ptr<luabridge::LuaRef> component_ref = std::make_shared<luabridge::LuaRef>(componentInstance);
			cur_actor->InjectConvenienceReferences(component_ref);

			cur_actor->componenet_instances.emplace(key, componentInstance);
		}

		GameStart::actors_loaded.push_back(cur_actor);
		GameStart::actors.push_back(cur_actor);
	}

	if (cur_actor) {
		luabridge::push(lua_state, cur_actor);
		return luabridge::LuaRef(lua_state, cur_actor);
	}
	return luabridge::LuaRef(lua_state);
}

void ReportError(const std::string& actor_name, const luabridge::LuaException& e)
{
	std::string error_message = e.what();

	std::replace(error_message.begin(), error_message.end(), '\\', '/');

	std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
}

void GameStart::load_rendering() {
	if (std::filesystem::exists("resources/rendering.config")) {
		ReadJsonFile("resources/rendering.config", render_json);

		if (render_json.HasMember("zoom_factor")) {
			zoom_factor = render_json["zoom_factor"].GetDouble();
		}

		if (render_json.HasMember("x_resolution")) {
			x_resolution_screen = render_json["x_resolution"].GetInt();
		}

		if (render_json.HasMember("y_resolution")) {
			y_resolution_screen = render_json["y_resolution"].GetInt();
		}

		ori_camera_offset_x = 0;
		ori_camera_offset_y = 0;


		if (render_json.HasMember("cam_offset_x")) {
			ori_camera_offset_x -= render_json["cam_offset_x"].GetDouble() * 100 * zoom_factor;
		}

		if (render_json.HasMember("cam_offset_y")) {
			ori_camera_offset_y -= render_json["cam_offset_y"].GetDouble() * 100 * zoom_factor;
		}

		if (render_json.HasMember("clear_color_r")) {
			clear_color_r = render_json["clear_color_r"].GetInt();
		}

		if (render_json.HasMember("clear_color_g")) {
			clear_color_g = render_json["clear_color_g"].GetInt();
		}

		if (render_json.HasMember("clear_color_b")) {
			clear_color_b = render_json["clear_color_b"].GetInt();
		}
	}
	else {
		ori_camera_offset_x = 0;
		ori_camera_offset_y = 0;
	}
}

void GameStart::load_image() {
	if (std::filesystem::exists("resources/rendering.config")) {
		ReadJsonFile("resources/rendering.config", render_json);

		if (render_json.HasMember("x_resolution")) {
			x_resolution_screen = render_json["x_resolution"].GetInt();
		}

		if (render_json.HasMember("y_resolution")) {
			y_resolution_screen = render_json["y_resolution"].GetInt();
		}

		if (render_json.HasMember("clear_color_r")) {
			clear_color_r = render_json["clear_color_r"].GetInt();
		}

		if (render_json.HasMember("clear_color_g")) {
			clear_color_g = render_json["clear_color_g"].GetInt();
		}

		if (render_json.HasMember("clear_color_b")) {
			clear_color_b = render_json["clear_color_b"].GetInt();
		}
	}
}

void GameStart::load_scene_json(bool first_load, std::string scene_name) {
	scenedb = new SceneDB();

	if (first_load) {
		if (!game_json.HasMember("initial_scene")) {
			std::cout << "error: initial_scene unspecified";
			std::exit(0);
		}
		else {
			scene_name = game_json["initial_scene"].GetString();
		}
	}
	GameStart::cur_scene_name = scene_name;
	camera_offset_x = 0;
	camera_offset_y = 0;

	if (scenedb->scene_loaded(scene_name)) {
		for (auto a : scenedb->actors[scene_name]) {
			actors.push_back(a);
		}
	}
	else if (std::filesystem::exists(scene_config_file_path + scene_name + ".scene")) {
		ReadJsonFile(scene_config_file_path + scene_name + ".scene", scenes_json);

		for (const auto& actor : scenes_json["actors"].GetArray()) {
			Actor* cur_actor = new Actor();
			cur_actor->id = GameStart::cur_id;

			if (actor.HasMember("template")) {
				std::string actor_template = actor["template"].GetString();
				std::string template_path = templates_path + actor_template + ".template";

				if (!std::filesystem::exists(template_path)) {
					std::cout << "error: template " << actor_template << " is missing";
					std::exit(0);
				}

				rapidjson::Document template_json;
				ReadJsonFile(template_path, template_json);

				if (template_json.HasMember("name")) cur_actor->actor_name = template_json["name"].GetString();

				if (template_json.HasMember("components")) {
					const rapidjson::Value& components = template_json["components"];

					for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
						const std::string key = itr->name.GetString();
						const rapidjson::Value& component = itr->value;

						luabridge::LuaRef templateInstance = luabridge::newTable(lua_state);
						for (rapidjson::Value::ConstMemberIterator itr1 = component.MemberBegin(); itr1 != component.MemberEnd(); ++itr1) {
							const std::string key1 = itr1->name.GetString();

							std::string component1;
							int component2;
							float component3;
							bool component4;

							if (itr1->value.IsString()) {
								component1 = itr1->value.GetString();
							}
							else if (itr1->value.IsInt()) {
								component2 = itr1->value.GetInt();
							}
							else if (itr1->value.IsDouble()) {
								component3 = itr1->value.GetDouble();
							}
							else if (itr1->value.IsBool()) {
								component4 = itr1->value.GetBool();
							}

							if (key1 == "type") {

								if (!std::filesystem::exists(get_lua_file_location(component1))) {
									std::cout << "error: failed to locate component " << component1;
									exit(0);
								}

								if (luaL_dofile(lua_state, get_lua_file_location(component1).c_str()) != 0) {
									std::cout << "problem with lua file " << component1;
									exit(0);
								}

								luabridge::LuaRef componentDef = luabridge::getGlobal(lua_state, component1.c_str());

								establishInheritance(templateInstance, componentDef, lua_state);
								cur_actor->componenet_types[key] = component1;

							}
							else {
								if (itr1->value.IsString()) {
									templateInstance[key1] = component1;
								}
								else if (itr1->value.IsInt()) {
									templateInstance[key1] = component2;
								}
								else if (itr1->value.IsDouble()) {
									templateInstance[key1] = component3;
								}
								else if (itr1->value.IsBool()) {
									templateInstance[key1] = component4;
								}
							}
						}
						templateInstance["enabled"] = true;
						cur_actor->template_instances1.push_back(key);
						cur_actor->template_instances2.push_back(templateInstance);
					}
				}
			}

			if (actor.HasMember("name")) cur_actor->actor_name = actor["name"].GetString();
			if (actor.HasMember("components")) {
				const rapidjson::Value& components = actor["components"];

				for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
					const std::string key = itr->name.GetString();
					const rapidjson::Value& component = itr->value;


					luabridge::LuaRef componentInstance = luabridge::newTable(lua_state);
					bool has_template = false;

					for (int i = 0; i < cur_actor->template_instances1.size(); i++) {
						if (cur_actor->template_instances1[i] == key) {
							has_template = true;
							luabridge::LuaRef templateRef = cur_actor->template_instances2[i];

							establishInheritance(componentInstance, templateRef, lua_state);

							for (rapidjson::Value::ConstMemberIterator itr1 = component.MemberBegin(); itr1 != component.MemberEnd(); ++itr1) {
								const std::string key1 = itr1->name.GetString();

								if (key1 == "type") continue;

								std::string component1;
								int component2;
								float component3;
								bool component4;

								if (itr1->value.IsString()) {
									component1 = itr1->value.GetString();
								}
								else if (itr1->value.IsInt()) {
									component2 = itr1->value.GetInt();
								}
								else if (itr1->value.IsDouble()) {
									component3 = itr1->value.GetDouble();
								}
								else if (itr1->value.IsBool()) {
									component4 = itr1->value.GetBool();
								}

								if (itr1->value.IsString()) {
									componentInstance[key1] = component1;
								}
								else if (itr1->value.IsInt()) {
									componentInstance[key1] = component2;
								}
								else if (itr1->value.IsDouble()) {
									componentInstance[key1] = component3;
								}
								else if (itr1->value.IsBool()) {
									componentInstance[key1] = component4;
								}
							}
							break;
						}
					}

					if (!has_template) {
						for (rapidjson::Value::ConstMemberIterator itr1 = component.MemberBegin(); itr1 != component.MemberEnd(); ++itr1) {
							const std::string key1 = itr1->name.GetString();

							std::string component1;
							int component2;
							float component3;
							bool component4;

							if (itr1->value.IsString()) {
								component1 = itr1->value.GetString();
							}
							else if (itr1->value.IsInt()) {
								component2 = itr1->value.GetInt();
							}
							else if (itr1->value.IsDouble()) {
								component3 = itr1->value.GetDouble();
							}
							else if (itr1->value.IsBool()) {
								component4 = itr1->value.GetBool();
							}

							if (key1 == "type") {

								if (!std::filesystem::exists(get_lua_file_location(component1))) {
									std::cout << "error: failed to locate component " << component1;
									exit(0);
								}

								if (luaL_dofile(lua_state, get_lua_file_location(component1).c_str()) != 0) {
									std::cout << "problem with lua file " << component1;
									exit(0);
								}

								luabridge::LuaRef componentDef = luabridge::getGlobal(lua_state, component1.c_str());

								establishInheritance(componentInstance, componentDef, lua_state);
								cur_actor->componenet_types[key] = component1;

							}
							else {
								if (itr1->value.IsString()) {
									componentInstance[key1] = component1;
								}
								else if (itr1->value.IsInt()) {
									componentInstance[key1] = component2;
								}
								else if (itr1->value.IsDouble()) {
									componentInstance[key1] = component3;
								}
								else if (itr1->value.IsBool()) {
									componentInstance[key1] = component4;
								}

							}
						}

					}
					componentInstance["key"] = key;
					componentInstance["enabled"] = true;

					cur_actor->componenet_instances.emplace(key, componentInstance);
				}
			}

			for (auto c : cur_actor->componenet_instances) {
				luabridge::getGlobalNamespace(lua_state)
					.beginClass<Actor>("Actor")
					.addFunction("GetName", &Actor::GetName)
					.addFunction("GetID", &Actor::GetID)
					.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
					.addFunction("GetComponent", &Actor::GetComponent)
					.addFunction("GetComponents", &Actor::GetComponents)
					.addFunction("AddComponent", &Actor::AddComponent)
					.addFunction("RemoveComponent", &Actor::RemoveComponent)
					.endClass();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Actor")
					.addFunction("Find", &Find)
					.addFunction("FindAll", &FindAll)
					.addFunction("Instantiate", &Instantiate)
					.addFunction("Destroy", &DestroyActor)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Application")
					.addFunction("Quit", &Quit)
					.addFunction("Sleep", &Sleep)
					.addFunction("GetFrame", &GetFrame)
					.addFunction("OpenURL", &OpenURL)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Input")
					.addFunction("GetKey", &input::GetKey)
					.addFunction("GetKeyDown", &input::GetKeyDown)
					.addFunction("GetKeyUp", &input::GetKeyUp)
					.addFunction("GetMousePosition", &GetMousePosition)
					.addFunction("GetMouseButton", &input::GetMouse)
					.addFunction("GetMouseButtonDown", &input::GetMouseDown)
					.addFunction("GetMouseButtonUp", &input::GetMouseUp)
					.addFunction("GetMouseScrollDelta", &GetScrollDelta)

					.addFunction("GetButton", &input::GetButton)
					.addFunction("GetButtonDown", &input::GetButtonDown)
					.addFunction("GetButtonUp", &input::GetButtonUp)

					.addFunction("GetLeftXAxis", &GetLeftXAxis)
					.addFunction("GetLeftYAxis", &GetLeftYAxis)
					.addFunction("GetRightXAxis", &GetRightXAxis)
					.addFunction("GetRightYAxis", &GetRightYAxis)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginClass<glm::vec2>("vec2")
					.addProperty("x", &glm::vec2::x)
					.addProperty("y", &glm::vec2::y)
					.endClass();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Text")
					.addFunction("Draw", &renderer::DrawText)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Image")
					.addFunction("DrawUI", &renderer::DrawUI)
					.addFunction("DrawUIEx", &renderer::DrawUIEx)
					.addFunction("Draw", &renderer::DrawImage)
					.addFunction("DrawEx", &renderer::DrawEx)
					.addFunction("DrawPixel", &renderer::DrawPixel)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Audio")
					.addFunction("Play", &Play)
					.addFunction("Halt", &Halt)
					.addFunction("SetVolume", &SetVolume)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Camera")
					.addFunction("SetPosition", &SetPosition)
					.addFunction("GetPositionX", &GetPositionX)
					.addFunction("GetPositionY", &GetPositionY)
					.addFunction("SetZoom", &SetZoom)
					.addFunction("GetZoom", &GetZoom)
					.endNamespace();

				luabridge::getGlobalNamespace(lua_state)
					.beginNamespace("Scene")
					.addFunction("Load", &Load)
					.addFunction("GetCurrent", &GetCurrent)
					.addFunction("DontDestroy", &DontDestroy)
					.endNamespace();

				std::shared_ptr<luabridge::LuaRef> component_ref = std::make_shared<luabridge::LuaRef>(c.second);
				cur_actor->InjectConvenienceReferences(component_ref);
			}

			for (int i = 0; i < cur_actor->template_instances1.size(); i++) {
				if (cur_actor->componenet_instances.find(cur_actor->template_instances1[i]) == cur_actor->componenet_instances.end()) {
					cur_actor->componenet_instances.emplace(cur_actor->template_instances1[i], cur_actor->template_instances2[i]);
				}
			}

			actors_loaded.push_back(cur_actor);
			actors.push_back(cur_actor);

			GameStart::cur_id++;
		}
	}
	else {
		std::cout << "error: scene " << scene_name << " is missing";
		std::exit(0);
	}
}

void cppLog(const std::string& message) {
	std::cout << message << std::endl;
}

void cppLogError(const std::string& message) {
	std::cerr << message << std::endl;
}

void GameStart::game_initialization() {
	check_json_file();

	ReadJsonFile("resources/game.config", game_json);
	load_rendering();


	window = Helper::SDL_CreateWindow498(game_title.c_str(), 800, 500, x_resolution_screen, y_resolution_screen, SDL_WINDOW_SHOWN);
	renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	Renderer::renderer = renderer;

	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", cppLog)
		.addFunction("LogError", cppLogError)
		.endNamespace();

	load_scene_json(true, "");

	TTF_Init();

	SDL_SetRenderDrawColor(renderer, clear_color_r, clear_color_g, clear_color_b, 255);

	SDL_RenderClear(renderer);
}

void GameStart::game_running() {
	std::string cur_operation;

	int cur_intro_index1 = 0;
	int cur_intro_index2 = 0;
	bool running = true;
	bool intro_part = true;


	int cur_frame = -180;

	bool first_load = true;
	bool ended = false;

	SDL_Init(SDL_INIT_GAMECONTROLLER);
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			std::cout << "Button A was pressed." << std::endl;
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (controller == nullptr) {
				SDL_Log("Could not open gamecontroller %i: %s", i, SDL_GetError());
			}
		}
	}

	while (true) {
		SDL_RenderClear(renderer);
		int cur_size = actors.size();

		Renderer::ui_to_be_render.reserve(cur_size);
		Renderer::image_to_be_render.reserve(cur_size);

		for (int i = 0; i < cur_size; i++) {
			Actor* actor = actors[i];
			if (!actor->components_to_be_added.empty() && actor->loaded) {
				for (auto componenet_instance : actor->components_to_be_added) {
					std::string key = componenet_instance.first;
					luabridge::LuaRef& component = componenet_instance.second;
					actor->componenet_instances.emplace(key, component);
					try {
						component["OnStart"](component);
					}
					catch (const luabridge::LuaException& e) {
						ReportError(actor->actor_name, e);
					}
				}
				actor->components_to_be_added.clear();
			}
		}

		cur_size = actors_loaded.size();

		for (int i = 0; i < cur_size; i++) {
			Actor* actor = actors_loaded[i];
			for (auto componenet_instance : actor->componenet_instances) {
				luabridge::LuaRef& component = componenet_instance.second;
				if (!component["enabled"]) continue;
				if (!component["OnStart"].isNil()) {
					try {
						component["OnStart"](component);
					}
					catch (const luabridge::LuaException& e) {
						ReportError(actor->actor_name, e);
					}
				}
			}
			actor->loaded = true;
		}

		if (actors_loaded.size() > cur_size) {
			std::vector<Actor*> tmp;
			for (int i = cur_size; i < actors_loaded.size(); i++) {
				tmp.push_back(actors_loaded[i]);
			}
			actors_loaded.clear();
			actors_loaded = tmp;
		}
		else {
			actors_loaded.clear();
		}

		SDL_Event e;

		bool scrolled = false;
		while (Helper::SDL_PollEvent498(&e)) {
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
				ended = true;
			}
			else if (e.type == SDL_MOUSEMOTION) {
				mouse_position.x = e.motion.x;
				mouse_position.y = e.motion.y;
			}
			else if (e.type == SDL_MOUSEWHEEL) {
				scroll_delta = e.wheel.preciseY;
				scrolled = true;

			}
			else if (e.type == SDL_CONTROLLERAXISMOTION) {
				if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
					int value = e.caxis.value;
					std::cout << "Left trigger value: " << value << std::endl;
				}
				else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
					int value = e.caxis.value;
					std::cout << "Left trigger value: " << value << std::endl;
				}
				else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
					left_x_axis = e.caxis.value;
				}
				else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
					left_y_axis = e.caxis.value;
				}
				else if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
					right_x_axis = e.caxis.value;
				}
				else if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
					right_y_axis = e.caxis.value;
				}
			}
			Input::ProcessEvent(e);
		}

		if (!scrolled) {
			scroll_delta = 0.0;
		}

		cur_size = actors.size();

		for (int i = 0; i < cur_size; i++) {
			Actor* actor = actors[i];
			for (auto componenet_instance : actor->componenet_instances) {
				luabridge::LuaRef& component = componenet_instance.second;
				if (!component["enabled"]) continue;
				if (!component["OnUpdate"].isNil()) {
					try {
						component["OnUpdate"](component);
					}
					catch (const luabridge::LuaException& e) {
						ReportError(actor->actor_name, e);
					}
				}
			}
		}

		for (int i = 0; i < cur_size; i++) {
			Actor* actor = actors[i];
			for (auto componenet_instance : actor->componenet_instances) {
				luabridge::LuaRef& component = componenet_instance.second;
				if (!component["enabled"]) continue;
				if (!component["OnLateUpdate"].isNil()) {
					try {
						component["OnLateUpdate"](component);
					}
					catch (const luabridge::LuaException& e) {
						ReportError(actor->actor_name, e);
					}
				}
			}
		}

		Renderer::Draw_All(camera_offset_x, camera_offset_y, x_resolution_screen, y_resolution_screen, zoom_factor);

		for (auto& actor : actor_to_be_deleted) {
			auto it = std::find(actors.begin(), actors.end(), actor);
			if (it != actors.end()) {
				actors.erase(it);
			}
			if (!actor->loaded) {
				auto it = std::find(actors_loaded.begin(), actors_loaded.end(), actor);
				if (it != actors_loaded.end()) {
					actors_loaded.erase(it);
				}
			}
		}

		for (auto& actor : actor_to_be_deleted) {
			delete actor;
		}

		actor_to_be_deleted.clear();
		Input::LateUpdate();

		Helper::SDL_RenderPresent498(renderer);

		if (ended) {
			TTF_CloseFont(font);
			TTF_Quit();

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elpased = end - GameStart::start;
			std::cout << "Time elpased: " << elpased.count() << " seconds." << std::endl;

			exit(0);
		}

		if (new_scene) {
			new_scene = false;

			int size1 = actors_loaded.size();
			int size2 = actors.size();
			for (int i = size1 - 1; i >= 0; i--) {
				bool found = false;
				for (auto aid : ActorDontDestroy) {
					if (actors_loaded[i]->id == aid) {
						found = true;
						break;
					}
				}
				if (!found) {
					actors_loaded.erase(actors_loaded.begin() + i);
				}
			}

			for (int i = size2 - 1; i >= 0; i--) {
				bool found = false;
				for (auto aid : ActorDontDestroy) {
					if (actors[i]->id == aid) {
						found = true;
						break;
					}
				}
				if (!found) {
					actors.erase(actors.begin() + i);
				}
			}

			load_scene_json(false, next_scene_name);
		}
	}
}