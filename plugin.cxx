#include "plugin.hxx"

#include "hooks.hxx"
#include "virtualbutton.hxx"
#include <obs.hpp>

#include <fstream>
#include <regex>

void Plugin::MIDI_handler(const libremidi::message& message){
    Plugin &plugin = Plugin::getInstance();

    std::string str = "";

    for(int i = 0; i < message.bytes.size(); ++i){
        if(i) str += " ";
        str += std::to_string((int) message.bytes[i]);
    }

    std::cout << "Received message from MIDI_IN: " << str << std::endl;

    for(auto button_pair : plugin.virtual_buttons_map){
        auto button = button_pair.second;

        if(button.state){
            for(auto midi_hook_name : button.midi_in_low_hooks){
                auto midi_hook = plugin.midi_in_hooks_map[midi_hook_name];
                if(midi_hook.midi_message != str) continue;

                plugin.virtual_buttons_map[button.name].state = false;
                plugin.processVirtualButtonOut(button.name);

                break;
            }
        }
        else{
            for(auto midi_hook_name : button.midi_in_high_hooks){
                auto midi_hook = plugin.midi_in_hooks_map[midi_hook_name];
                if(midi_hook.midi_message != str) continue;

                plugin.virtual_buttons_map[button.name].state = true;
                plugin.processVirtualButtonOut(button.name);

                break;
            }
        }
        
    }
}

void Plugin::processVirtualButtonOut (std::string name){
    Plugin &plugin = Plugin::getInstance();
    VirtualButton &button = plugin.virtual_buttons_map[name];

    if(button.state){
        for (auto midi_hook_name : button.midi_out_high_hooks){

            std::vector<unsigned char> bytes;

            auto midi_hook = plugin.midi_out_hooks_map[midi_hook_name];

            std::string message = midi_hook.midi_message;
            std::string delim = " ";

            auto start = 0U;
            auto end = message.find(delim);
            while (end != std::string::npos)
            {
                bytes.push_back(std::stoi(message.substr(start, end - start)));
                start = end + delim.length();
                end = message.find(delim, start);
            }
            bytes.push_back(std::stoi(message.substr(start, end)));

            plugin.m_out.send_message(bytes);
        }    

        for (auto obs_hook_name : button.obs_out_high_hooks){
            auto obs_hook = plugin.obs_hooks_map[obs_hook_name];

            OBSSource source = obs_get_source_by_name(obs_hook.source_name.c_str());

            if (obs_hook.obs_action == "mute_on"){
                bool already_muted = obs_source_muted(source);
                if(!already_muted) obs_source_set_muted(source, true);
            }
            else if (obs_hook.obs_action == "mute_off"){
                bool already_muted = obs_source_muted(source);
                if(already_muted) obs_source_set_muted(source, false);
            }
            else {
                //TODO
            }
        }
    }
    else{
        for (auto midi_hook_name : button.midi_out_low_hooks){

            std::vector<unsigned char> bytes;

            auto midi_hook = plugin.midi_out_hooks_map[midi_hook_name];

            std::string message = midi_hook.midi_message;
            std::string delim = " ";

            auto start = 0U;
            auto end = message.find(delim);
            while (end != std::string::npos)
            {
                bytes.push_back(std::stoi(message.substr(start, end - start)));
                start = end + delim.length();
                end = message.find(delim, start);
            }
            bytes.push_back(std::stoi(message.substr(start, end)));

            plugin.m_out.send_message(bytes);
        }    

        for (auto obs_hook_name : button.obs_out_low_hooks){
            auto obs_hook = plugin.obs_hooks_map[obs_hook_name];

            OBSSource source = obs_get_source_by_name(obs_hook.source_name.c_str());

            if (obs_hook.obs_action == "mute_on"){
                bool already_muted = obs_source_muted(source);
                if(!already_muted) obs_source_set_muted(source, true);
            }
            else if (obs_hook.obs_action == "mute_off"){
                bool already_muted = obs_source_muted(source);
                if(already_muted) obs_source_set_muted(source, false);
            }
            else {
                //TODO
            }
        }
    }
}

void Plugin::FROMOBS_OnSourceCreate(void *param, calldata_t *data)
{
    OBSSource source = calldata_get_pointer<obs_source_t>(data, "source");
    if (!source) {
        return;
    }   
    Plugin::connectSourceSignals(source);
}

