#ifndef PHX_Bytes
#define PHX_Bytes

#include "Common.h"

/* --- Bytes -------------------------------------------------------------------
 *
 *   A generic memory buffer for reading/writing. Limited to 32-bit capacity.
 *   Bytes* is a contiguous header + raw memory pair, not a header +
 *   pointer-to-memory (i.e., it is 'inline').
 *
 *   Bytes_FromData : Equivalent to Create(len) -> Write(data, len) -> Rewind()
 *                  : Client is still responsible for freeing data; Bytes
 *                    makes a copy, it does not take ownership of existing
 *                    data
 *   Bytes_Rewind   : Equivalent to SetCursor(0)
 *   Bytes_Read*    : Reads in binary
 *                  : Advances internal cursor by size of type being read
 *   Bytes_Write*   : Writes in binary
 *                  : Advances internal cursor by size of type being written
 *   Bytes_WriteStr : Does not write null-terminator
 *                  : Advances internal cursor by strlen(data)
 *
 * -------------------------------------------------------------------------- */

PHX_API Bytes*  Bytes_Create       (uint32 len);
PHX_API Bytes*  Bytes_FromData     (void const* data, uint32 len);
PHX_API Bytes*  Bytes_Load         (cstr path);
PHX_API void    Bytes_Free         (Bytes*);

PHX_API void*   Bytes_GetData      (Bytes*);
PHX_API uint32  Bytes_GetSize      (Bytes*);

PHX_API Bytes*  Bytes_Compress     (Bytes*);
PHX_API Bytes*  Bytes_Decompress   (Bytes*);

PHX_API uint32  Bytes_GetCursor    (Bytes*);
PHX_API void    Bytes_Rewind       (Bytes*);
PHX_API void    Bytes_SetCursor    (Bytes*, uint32);

PHX_API void    Bytes_Read         (Bytes*, void* data, uint32 len);
PHX_API uint8   Bytes_ReadU8       (Bytes*);
PHX_API uint16  Bytes_ReadU16      (Bytes*);
PHX_API uint32  Bytes_ReadU32      (Bytes*);
PHX_API uint64  Bytes_ReadU64      (Bytes*);
PHX_API int8    Bytes_ReadI8       (Bytes*);
PHX_API int16   Bytes_ReadI16      (Bytes*);
PHX_API int32   Bytes_ReadI32      (Bytes*);
PHX_API int64   Bytes_ReadI64      (Bytes*);
PHX_API float   Bytes_ReadF32      (Bytes*);
PHX_API double  Bytes_ReadF64      (Bytes*);

PHX_API void    Bytes_Write        (Bytes*, void const* data, uint32 len);
PHX_API void    Bytes_WriteStr     (Bytes*, cstr data);
PHX_API void    Bytes_WriteU8      (Bytes*, uint8);
PHX_API void    Bytes_WriteU16     (Bytes*, uint16);
PHX_API void    Bytes_WriteU32     (Bytes*, uint32);
PHX_API void    Bytes_WriteU64     (Bytes*, uint64);
PHX_API void    Bytes_WriteI8      (Bytes*, int8);
PHX_API void    Bytes_WriteI16     (Bytes*, int16);
PHX_API void    Bytes_WriteI32     (Bytes*, int32);
PHX_API void    Bytes_WriteI64     (Bytes*, int64);
PHX_API void    Bytes_WriteF32     (Bytes*, float);
PHX_API void    Bytes_WriteF64     (Bytes*, double);

PHX_API void    Bytes_Print        (Bytes*);
PHX_API void    Bytes_Save         (Bytes*, cstr path);

#endif
