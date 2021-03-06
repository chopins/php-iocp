typedef void *HANDLE;
typedef int BOOL;
typedef unsigned int u_int;
typedef unsigned long  u_long;
typedef unsigned long  ULONG, *PULONG;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef unsigned __int3264 ULONG_PTR;
typedef void VOID, *PVOID, *LPVOID;
typedef u_int  SOCKET;
typedef long long __int64;
typedef char CHAR, *PCHAR;
typedef ULONG_PTR *PULONG_PTR;
typedef unsigned long DWORD;

typedef uint64_t zend_ulong;
typedef int64_t zend_long;
typedef int64_t zend_off_t;
typedef unsigned long sigset_t;

typedef struct _zend_object_handlers zend_object_handlers;
typedef unsigned char zend_uchar;
typedef struct _zend_array zend_array;
typedef struct _zend_object zend_object;
typedef struct _zend_resource zend_resource;
typedef struct _zend_reference zend_reference;
typedef struct _zval_struct zval;
typedef struct _zend_ast_ref    zend_ast_ref;
typedef struct _zend_ast        zend_ast;
typedef struct _zend_class_entry     zend_class_entry;
typedef union  _zend_function        zend_function;
typedef struct _zend_array HashTable;
typedef struct _php_netstream_data_t php_netstream_data_t;
typedef void (*dtor_func_t)(zval *pDest);

typedef struct _php_stream php_stream;

typedef struct _zend_refcounted_h {
    uint32_t         refcount;                      /* reference counter 32-bit */
    union {
            uint32_t type_info;
    } u;
} zend_refcounted_h;
typedef struct _zend_string {
    zend_refcounted_h gc;
    zend_ulong        h;                /* hash value */
    size_t            len;
    char              val[1];
} zend_string;
typedef struct _zend_refcounted {
    zend_refcounted_h gc;
} zend_refcounted;
struct _zend_resource {
    zend_refcounted_h gc;
    int               handle; // TODO: may be removed ???
    int               type;
    void             *ptr;
};
typedef struct _zend_property_info zend_property_info;

typedef union {
        zend_property_info *ptr;
        uintptr_t list;
} zend_property_info_source_list;


struct _zval_struct {
    union {
        zend_long         lval;             /* long value */
        double            dval;             /* double value */
        zend_refcounted  *counted;
        zend_string      *str;
        zend_array       *arr;
        zend_object      *obj;
        zend_resource    *res;
        zend_reference   *ref;
        zend_ast_ref     *ast;
        zval             *zv;
        void             *ptr;
        zend_class_entry *ce;
        zend_function    *func;
        struct {
            uint32_t w1;
            uint32_t w2;
        } ww;
    } value;
    union {
        struct {
                zend_uchar    type;         /* active type */
                zend_uchar    type_flags;
                zend_uchar    const_flags;
                zend_uchar    reserved;     /* call info for EX(This) */
        } v;
        uint32_t type_info;
    } u1;
    union {
        uint32_t     var_flags;
        uint32_t     next;                 /* hash collision chain */
        uint32_t     cache_slot;           /* literal cache slot */
        uint32_t     lineno;               /* line number (for ast nodes) */
        uint32_t     num_args;             /* arguments number for EX(This) */
        uint32_t     fe_pos;               /* foreach position */
        uint32_t     fe_iter_idx;          /* foreach iterator index */
    } u2;
};
struct _zend_ast_ref {
        zend_refcounted_h gc;
        /*zend_ast        ast; zend_ast follows the zend_ast_ref structure */
};
struct _zend_reference {
    zend_refcounted_h              gc;
    zval                           val;
    zend_property_info_source_list sources;
};
struct _zend_object {
    zend_refcounted_h gc;
    uint32_t          handle; // TODO: may be removed ???
    zend_class_entry *ce;
    const zend_object_handlers *handlers;
    HashTable        *properties;
    zval              properties_table[1];
};
typedef struct _Bucket {
    zval              val;
    zend_ulong        h;                /* hash value (or numeric index)   */
    zend_string      *key;              /* string key or NULL for numerics */
} Bucket;

typedef struct _zend_array {
    zend_refcounted_h gc;
    union {
            struct {
                zend_uchar    flags;
                zend_uchar    _unused;
                zend_uchar    nIteratorsCount;
                zend_uchar    _unused2;
            } v;
            uint32_t flags;
    } u;
    uint32_t          nTableMask;
    Bucket           *arData;
    uint32_t          nNumUsed;
    uint32_t          nNumOfElements;
    uint32_t          nTableSize;
    uint32_t          nInternalPointer;
    zend_long         nNextFreeElement;
    dtor_func_t       pDestructor;
};

typedef struct _php_stream_filter_chain {
        void *head, *tail;

        /* Owning stream */
        php_stream *stream;
} php_stream_filter_chain;

struct _php_stream  {
    void *ops;
    void *abstract;                 /* convenience pointer for abstraction */

    php_stream_filter_chain readfilters, writefilters;