void Plugin::FROMOBS_OnSourceMuteStateChange (void *param, calldata_t *data)
{
    OBSSource source = calldata_get_pointer<obs_source_t>(data, "source");
    if (!source) {
        return;
    }

    std::string source_name = obs_source_get_name(source);

    Plugin &plugin = Plugin::getInstance();

    bool muted;
	calldata_get_bool(data, "muted", &muted);
    std::string checked = muted ? "mute_on" : "mute_off";


    for(auto button_pair : plugin.virtual_buttons_map){
        auto button = button_pair.second;

        if(button.state){
            for(auto obs_hook_name : button.obs_in_low_hooks){
                auto obs_hook = plugin.obs_hooks_map[obs_hook_name];
                if(obs_hook.source_name != source_name) continue;
                if(obs_hook.obs_action != checked) continue;

                plugin.virtual_buttons_map[button.name].state = false;
                plugin.processVirtualButtonOut(button.name);

                break;
            }
        }
        else{
            for(auto obs_hook_name : button.obs_in_high_hooks){
                auto obs_hook = plugin.obs_hooks_map[obs_hook_name];
                if(obs_hook.source_name != source_name) continue;
                if(obs_hook.obs_action != checked) continue;

                plugin.virtual_buttons_map[button.name].state = true;
                plugin.processVirtualButtonOut(button.name);

                break;
            }
        }
        
    }
}

void Plugin::connectSourceSignals(obs_source_t *source)
{
    if (!source) {
        return;
    }
    disconnectSourceSignals(source);
    obs_source_type sourceType = obs_source_get_type(source);
    signal_handler_t * handler = obs_source_get_signal_handler(source);
    signal_handler_connect(handler, "mute", FROMOBS_OnSourceMuteStateChange, nullptr);
}

void Plugin::disconnectSourceSignals(obs_source_t *source)
{
    if (!source) {
        return;
    }
    signal_handler_t *sh = obs_source_get_signal_handler(source);
    signal_handler_disconnect(sh, "mute", FROMOBS_OnSourceMuteStateChange, nullptr);
}

Plugin& Plugin::getInstance()
{
    static Plugin instance;
    return instance;
}

Plugin::Plugin() {    
    std::fstream settingsFile;
    
    std::string filePath = "./obs-midi-miem_settings.json";
    settingsFile.open(filePath, std::fstream::in);
    
    if(!settingsFile){
        std::cout << "Settings file not found!" << std::endl << "Creating empty one..." << std::endl;
        settingsFile.close();

        settingsFile.open(filePath,  std::fstream::out | std::fstream::trunc);
        settingsFile.close();

        std::cout << "Now specify settings and reload OBS!";

        system("pause");

        exit(1);
    }
    else{
        std::cout << "Settings file found!" << std::endl << "Trying to load settings..." << std::endl;

        std::string jsonStr;

        settingsFile.seekg(0, std::ios::end);   
        jsonStr.reserve(settingsFile.tellg());
        settingsFile.seekg(0, std::ios::beg);

        jsonStr.assign((std::istreambuf_iterator<char>(settingsFile)),
                    std::istreambuf_iterator<char>());

        std::cout << jsonStr << std::endl;

        nlohmann::json json = nlohmann::json::parse(jsonStr);
        
        std::vector<Midi_in_hook> midi_in_hooks             = json["midi_in_hooks"].get<std::vector<Midi_in_hook>>();
        std::vector<Midi_out_hook> midi_out_hooks           = json["midi_out_hooks"].get<std::vector<Midi_out_hook>>();
        std::vector<OBS_hook> obs_hooks                     = json["obs_hooks"].get<std::vector<OBS_hook>>();
        std::vector<VirtualButton> virtual_button_jsons     = json["virtual_buttons"].get<std::vector<VirtualButton>>();


        for(auto hook : midi_in_hooks)
            midi_in_hooks_map.insert(std::pair<std::string, Midi_in_hook>(hook.hook_name, hook));

        for(auto hook : midi_out_hooks)
            midi_out_hooks_map.insert(std::pair<std::string, Midi_out_hook>(hook.hook_name, hook));
        
        for(auto hook : obs_hooks)
            obs_hooks_map.insert(std::pair<std::string, OBS_hook>(hook.hook_name, hook));

        for(auto button_json : virtual_button_jsons)
            virtual_buttons_map.insert(std::pair<std::string, VirtualButton>(button_json.name, button_json));

        obs_enum_sources(
		[](void *param, obs_source_t *source) {
			Plugin::connectSourceSignals(source);
			return true;
		},
		this);
        signal_handler_t *coreSignalHandler = obs_get_signal_handler();
        if (coreSignalHandler) {
            signal_handler_connect(coreSignalHandler, "source_create",
                        Plugin::FROMOBS_OnSourceCreate, this);
        }

        int port_in, port_out; 

        std::cout << "To proceed select one of input device ids:" << std::endl;
        
        for (int i = 0, N = m_in.get_port_count(); i < N; i++) {
            std::cout << "ID: " << i << " description:" << m_in.get_port_name(i) << std::endl;
        }

        std::cin >> port_in; 
        std::cout << std::endl;

        std::cout << "To proceed select one of output device ids:" << std::endl;

        for (int i = 0, N = m_out.get_port_count(); i < N; i++) {
            std::cout << "ID: " << i << " description:" << m_out.get_port_name(i) << std::endl;
        }

        std::cin >> port_out;
        std::cout << std::endl;

        m_in.open_port(port_in);
        m_out.open_port(port_out);

        m_in.set_callback(Plugin::MIDI_handler);


    }

}
