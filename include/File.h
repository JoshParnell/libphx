#ifndef PHX_File
#define PHX_File

#include "Common.h"

/* --- File --------------------------------------------------------------------
 *
 *   File_Create   : Overwrites existing file if present
 *   File_Open     : Appends to existing file if present
 *   File_Read     : Returns 0 if opening file fails
 *                 : Returns 0 if file is empty
 *                 : Throws Fatal if filesize exceeds 2^32 - 1
 *                   (Bytes* does not support 64-bit capacities.)
 *   File_Size     : Returns -1 if opening file fails
 *   File_Write*   : Write binary data, not ascii
 *   File_WriteStr : Does NOT write null-terminator
 *
 * -------------------------------------------------------------------------- */

PHX_API File*   File_Create     (cstr path);
PHX_API File*   File_Open       (cstr path);
PHX_API void    File_Close      (File*);

PHX_API bool    File_Exists     (cstr path);
PHX_API bool    File_IsDir      (cstr path);
PHX_API Bytes*  File_ReadBytes  (cstr path);
PHX_API cstr    File_ReadCstr   (cstr path);
PHX_API int64   File_Size       (cstr path);

PHX_API void    File_Read       (File*, void* data, uint32 len);
PHX_API uint8   File_ReadU8     (File*);
PHX_API uint16  File_ReadU16    (File*);
PHX_API uint32  File_ReadU32    (File*);
PHX_API uint64  File_ReadU64    (File*);
PHX_API int8    File_ReadI8     (File*);
PHX_API int16   File_ReadI16    (File*);
PHX_API int32   File_ReadI32    (File*);
PHX_API int64   File_ReadI64    (File*);
PHX_API float   File_ReadF32    (File*);
PHX_API double  File_ReadF64    (File*);

PHX_API void    File_Write      (File*, void const* data, uint32 len);
PHX_API void    File_WriteStr   (File*, cstr);
PHX_API void    File_WriteU8    (File*, uint8);
PHX_API void    File_WriteU16   (File*, uint16);
PHX_API void    File_WriteU32   (File*, uint32);
PHX_API void    File_WriteU64   (File*, uint64);
PHX_API void    File_WriteI8    (File*, int8);
PHX_API void    File_WriteI16   (File*, int16);
PHX_API void    File_WriteI32   (File*, int32);
PHX_API void    File_WriteI64   (File*, int64);
PHX_API void    File_WriteF32   (File*, float);
PHX_API void    File_WriteF64   (File*, double);

#endif
