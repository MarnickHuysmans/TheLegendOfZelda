#include "pch.h"
#include "Core.h"
#include <ctime>
#include "Vector2f.h"

void StartHeapControl();
void DumpMemoryLeaks();

int main(int argc, char* argv[])
{
	srand(static_cast<unsigned int>(time(nullptr)));
	{
		StartHeapControl();

		Vector2f window{ 256.0f, 224.0f };
		float scale{ 3.0f };
		window *= scale;

		Core core{ Window{ "The Legend of Zelda - Huysmans, Marnick - 1DAE18", window.x, window.y }, scale };
		core.Run();
	}
	DumpMemoryLeaks();
	return 0;
}


void StartHeapControl()
{
#if defined(DEBUG) | defined(_DEBUG)
	// Notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Report detected leaks when the program exits
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Set a breakpoint on the specified object allocation order number
	//_CrtSetBreakAlloc( 3072 );
#endif
}

void DumpMemoryLeaks()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}


