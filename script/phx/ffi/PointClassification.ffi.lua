-- PointClassification ---------------------------------------------------------
local ffi = require('ffi')
local PointClassification

do -- Global Symbol Table
  PointClassification = {
    InFront  = 1,
    Behind   = 2,
    Coplanar = 3,
  }

  if onDef_PointClassification then onDef_PointClassification(PointClassification, mt) end
  PointClassification = setmetatable(PointClassification, mt)
end

return PointClassification
