#pragma once
#include "StringHash.hpp"
#include "Color.hpp"
#include "../../nlohmann/json.hpp"
#include <glm/glm.hpp>

namespace hd {

using JSON = nlohmann::json;

void from_json(const JSON &json, StringHash &data);
void from_json(const JSON &json, Color4 &data);

void to_json(JSON &json, const StringHash &data);
void to_json(JSON &json, const Color4 &data);

}

namespace glm {

void to_json(hd::JSON &json, const vec2 &data);
void to_json(hd::JSON &json, const ivec2 &data);
void to_json(hd::JSON &json, const vec3 &data);
void to_json(hd::JSON &json, const ivec3 &data);
void to_json(hd::JSON &json, const vec4 &data);
void to_json(hd::JSON &json, const ivec4 &data);

void from_json(const hd::JSON &json, vec2 &data);
void from_json(const hd::JSON &json, ivec2 &data);
void from_json(const hd::JSON &json, vec3 &data);
void from_json(const hd::JSON &json, ivec3 &data);
void from_json(const hd::JSON &json, vec4 &data);
void from_json(const hd::JSON &json, ivec4 &data);

}
