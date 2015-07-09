#include "Nooskewl_Engine/load_dll.h"
#include "Nooskewl_Engine/module.h"
#include "Nooskewl_Engine/util.h"

void load_dll()
{
	List_Directory ld(".\\*.dll");
	std::string filename;

	while ((filename = ld.next()) != "") {
		HINSTANCE hGetProcIDDLL = LoadLibrary(filename.c_str());

		if (!hGetProcIDDLL) {
			throw FileNotFoundError("Couldn't load game.dll!");
		}

		m.get_map_logic = (Map_Logic_Getter)GetProcAddress(hGetProcIDDLL, "get_map_logic");
		if (m.get_map_logic != NULL) {
			return;
		}
	}

	throw FileNotFoundError("Couldn't find a game DLL!");
}