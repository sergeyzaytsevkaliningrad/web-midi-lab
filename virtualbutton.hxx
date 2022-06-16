#pragma once

#include <nlohmann/json.hpp>

#include <string>

#include "hooks.hxx"

class VirtualButton{
public:
    std::string name;

    bool state{false};

    std::vector<std::string> obs_in_low_hooks;
    std::vector<std::string> obs_in_high_hooks;
    std::vector<std::string> obs_out_low_hooks;
    std::vector<std::string> obs_out_high_hooks;

    std::vector<std::string> midi_in_low_hooks;
    std::vector<std::string> midi_in_high_hooks;
    
    std::vector<std::string> midi_out_low_hooks;
    std::vector<std::string> midi_out_high_hooks;
};

void from_json(const nlohmann::json& j, VirtualButton& p);