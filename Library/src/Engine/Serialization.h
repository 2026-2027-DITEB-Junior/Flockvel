#pragma once
#include "../API.h"
#include <string>
#include <nlohmann/json.hpp>
#include "AEXMath.h"
#include "Transform.h"

namespace clc {
	using json = nlohmann::json;

	class IBase;
	struct Color;

	API bool ReadJsonFile(const std::string& filename, json& j);
	API bool WriteJsonFile(const std::string& filename, const json& j);

	// wrappers that serialize/deserialize any ibase directly to/from a json file
	API bool SerializeToFile(const std::string& filename, const IBase& obj);
	API bool DeserializeFromFile(const std::string& filename, IBase& obj);

	API void to_json(nlohmann::json& j, const Transform& t);
	API void from_json(const nlohmann::json& j, Transform& t);

	API void to_json(nlohmann::json& j, const glm::vec2& v);
	API void from_json(const nlohmann::json& j, glm::vec2& v);

	API void to_json(nlohmann::json& j, const glm::vec3& v);
	API void from_json(const nlohmann::json& j, glm::vec3& v);

	API void to_json(nlohmann::json& j, const glm::vec4& v);
	API void from_json(const nlohmann::json& j, glm::vec4& v);

	API void to_json(nlohmann::json& j, const aex::Quaternion& q);
	API void from_json(const nlohmann::json& j, aex::Quaternion& q);

	API void to_json(nlohmann::json& j, const Color& c);
	API void from_json(const nlohmann::json& j, Color& c);
}

// nlohmann only looks in the type namespace -> send these to the clc functions
namespace nlohmann {
	template <> struct adl_serializer<glm::vec2> {
		static void to_json(json& j, const glm::vec2& v) { clc::to_json(j, v); }
		static void from_json(const json& j, glm::vec2& v) { clc::from_json(j, v); }
	};

	template <> struct adl_serializer<glm::vec3> {
		static void to_json(json& j, const glm::vec3& v) { clc::to_json(j, v); }
		static void from_json(const json& j, glm::vec3& v) { clc::from_json(j, v); }
	};

	template <> struct adl_serializer<glm::vec4> {
		static void to_json(json& j, const glm::vec4& v) { clc::to_json(j, v); }
		static void from_json(const json& j, glm::vec4& v) { clc::from_json(j, v); }
	};

	template <> struct adl_serializer<aex::Quaternion> {
		static void to_json(json& j, const aex::Quaternion& q) { clc::to_json(j, q); }
		static void from_json(const json& j, aex::Quaternion& q) { clc::from_json(j, q); }
	};
}
