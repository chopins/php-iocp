typedef void *HANDLE;
typedef int BOOL;
typedef unsigned int u_int;
typedef unsigned long  u_long;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef unsigned __int3264 ULONG_PTR;
typedef void VOID, *PVOID, *LPVOID;
typedef u_int  SOCKET;

typedef struct sockaddr_storage {
  short   ss_family;
  char    __ss_pad1[_SS_PAD1SIZE];
  __int64 __ss_align;
  char    __ss_pad2[_SS_PAD2SIZE];
} SOCKADDR_STORAGE, *PSOCKADDR_STORAGE;

typedef struct{
    SOCKET socket;
    SOCKADDR_STORAGE ClientAddr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct _WSABUF {
  u_long   len;
  char *buf;
} WSABUF, * LPWSABUF;

typedef struct _OVERLAPPED {
  ULONG_PTR Internal;
  ULONG_PTR InternalHigh;
  union {
    struct {
      DWORD Offset;
      DWORD OffsetHigh;
    } DUMMYSTRUCTNAME;
    PVOID Pointer;
  } DUMMYUNIONNAME;
  HANDLE    hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _OVERLAPPEDPLUS{
    OVERLAPPED ol;  
    SOCKET s, sclient;  int OpCode; 
    WSABUF wbuf;　
    DWORD dwBytes, dwFlags;
} OVERLAPPEDPLUS;

HANDLE WINAPI CreateIoCompletionPort(
  _In_     HANDLE    FileHandle,
  _In_opt_ HANDLE    ExistingCompletionPort,
  _In_     ULONG_PTR CompletionKey, 
  _In_     DWORD     NumberOfConcurrentThreads
);

BOOL GetQueuedCompletionStatus(
        HANDLE CompletionPort,
        LPDWORD lpNumberOfBytes,
        PULONG_PTR lpCompletionKey,
        LPOVERLAPPED *lpOverlapped,
        DWORD dwMilliseconds);
BOOL PostQueuedCompletionStatus( 
　　HANDLE CompletlonPort, 
　　DW0RD dwNumberOfBytesTrlansferred, 
　　DWORD dwCompletlonKey,
　　LPOVERLAPPED lpoverlapped); 