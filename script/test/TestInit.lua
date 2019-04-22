package.path = package.path .. ';./script/?.lua'

local ffi = require('ffi')
local Engine = require('ffi.Engine')

Engine.Init(2, 1)
print("Phoenix Engine Initialized:")
print(string.format(" > x86-%d", Engine.GetBits()))
print(string.format(" > Compiled on %s", ffi.string(Engine.GetVersion())))
Engine.Free()
