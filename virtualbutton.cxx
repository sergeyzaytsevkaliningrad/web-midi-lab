#include "virtualbutton.hxx"

#include "plugin.hxx"

void from_json(const nlohmann::json& j, VirtualButton& p) {
    
    j.at("name").get_to(p.name);

    j.at("midi_in_high_hooks")  .get_to(p.midi_in_high_hooks);
    j.at("obs_in_high_hooks")   .get_to(p.obs_in_high_hooks);
    j.at("midi_out_high_hooks") .get_to(p.midi_out_high_hooks);
    j.at("obs_out_high_hooks")  .get_to(p.obs_out_high_hooks);
    j.at("midi_in_low_hooks")   .get_to(p.midi_in_low_hooks);
    j.at("obs_in_low_hooks")    .get_to(p.obs_in_low_hooks);
    j.at("midi_out_low_hooks")  .get_to(p.midi_out_low_hooks);
    j.at("obs_out_low_hooks")   .get_to(p.obs_out_low_hooks);

}