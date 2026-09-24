// Spinblade
#include <iostream>
#include "GameEngine.h"

int main() {
	auto& engine = clcEngine;
	
	engine.Initialize();
	engine.Run();
	engine.Shutdown();

	return 0;
}