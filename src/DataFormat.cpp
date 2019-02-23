#include "DataFormat.h"
#include "OpenGL.h"

DataFormat const DataFormat_U8    = GL_UNSIGNED_BYTE;
DataFormat const DataFormat_I8    = GL_BYTE;
DataFormat const DataFormat_U16   = GL_UNSIGNED_SHORT;
DataFormat const DataFormat_I16   = GL_SHORT;
DataFormat const DataFormat_U32   = GL_UNSIGNED_INT;
DataFormat const DataFormat_I32   = GL_INT;
DataFormat const DataFormat_Float = GL_FLOAT;

int DataFormat_GetSize (DataFormat self) {
  switch (self) {
    case DataFormat_U8:
    case DataFormat_I8:
      return 1;
    case DataFormat_U16:
    case DataFormat_I16:
      return 2;
    case DataFormat_U32:
    case DataFormat_I32:
    case DataFormat_Float:
      return 4;
  }
  return 0;
}
