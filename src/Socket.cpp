#include "Bytes.h"
#include "PhxMemory.h"
#include "Socket.h"
#include "SocketType.h"
#include "PhxString.h"

#include <stdio.h>
#include <string.h>

#if WINDOWS
  #define _WINSOCK_DEPRECATED_NO_WARNINGS 1
  #include <winsock2.h>

  #define SOCK_CLOSE closesocket
  #define SOCK_ERROR SOCKET_ERROR
  #define SOCK_INVALID INVALID_SOCKET
  #define SOCK_LASTERROR (WSAGetLastError())
  #define SOCK_WOULDBLOCK WSAEWOULDBLOCK

  typedef SOCKET sock_t;
  typedef int socklen_t;
  static WSADATA wsa;
#elif POSIX
  #include <errno.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

  #define SOCK_CLOSE close
  #define SOCK_ERROR -1
  #define SOCK_INVALID -1
  #define SOCK_LASTERROR errno

  #define SOCK_WOULDBLOCK EWOULDBLOCK
  typedef int sock_t;
#endif

#define BUF_SIZE 2048

struct Socket {
  SocketType type;
  sock_t sock;
  sockaddr_in addrSend;
  sockaddr_in addrRecv;
  char buffer[BUF_SIZE];
};

inline static void Socket_Cleanup (sock_t self) {
  if (self != SOCK_INVALID)
    SOCK_CLOSE(self);
}

inline static int Socket_Receive (sock_t self, void* buf, int len) {
  #if POSIX
    return read(self, buf, len);
  #elif WINDOWS
    return recv(self, (char*)buf, len, 0);
  #endif
}

inline static int Socket_Send (sock_t self, void const* buf, int len) {
  #if POSIX
    return write(self, buf, len);
  #elif WINDOWS
    return send(self, (char const*)buf, len, 0);
  #endif
}

inline static bool Socket_SetNonblocking (sock_t self) {
  #if POSIX
    return fcntl(self, F_SETFL, fcntl(self, F_GETFL, 0) | O_NONBLOCK) >= 0;
  #elif WINDOWS
    ulong opt = 1;
    return ioctlsocket(self, FIONBIO, &opt) != SOCKET_ERROR;
  #endif
}

Socket* Socket_Create (SocketType type) {
  if (type != SocketType_UDP && type != SocketType_TCP)
    Fatal("Socket_Create: socket type must be either SocketType_TCP or SocketType_UDP");

  #if WINDOWS
    static bool init = false;
    if (!init) {
      init = true;
      if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        Fatal("Socket_Create: failed to initialize Winsock");
    }
  #endif

  Socket* self = MemNew(Socket);
  self->type = type;
  self->sock = socket(AF_INET, type == SocketType_UDP ? SOCK_DGRAM : SOCK_STREAM, 0);
  if (self->sock == SOCK_INVALID)
    Fatal("Socket_Create: failed to open socket");

  #if POSIX
    int opt = 1;
    if (setsockopt(self->sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)))
      Fatal("Socket_Create: failed to set socket to reusable");
  #endif

  if (!Socket_SetNonblocking(self->sock))
    Fatal("Socket_Create: failed to set socket to non-blocking");
  return self;
}

void Socket_Free (Socket* self) {
  Socket_Cleanup(self->sock);
  MemFree(self);
}

Socket* Socket_Accept (Socket* self) {
  if (self->type != SocketType_TCP)
    Fatal("Socket_Accept: can only accept connections on TCP sockets");

  sock_t sock = accept(self->sock, 0, 0);
  if (sock == SOCK_INVALID)
    return 0;

  Socket* con = MemNew(Socket);
  con->type = SocketType_TCP;
  con->sock = sock;
  if (!Socket_SetNonblocking(con->sock))
    Fatal("Socket_Accept: failed to set socket to non-blocking");
  return con;
}

void Socket_Bind (Socket* self, int port) {
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((uint16)port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(self->sock, (sockaddr*)&addr, sizeof(addr)) == SOCK_ERROR)
    Fatal("Socket_Bind: failed to bind socket");
}

