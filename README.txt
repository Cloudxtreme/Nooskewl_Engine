Nooskewl Engine is a C++ 2D JRPG engine using SDL. It is being developed for the
Nooskewl game Monster RPG 3.  Great effort is being made to make the engine
reusable for other games. Game-specifics are all done in your game project,
while everything you need to build on is done in the engine.

The engine is still in the early stages and there is no battle system yet.

Nooskewl Engine is modular: you need 3 DLLs plus an executable for a complete
game. The DLLs are TGUI3, Nooskewl Engine and your game. The executable simply
sets things in motion and is included as part of Nooskewl Engine.

Your game's DLL has a few hooks like dll_get_map_logic, dll_load_brain and
dll_choose_action.

Map_Logic is a class in Nooskewl Engine which has several methods that get
called as a map runs, which you can use to add custom behaviour to your maps.

Brain is another Nooskewl_Engine class that gives entities their behaviour. You
use classes derived from Brain to give your entities custom behaviour.

dll_choose_action allows you to implement a custom actions menu bound to the
tab key. Monster RPG 3 uses this to choose actions like Talk, Collect, Pick
Pocket and Attack.

The full list of hooks are:

	dll_start - called at program launch
	dll_end - called at program end
	dll_game_started - called when a game is loaded or a new game started
	dll_pause - this is your pause menu
	dll_get_map_logic - mentioned above
	dll_get_brain - mentioned above
	dll_choose_action - mentioned above

The game reads its data from a CPA (Crystal Picnic Archive) file. CPA is a
compressed file with an index and the data concatenated together and can be
created by the mkcpa* scripts in the tools/ directory of Nooskewl Engine.

You can name your game DLL and CPA anything you want and the engine will pick it
up if its in the current directory.

See the Monster RPG 3 repo for an example game implementation using Nooskewl
Engine.
