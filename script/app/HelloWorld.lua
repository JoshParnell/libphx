package.path = package.path .. ';./script/?.lua'
package.path = package.path .. ';./script/?.ext.lua'

local ffi = require('ffi')
local Engine = require('libphx.Engine')

Engine.Init(2, 1)
print(Engine.GetBits())
print(ffi.string(Engine.GetVersion()))
Engine.Free()
