<?php

namespace Toknot;

use FFI;
use FFI\CData;
use TypeError;
use Toknot\FFIExtend;

/**
 * @method int cancelIo()
 * @method int cancelIoEx(HANDLE $hFile, LPOVERLAPPED $lpOverlapped)           need windows 2008
 * @method int cancelSynchronousIo(HANDLE $hThread)                     need windows 2008
 * @method HANDLE CreateIoCompletionPort(HANDLE $FileHandle,HANDLE $ExistingCompletionPort,ULONG_PTR $CompletionKey, DWORD $NumberOfConcurrentThreads)
 * @method BOOL DeviceIoControl(HANDLE $hDevice,DWORD $dwIoControlCode,LPVOID $lpInBuffer,DWORD $nInBufferSize,LPVOID $lpOutBuffer,DWORD $nOutBufferSize,LPDWORD $lpBytesReturned,LPOVERLAPPED $lpOverlapped)
 * @method BOOL GetOverlappedResult(HANDLE $hFile,LPOVERLAPPED $lpOverlapped,LPDWORD $lpNumberOfBytesTransferred,BOOL $bWait)
 * @method BOOL GetOverlappedResultEx(HANDLE $hFile,LPOVERLAPPED $lpOverlapped,LPDWORD $lpNumberOfBytesTransferred,DWORD $dwMilliseconds,BOOL $bAlertable)      need windows 2012
 * @method BOOL GetQueuedCompletionStatusEx(HANDLE $CompletionPort,LPOVERLAPPED_ENTRY $lpCompletionPortEntries,ULONG $ulCount,PULONG $ulNumEntriesRemoved,DWORD $dwMilliseconds,BOOL $fAlertable)    need windows 2008
 * @method  BOOL GetQueuedCompletionStatus(HANDLE $CompletionPort,LPDWORD $lpNumberOfBytes,PULONG_PTR $lpCompletionKey,LPOVERLAPPED *lpOverlapped,DWORD $dwMilliseconds);
 * @method BOOL PostQueuedCompletionStatus(HANDLE $CompletlonPort,DWORD $dwNumberOfBytesTrlansferred,DWORD $dwCompletlonKey,LPOVERLAPPED $lpoverlapped); 
 */
class IOCP
{
    private static $ffi;
    private static $phpExt;
    private static $msvcrt;
    static public $INVALID_HANDLE_VALUE = -1;
    const RES_TYPE_FILE = 1;
    const RES_TYPE_NET = 2;
    public function __construct()
    {
        if (!self::$ffi) {
            self::$phpExt = new FFIExtend;
            $code = file_get_contents(__DIR__ . '/iocp.h');
            $this->checkWinVersion($code);
            $this->replaceKey($code);
            self::$ffi = FFI::cdef($code, 'C:\Windows\System32\kernel32.dll');
            self::$msvcrt = FFI::cdef('intptr_t _get_osfhandle( int fd);', 'C:\Windows\System32\MSVCRT.DLL');
            $this->initInvalidHandle();
        }
    }

    public function getFFI()
    {
        return self::$ffi;
    }

    private function initInvalidHandle()
    {
        self::$INVALID_HANDLE_VALUE = FFI::new('int');
        self::$INVALID_HANDLE_VALUE->cdata = -1;
        self::$INVALID_HANDLE_VALUE = FFI::cast('void*', self::$INVALID_HANDLE_VALUE);
    }

    public function initIoCompletionPort()
    {
        $cp = self::$ffi->CreateIoCompletionPort(self::$INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (self::$phpExt->isNull($cp)) {
            return false;
        }
        return $cp;
    }

    private function replaceKey(&$code)
    {
        $type = PHP_INT_SIZE === 4 ? ' int ' : ' long long ';
        $code = str_replace(' __int3264 ', $type, $code);
        $code = str_replace('_SS_PAD1SIZE', 6, $code);
        $code = str_replace('_SS_PAD2SIZE', 112, $code);
    }

    private function checkWinVersion(&$code)
    {
        $osVersion = php_uname('r');
        if (version_compare($osVersion, 5.2) < 0) {
            trigger_error('IOCP minimum supported Windows Server 2003', E_USER_ERROR);
        }
        self::$phpExt->replaceMacro('WIN2008', version_compare($osVersion, '6.0') < 0, '//', $code);
        self::$phpExt->replaceMacro('WIN2012', version_compare($osVersion, '6.2') < 0, '//', $code);
        self::$phpExt->replaceMacro('IF_PHP_DEBUG', !PHP_DEBUG, '//', $code);
    }

    public function getHandle($resource, $type): CData
    {
        if (!is_resource($resource)) {
            throw new TypeError('Epoll::getFdno() of paramter 1 must be resource');
        }

        $stream = self::$phpExt->zval($resource)->ptr;
        $fd = self::$ffi->cast('php_stream', $stream)->abstract;
        if ($type === self::RES_TYPE_FILE) {
            return self::$ffi->cast('HANDLE', self::$msvcrt->_get_osfhandle(self::$ffi->cast('php_stdio_stream_data', $fd)->fd));
        } elseif ($type === self::RES_TYPE_NET) {
            return self::$ffi->cast('HANDLE', self::$msvcrt->_get_osfhandle(self::$ffi->cast('php_netstream_data_t', $fd)->socket));
        } else {
            return self::$INVALID_HANDLE_VALUE;
        }
    }

    public function __call($name, $arguments)
    {
        $name = ucfirst($name);
        return call_user_func_array([self::$ffi, $name], $arguments);
    }
}
