local libphx = require('ffi.libphx').lib

function onDef_BSP (t, mt)
  t.Create = function (...)
    local e = libphx.Mesh_Validate(...)
    if e ~= 0 then
      print('BSP Incoming Mesh Error:')
      libphx.Error_Print(e)
    end
    local result = libphx.BSP_Create(...)
    return result
  end
end
