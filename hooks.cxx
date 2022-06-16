#include <nlohmann/json.hpp>
 
#include "hooks.hxx"

void from_json(const nlohmann::json& j, OBS_hook& p) {
    j.at("source_name").get_to(p.source_name);
    j.at("obs_action").get_to(p.obs_action);
    j.at("hook_name").get_to(p.hook_name);    
}

void from_json(const nlohmann::json& j, Midi_out_hook& p) {
    j.at("midi_message").get_to(p.midi_message);
    j.at("hook_name").get_to(p.hook_name);    
}

void from_json(const nlohmann::json& j, Midi_in_hook& p) {
    j.at("midi_message").get_to(p.midi_message);
    j.at("hook_name").get_to(p.hook_name);    
}