void Socket_Listen (Socket* self) {
  if (self->type != SocketType_TCP)
    Fatal("Socket_Listen: can only listen for connections on TCP sockets");
  if (listen(self->sock, 1) == SOCK_ERROR)
    Fatal("Socket_Listen: failed to listen");
}

cstr Socket_Read (Socket* self) {
  int bytes = Socket_Receive(self->sock, self->buffer, sizeof(self->buffer));
  if (bytes == SOCK_ERROR) {
    if (SOCK_LASTERROR == SOCK_WOULDBLOCK)
      return 0;
    Fatal("Socket_Read: failed to read from socket");
  }

  if (bytes == 0)
    return 0;

  self->buffer[bytes] = 0;
  return self->buffer;
}

Bytes* Socket_ReadBytes (Socket* self) {
  int bytes = Socket_Receive(self->sock, self->buffer, sizeof(self->buffer));
  if (bytes == SOCK_ERROR) {
    if (SOCK_LASTERROR == SOCK_WOULDBLOCK)
      return 0;
    Fatal("Socket_ReadRaw: failed to read from socket");
  }

  if (bytes == 0)
    return 0;

  Bytes* data = Bytes_Create(bytes);
  Bytes_Write(data, self->buffer, bytes);
  return data;
}

int Socket_ReceiveFrom (Socket* self, void* data, size_t len) {
  MemZero(data, len);
  socklen_t addrSize;
#if POSIX
  int bytes = recvfrom(self->sock, data, len, 0, (sockaddr*)&self->addrRecv, &addrSize);
#else
  int bytes = recvfrom(self->sock, (char*)data, (int)len, 0, (sockaddr*)&self->addrRecv, &addrSize);
#endif

  if (bytes == SOCK_ERROR) {
    if (SOCK_LASTERROR == SOCK_WOULDBLOCK)
      return 0;
    return -1;
  }

  return bytes;
}

cstr Socket_GetAddress (Socket* self) {
  return StrFormat("%s:%d",
    inet_ntoa(self->addrRecv.sin_addr),
    ntohs(self->addrRecv.sin_port));
}

void Socket_SetAddress (Socket* self, cstr addr) {
  cstr colon = StrFind(addr, ":");
  if (!colon)
    Fatal("Socket_SetReceiver: address must be in format a.b.c.d:port format");
  cstr ip = StrSubStr(addr, colon);
  cstr port = StrSubStr(colon + 1, addr + strlen(addr));
  self->addrSend.sin_family = AF_INET;
  self->addrSend.sin_port = htons((uint16)strtol(port, 0, 0));

#if POSIX
  if (inet_aton(ip, &self->addrSend.sin_addr) == 0)
#else
  self->addrSend.sin_addr.S_un.S_addr = inet_addr(ip);
  if (self->addrSend.sin_addr.S_un.S_addr == INADDR_NONE)
#endif
    Fatal("Socket_SetReceiver: failed to interpret network address");

  StrFree(ip);
  StrFree(port);
}

int Socket_SendTo (Socket* self, void const* data, size_t len) {
#if POSIX
  int bytes = sendto(self->sock, data, len, 0, (sockaddr*)&self->addrSend, sizeof(self->addrSend));
#else
  int bytes = sendto(self->sock, (cstr)data, (int)len, 0, (sockaddr*)&self->addrSend, sizeof(self->addrSend));
#endif
  if (bytes == SOCK_ERROR)
    return -1;
  return bytes;
}

void Socket_Write (Socket* self, cstr msg) {
  if (Socket_Send(self->sock, msg, StrLen(msg)) == SOCK_ERROR)
    Fatal("Socket_Write: failed to write to socket");
}

void Socket_WriteBytes (Socket* self, Bytes* msg) {
  if (Socket_Send(self->sock, Bytes_GetData(msg), Bytes_GetSize(msg)) == SOCK_ERROR)
    Fatal("Socket_WriteRaw: failed to write to socket");
}
