#pragma once

#include <string>

class Midi_in_hook{
public:
    std::string midi_message;
    std::string hook_name;
};

void from_json(const nlohmann::json& j, Midi_in_hook& p);


class Midi_out_hook{
public:
    std::string midi_message;
    std::string hook_name;
};

void from_json(const nlohmann::json& j, Midi_out_hook& p);


class OBS_hook{
public:
    std::string source_name;
    std::string obs_action;
    std::string hook_name;
};

void from_json(const nlohmann::json& j, OBS_hook& p);