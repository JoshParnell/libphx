# LibPHX

LibPHX is a lightweight, (mostly) C-based game engine developed for the cancelled space simulation game Limit Theory. LibPHX focuses on implementing the core features necessary to build performant 3D games without including the kitchen sink.

Rather than acting as a framework like most modern game engines, the LibPHX philosophy is to provide a game engine *as a library* -- that is, control flow is in the hands of the user, not the engine. LibPHX is designed specifically to be controlled from Lua scripts using the LuaJIT runtime. The engine's C interface, combined with LuaJIT's FFI technology, allows for scripts to make zero-overhead calls into the engine, thus allowing *the majority* of game logic and control to remain in script. Keeping control flow in the hands of Lua opens up a world of fast iteration and fully-moddable game logic, while achieving maximal performance by delegating heavy computation to engine-side functions.

LibPHX is now abandoned and mostly undocumented, however you can see examples of its usage in https://github.com/JoshParnell/ltheory.