    void *wrapper; /* which wrapper was used to open the stream */
    void *wrapperthis;              /* convenience pointer for a instance of a wrapper */
    zval wrapperdata;               /* fgetwrapperdata retrieves this */

    uint8_t is_persistent:1;
    uint8_t in_free:2;                      /* to prevent recursion during free */
    uint8_t eof:1;
    uint8_t __exposed:1;    /* non-zero if exposed as a zval somewhere */

    /* so we know how to clean it up correctly.  This should be set to
        * PHP_STREAM_FCLOSE_XXX as appropriate */
    uint8_t fclose_stdiocast:2;

    uint8_t fgetss_state;           /* for fgetss to handle multiline tags */

    char mode[16];                  /* "rwb" etc. ala stdio */

    uint32_t flags; /* PHP_STREAM_FLAG_XXX */

    zend_resource *res;             /* used for auto-cleanup */
    void *stdiocast;    /* cache this, otherwise we might leak! */
    char *orig_path;

    zend_resource *ctx;

    /* buffer */
    zend_off_t position; /* of underlying stream */
    unsigned char *readbuf;
    size_t readbuflen;
    zend_off_t readpos;
    zend_off_t writepos;

    /* how much data to read when filling buffer */
    size_t chunk_size;

    IF_PHP_DEBUG const char *open_filename;uint32_t open_lineno;

    struct _php_stream *enclosing_stream; /* this is a private stream owned by enclosing_stream */
}; /* php_stream */

typedef struct {
	void *file;
	int fd;					/* underlying file descriptor */
	unsigned is_process_pipe:1;	/* use pclose instead of fclose */
	unsigned is_pipe:1;			/* don't try and seek */
	unsigned cached_fstat:1;	/* sb is valid */
	unsigned is_pipe_blocking:1; /* allow blocking read() on pipes, currently Windows only */
	unsigned no_forced_fstat:1;  /* Use fstat cache even if forced */
	unsigned _reserved:28;

	int lock_flag;			/* stores the lock state */
	zend_string *temp_name;	/* if non-null, this is the path to a temporary file that
							 * is to be deleted when the stream is closed */
	char last_op;

  char *last_mapped_addr;
  HANDLE file_mapping;
	void * sb;
} php_stdio_stream_data;

struct _php_netstream_data_t	{
	int socket;
	char is_blocked;
	void* timeout;
	char timeout_event;
	size_t ownsize;
} ;

typedef struct _WSABUF {
  ULONG len;
  CHAR  *buf;
} WSABUF, *LPWSABUF;

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
   SOCKET s, sclient;
   int OpCode;
   WSABUF wbuf;
   DWORD dwBytes, dwFlags;
} OVERLAPPEDPLUS;
typedef struct _OVERLAPPED_ENTRY {
  ULONG_PTR    lpCompletionKey;
  LPOVERLAPPED lpOverlapped;
  ULONG_PTR    Internal;
  DWORD        dwNumberOfBytesTransferred;
} OVERLAPPED_ENTRY, *LPOVERLAPPED_ENTRY;

BOOL CancelIo(
  HANDLE hFile
);
WIN2008 BOOL CancelIoEx(HANDLE hFile, LPOVERLAPPED lpOverlapped);

WIN2008 BOOL CancelSynchronousIo(HANDLE hThread);
HANDLE CreateIoCompletionPort(
  HANDLE    FileHandle,
  HANDLE    ExistingCompletionPort,
  ULONG_PTR CompletionKey, 
  DWORD     NumberOfConcurrentThreads
);
BOOL DeviceIoControl(
  HANDLE       hDevice,
  DWORD        dwIoControlCode,
  LPVOID       lpInBuffer,
  DWORD        nInBufferSize,
  LPVOID       lpOutBuffer,
  DWORD        nOutBufferSize,
  LPDWORD      lpBytesReturned,
  LPOVERLAPPED lpOverlapped
);
BOOL GetOverlappedResult(
  HANDLE       hFile,
  LPOVERLAPPED lpOverlapped,
  LPDWORD      lpNumberOfBytesTransferred,
  BOOL         bWait
);
WIN2012 BOOL GetOverlappedResultEx(HANDLE hFile,LPOVERLAPPED lpOverlapped,LPDWORD lpNumberOfBytesTransferred,DWORD  dwMilliseconds,BOOL  bAlertable);
WIN2008 BOOL GetQueuedCompletionStatusEx(HANDLE  CompletionPort,LPOVERLAPPED_ENTRY lpCompletionPortEntries,ULONG  ulCount,PULONG  ulNumEntriesRemoved,DWORD dwMilliseconds,BOOL  fAlertable);
BOOL GetQueuedCompletionStatus(HANDLE CompletionPort,LPDWORD lpNumberOfBytes,PULONG_PTR lpCompletionKey,LPOVERLAPPED *lpOverlapped,DWORD dwMilliseconds);

BOOL PostQueuedCompletionStatus(
  HANDLE CompletlonPort,
DWORD dwNumberOfBytesTrlansferred,
DWORD dwCompletlonKey,
LPOVERLAPPED lpoverlapped); 