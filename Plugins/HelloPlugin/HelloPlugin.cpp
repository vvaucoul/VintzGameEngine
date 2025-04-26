#include "../../Engine/CorePlugins/PluginAPI.h"
#include <iostream>

extern "C" {

PLUGIN_API void Plugin_Init(void * /*engineContext*/) {
	std::cout << "[HelloPlugin] Hello from HelloPlugin!" << std::endl;
}

PLUGIN_API void Plugin_Update(float /*deltaTime*/) {
	// No per-frame behavior
}

PLUGIN_API void Plugin_Shutdown() {
	std::cout << "[HelloPlugin] Goodbye from HelloPlugin." << std::endl;
}
}
