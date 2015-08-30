Nooskewl Engine is a C++ 2D JRPG engine using SDL. It is being developed for
the Nooskewl game Monster RPG 3.  Great effort is being made to make the
engine reusable for other games. Game-specifics are all done in your game
project, while everything you need to build on is done in the engine.

The engine is still in the early stages and there is no battle system yet.

Nooskewl Engine is modular: you need 3 DLLs plus an executable for a complete
game. The DLLs are TGUI3, Nooskewl Engine and your game, while the executable
simply sets things in motion and is included as part of Nooskewl Engine.

Your game's DLL has two (currently) hooks: dll_get_map_logic and
dll_load_brain. Map_Logic is a class in Nooskewl Engine which has several
methods that get called as a map runs, which you can use to add custom
behaviour to your maps. Brain is another Nooskewl_Engine class that gives
entities their behaviour. You use classes derived from Brain to give your
entities custom behaviour.

The DLL also has dll_start, dll_end and dll_pause functions that get called
at the appropriate times. dll_start and dll_end are called when the game
starts and exits, respectively. dll_pause is called any time the user
activates the pause menu (ESCAPE key from a map screen) so you can include
a custom pause screen.

The game reads its data from a CPA (Crystal Picnic Archive) file. CPA is a
compressed file with an index and the data concatenated together and can be
created by the mkcpa* scripts in the tools/ directory of Nooskewl Engine.

You can name your game DLL and CPA anything you want and the engine will pick
it up if its in the current directory.

See the Monster RPG 3 repo for an example game implementation using Nooskewl
Engine.
