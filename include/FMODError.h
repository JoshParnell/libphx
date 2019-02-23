#ifndef PHX_FMODError
#define PHX_FMODError

#include "Common.h"
#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"

#define FMODERROR_X                                                            \
  X(FMOD_OK)                                                                   \
  X(FMOD_ERR_BADCOMMAND)                                                       \
  X(FMOD_ERR_CHANNEL_ALLOC)                                                    \
  X(FMOD_ERR_CHANNEL_STOLEN)                                                   \
  X(FMOD_ERR_DMA)                                                              \
  X(FMOD_ERR_DSP_CONNECTION)                                                   \
  X(FMOD_ERR_DSP_DONTPROCESS)                                                  \
  X(FMOD_ERR_DSP_FORMAT)                                                       \
  X(FMOD_ERR_DSP_INUSE)                                                        \
  X(FMOD_ERR_DSP_NOTFOUND)                                                     \
  X(FMOD_ERR_DSP_RESERVED)                                                     \
  X(FMOD_ERR_DSP_SILENCE)                                                      \
  X(FMOD_ERR_DSP_TYPE)                                                         \
  X(FMOD_ERR_FILE_BAD)                                                         \
  X(FMOD_ERR_FILE_COULDNOTSEEK)                                                \
  X(FMOD_ERR_FILE_DISKEJECTED)                                                 \
  X(FMOD_ERR_FILE_EOF)                                                         \
  X(FMOD_ERR_FILE_ENDOFDATA)                                                   \
  X(FMOD_ERR_FILE_NOTFOUND)                                                    \
  X(FMOD_ERR_FORMAT)                                                           \
  X(FMOD_ERR_HEADER_MISMATCH)                                                  \
  X(FMOD_ERR_HTTP)                                                             \
  X(FMOD_ERR_HTTP_ACCESS)                                                      \
  X(FMOD_ERR_HTTP_PROXY_AUTH)                                                  \
  X(FMOD_ERR_HTTP_SERVER_ERROR)                                                \
  X(FMOD_ERR_HTTP_TIMEOUT)                                                     \
  X(FMOD_ERR_INITIALIZATION)                                                   \
  X(FMOD_ERR_INITIALIZED)                                                      \
  X(FMOD_ERR_INTERNAL)                                                         \
  X(FMOD_ERR_INVALID_FLOAT)                                                    \
  X(FMOD_ERR_INVALID_HANDLE)                                                   \
  X(FMOD_ERR_INVALID_PARAM)                                                    \
  X(FMOD_ERR_INVALID_POSITION)                                                 \
  X(FMOD_ERR_INVALID_SPEAKER)                                                  \
  X(FMOD_ERR_INVALID_SYNCPOINT)                                                \
  X(FMOD_ERR_INVALID_THREAD)                                                   \
  X(FMOD_ERR_INVALID_VECTOR)                                                   \
  X(FMOD_ERR_MAXAUDIBLE)                                                       \
  X(FMOD_ERR_MEMORY)                                                           \
  X(FMOD_ERR_MEMORY_CANTPOINT)                                                 \
  X(FMOD_ERR_NEEDS3D)                                                          \
  X(FMOD_ERR_NEEDSHARDWARE)                                                    \
  X(FMOD_ERR_NET_CONNECT)                                                      \
  X(FMOD_ERR_NET_SOCKET_ERROR)                                                 \
  X(FMOD_ERR_NET_URL)                                                          \
  X(FMOD_ERR_NET_WOULD_BLOCK)                                                  \
  X(FMOD_ERR_NOTREADY)                                                         \
  X(FMOD_ERR_OUTPUT_ALLOCATED)                                                 \
  X(FMOD_ERR_OUTPUT_CREATEBUFFER)                                              \
  X(FMOD_ERR_OUTPUT_DRIVERCALL)                                                \
  X(FMOD_ERR_OUTPUT_FORMAT)                                                    \
  X(FMOD_ERR_OUTPUT_INIT)                                                      \
  X(FMOD_ERR_OUTPUT_NODRIVERS)                                                 \
  X(FMOD_ERR_PLUGIN)                                                           \
  X(FMOD_ERR_PLUGIN_MISSING)                                                   \
  X(FMOD_ERR_PLUGIN_RESOURCE)                                                  \
  X(FMOD_ERR_PLUGIN_VERSION)                                                   \
  X(FMOD_ERR_RECORD)                                                           \
  X(FMOD_ERR_REVERB_CHANNELGROUP)                                              \
  X(FMOD_ERR_REVERB_INSTANCE)                                                  \
  X(FMOD_ERR_SUBSOUNDS)                                                        \
  X(FMOD_ERR_SUBSOUND_ALLOCATED)                                               \
  X(FMOD_ERR_SUBSOUND_CANTMOVE)                                                \
  X(FMOD_ERR_TAGNOTFOUND)                                                      \
  X(FMOD_ERR_TOOMANYCHANNELS)                                                  \
  X(FMOD_ERR_TRUNCATED)                                                        \
  X(FMOD_ERR_UNIMPLEMENTED)                                                    \
  X(FMOD_ERR_UNINITIALIZED)                                                    \
  X(FMOD_ERR_UNSUPPORTED)                                                      \
  X(FMOD_ERR_VERSION)                                                          \
  X(FMOD_ERR_EVENT_ALREADY_LOADED)                                             \
  X(FMOD_ERR_EVENT_LIVEUPDATE_BUSY)                                            \
  X(FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH)                                        \
  X(FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT)                                         \
  X(FMOD_ERR_EVENT_NOTFOUND)                                                   \
  X(FMOD_ERR_STUDIO_UNINITIALIZED)                                             \
  X(FMOD_ERR_STUDIO_NOT_LOADED)                                                \
  X(FMOD_ERR_INVALID_STRING)                                                   \
  X(FMOD_ERR_ALREADY_LOCKED)                                                   \
  X(FMOD_ERR_NOT_LOCKED)                                                       \
  X(FMOD_ERR_RECORD_DISCONNECTED)                                              \
  X(FMOD_ERR_TOOMANYSAMPLES)                                                   \
  X(FMOD_RESULT_FORCEINT)

inline cstr FMODError_ToString (FMOD_RESULT self) {
  switch (self) {
#define X(x) case x: return #x;
    FMODERROR_X
#undef X
  };
  return "Unknown Error";
}

inline void FMOD_CheckError (FMOD_RESULT result, cstr file, int line, cstr func) {
  if (result != FMOD_OK) {
    Fatal("%s: %s\n%s\n  [%s @ Line %d]",
      func, FMODError_ToString(result), FMOD_ErrorString(result), file, line
    );
  }
}
#define FMODCALL(r) FMOD_CheckError(r, __FILE__, __LINE__, __func__)

#endif
