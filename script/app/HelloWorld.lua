package.path = package.path .. ';./script/?.lua'
package.path = package.path .. ';./script/?.ext.lua'
package.path = package.path .. ';./script/?.ffi.lua'

local ffi = require('ffi')
require('phx.libphx')
local Engine = require('phx.ffi.Engine')

Engine.Init(2, 1)
print(Engine.GetBits())
print(ffi.string(Engine.GetVersion()))
Engine.Free()
