// If you are looking for <windows.h>, it's in Engine.h
// #include "Engine/Engine.h"

//Editor
#include "EditorEngine.h"

int main() {
	auto& engine = clcEngine;

	engine.Initialize();
	engine.Run();
	engine.Shutdown();

	return 0;
}