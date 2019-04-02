-- CollisionMask ---------------------------------------------------------------
local ffi = require('ffi')
local libphx = require('libphx.libphx')
local CollisionMask

do -- Global Symbol Table
  CollisionMask = {
  }

  if onDef_CollisionMask then onDef_CollisionMask(CollisionMask, mt) end
  CollisionMask = setmetatable(CollisionMask, mt)
end

return CollisionMask
