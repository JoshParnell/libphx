# LibPHX

LibPHX is a lightweight, (mostly) C-based game engine developed for the cancelled space simulation game **Limit Theory**. LibPHX focuses on implementing the core features necessary to build performant 3D games without including the kitchen sink. Simplicity, elegance, and minimalism are central to the design philosophy of the engine.

Rather than acting as a framework like most modern game engines, the LibPHX philosophy is to provide a game engine *as a library* -- that is, control flow is in the hands of the user, not the engine. LibPHX is designed specifically to be controlled from Lua scripts using the LuaJIT runtime. The engine's C interface, combined with LuaJIT's FFI technology, allows for scripts to make zero-overhead calls into the engine, thus allowing *the vast majority* of game logic and control to remain in script. Keeping control flow in the hands of Lua opens up an exciting world of fast iteration and fully-moddable game logic, all while achieving maximal performance by delegating heavy computation to engine-side functions.

### Supported Platforms
  - Windows
  - Linux
