#include "src/EditorEngine.h"

int main(int, char**)
{
	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 

	auto& editor = clcEngine;

	if (!editor.Initialize())
		return 1;

	editor.Run();
	editor.Shutdown();

	return 0;
}
