/* This macro definition has no actual functionality and is used solely to suppress IDE warnings or error messages. */
#define WIN2008
#define WIN2012
#define __int3264 int
#define _SS_PAD1SIZE 6
#define _SS_PAD2SIZE 12
/* ---- END ------- */

typedef const unsigned short *LPCWSTR;
typedef void *HANDLE;
typedef const void *LPCVOID;
typedef int BOOL;
typedef unsigned __int3264 ULONG_PTR;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned long ULONG, *PULONG;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;

typedef void VOID, *PVOID, *LPVOID;
typedef u_int SOCKET;
typedef long long __int64;
typedef char CHAR, *PCHAR, *LPWSTR;
typedef ULONG_PTR *PULONG_PTR;
typedef unsigned long DWORD;

typedef struct _WSABUF
{
  ULONG len;
  CHAR *buf;
} WSABUF, *LPWSABUF;

typedef struct sockaddr_storage
{
  short ss_family;
  char __ss_pad1[_SS_PAD1SIZE];
  __int64 __ss_align;
  char __ss_pad2[_SS_PAD2SIZE];
} SOCKADDR_STORAGE, *PSOCKADDR_STORAGE;

typedef struct
{
  SOCKET socket;
  SOCKADDR_STORAGE ClientAddr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct _OVERLAPPED
{
  ULONG_PTR Internal;
  ULONG_PTR InternalHigh;
  union
  {
    struct
    {
      DWORD Offset;
      DWORD OffsetHigh;
    } DUMMYSTRUCTNAME;
    PVOID Pointer;
  } DUMMYUNIONNAME;
  HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _OVERLAPPEDPLUS
{
  OVERLAPPED ol;
  SOCKET s, sclient;
  int OpCode;
  WSABUF wbuf;
  DWORD dwBytes, dwFlags;
} OVERLAPPEDPLUS;
typedef struct _OVERLAPPED_ENTRY
{
  ULONG_PTR lpCompletionKey;
  LPOVERLAPPED lpOverlapped;
  ULONG_PTR Internal;
  DWORD dwNumberOfBytesTransferred;
} OVERLAPPED_ENTRY, *LPOVERLAPPED_ENTRY;

typedef struct _SECURITY_ATTRIBUTES
{
  DWORD nLength;
  LPVOID lpSecurityDescriptor;
  BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

BOOL CancelIo(HANDLE hFile);
WIN2008 BOOL CancelIoEx(HANDLE hFile, LPOVERLAPPED lpOverlapped);
WIN2008 BOOL CancelSynchronousIo(HANDLE hThread);
HANDLE CreateIoCompletionPort(HANDLE FileHandle, HANDLE ExistingCompletionPort, ULONG_PTR CompletionKey, DWORD NumberOfConcurrentThreads);
BOOL DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
BOOL GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait);
WIN2012 BOOL GetOverlappedResultEx(HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, DWORD dwMilliseconds, BOOL bAlertable);
WIN2008 BOOL GetQueuedCompletionStatusEx(HANDLE CompletionPort, LPOVERLAPPED_ENTRY lpCompletionPortEntries, ULONG ulCount, PULONG ulNumEntriesRemoved, DWORD dwMilliseconds, BOOL fAlertable);
BOOL GetQueuedCompletionStatus(HANDLE CompletionPort, LPDWORD lpNumberOfBytes, PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped, DWORD dwMilliseconds);
BOOL PostQueuedCompletionStatus(HANDLE CompletlonPort, DWORD dwNumberOfBytesTrlansferred, DWORD dwCompletlonKey, LPOVERLAPPED lpoverlapped);

DWORD GetLastError();
DWORD FormatMessageW(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR lpBuffer, DWORD nSize, ...);

HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

BOOL CloseHandle(HANDLE hObject);
