#ifndef PHX_DataFormat
#define PHX_DataFormat

#include "Common.h"

/* --- DataFormat --------------------------------------------------------------
 *
 *   DataFormat_GetSize : Size in bytes of single element
 *
 * -------------------------------------------------------------------------- */

PHX_API const DataFormat DataFormat_U8;
PHX_API const DataFormat DataFormat_I8;
PHX_API const DataFormat DataFormat_U16;
PHX_API const DataFormat DataFormat_I16;
PHX_API const DataFormat DataFormat_U32;
PHX_API const DataFormat DataFormat_I32;
PHX_API const DataFormat DataFormat_Float;

PHX_API int  DataFormat_GetSize  (DataFormat);

#endif
