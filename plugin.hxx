#pragma once

#include <obs-frontend-api.h>
#include <obs.hpp>

#include <libremidi/libremidi.hpp>

#include <nlohmann/json.hpp>

#include "hooks.hxx"
#include "virtualbutton.hxx"

#include <iostream>
#include <string>
#include <unordered_map>
#include <set>

template<typename T> T *calldata_get_pointer(const calldata_t *data, const char *name)
{
	void *ptr = nullptr;
	calldata_get_ptr(data, name, &ptr);
	return reinterpret_cast<T *>(ptr);
}

class Plugin {

public:
    static Plugin& getInstance();

    std::unordered_map<std::string, Midi_in_hook>   midi_in_hooks_map;
    std::unordered_map<std::string, Midi_out_hook>  midi_out_hooks_map;
    std::unordered_map<std::string, OBS_hook>       obs_hooks_map;
    
    std::unordered_map<std::string, VirtualButton>  virtual_buttons_map;

private:

    Plugin(Plugin const&)               = delete;
    void operator=(Plugin const&)       = delete;

	Plugin();

    unsigned int port;

	libremidi::midi_in m_in;
	libremidi::midi_out m_out;

    static void connectSourceSignals    (obs_source_t *source);
    static void disconnectSourceSignals (obs_source_t *source);

    static void MIDI_handler(const libremidi::message& message);

    static void FROMOBS_OnSourceMuteStateChange (void *param, calldata_t *data);
    static void FROMOBS_OnSourceCreate          (void *param, calldata_t *data);

    static void processVirtualButtonOut (std::string name);

};

