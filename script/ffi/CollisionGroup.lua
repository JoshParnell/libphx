-- CollisionGroup --------------------------------------------------------------
local ffi = require('ffi')
local libphx = require('ffi.libphx')
local CollisionGroup

do -- Global Symbol Table
  CollisionGroup = {
  }

  if onDef_CollisionGroup then onDef_CollisionGroup(CollisionGroup, mt) end
  CollisionGroup = setmetatable(CollisionGroup, mt)
end

return CollisionGroup
