#include "pch.h"
#include "Serialization.h"
#include "IBase.h"
#include "Factory.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Transform.h"
#include "../Graphics/GL/AEXColor.h"
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

namespace clc {
	bool ReadJsonFile(const std::string& filename, json& j) {
		std::ifstream file(std::filesystem::u8path(filename));
		if (!file.is_open()) {
			std::cout << "cant read: " << filename << "\n";
			return false;
		}
		j = json::parse(file, nullptr, false); // with the 3rd param it does not throw and gives a discarded value instead if fail
		if (j.is_discarded()) {
			std::cout << "bad json: " << filename << "\n";
			return false;
		}
		return true;
	}

	bool WriteJsonFile(const std::string& filename, const json& j) {
		std::ofstream file(std::filesystem::u8path(filename));
		if (!file.is_open()) {
			std::cout << "cant write: " << filename << "\n";
			return false;
		}
		file << j.dump(2); // optional param for the json "sangria" of 2 spaces, otherwise one line
		file.flush();
		return file.good();
	}

	bool SerializeToFile(const std::string& filename, const IBase& obj) {
		json j;
		obj.Serialize(j);
		return WriteJsonFile(filename, j);
	}

	bool DeserializeFromFile(const std::string& filename, IBase& obj) {
		json j;
		if (!ReadJsonFile(filename, j)) return false;
		obj.Deserialize(j);
		return true;
	}

	// transform
	void to_json(nlohmann::json& j, const Transform& t) {
		j["position"] = t.mPosition;
		j["rotation"] = t.mRotation; // euler angles in radians
		j["scale"] = t.mScale;
	}

	void from_json(const nlohmann::json& j, Transform& t) {
		// missing fields keep the default value instead of throwing
		if (j.contains("position")) j["position"].get_to(t.mPosition);
		if (j.contains("rotation")) j["rotation"].get_to(t.mRotation);
		if (j.contains("scale")) j["scale"].get_to(t.mScale);
	}

	// vec2
	void to_json(nlohmann::json& j, const glm::vec2& v) {
		j["x"] = v.x;
		j["y"] = v.y;
	}

	void from_json(const nlohmann::json& j, glm::vec2& v) {
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
	}

	// vec3
	void to_json(nlohmann::json& j, const glm::vec3& v) {
		j["x"] = v.x;
		j["y"] = v.y;
		j["z"] = v.z;
	}

	void from_json(const nlohmann::json& j, glm::vec3& v) {
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
		j.at("z").get_to(v.z);
	}

	// vec4
	void to_json(nlohmann::json& j, const glm::vec4& v) {
		j["x"] = v.x;
		j["y"] = v.y;
		j["z"] = v.z;
		j["w"] = v.w;
	}

	void from_json(const nlohmann::json& j, glm::vec4& v) {
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
		j.at("z").get_to(v.z);
		j.at("w").get_to(v.w);
	}

	// quaternion
	void to_json(nlohmann::json& j, const aex::Quaternion& q) {
		j["x"] = q.x;
		j["y"] = q.y;
		j["z"] = q.z;
		j["w"] = q.w;
	}

	void from_json(const nlohmann::json& j, aex::Quaternion& q) {
		j.at("x").get_to(q.x);
		j.at("y").get_to(q.y);
		j.at("z").get_to(q.z);
		j.at("w").get_to(q.w);
	}

	// color
	void to_json(nlohmann::json& j, const Color& c) {
		j["r"] = c.r;
		j["g"] = c.g;
		j["b"] = c.b;
		j["a"] = c.a;
	}

	void from_json(const nlohmann::json& j, Color& c) {
		j.at("r").get_to(c.r);
		j.at("g").get_to(c.g);
		j.at("b").get_to(c.b);
		j.at("a").get_to(c.a);
	}
}
