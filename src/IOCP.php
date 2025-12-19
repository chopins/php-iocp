<?php

namespace Toknot;

use FFI;
use FFI\CData;
use TypeError;

class IOCP
{
    private static ?FFI $win = null;
    private static FFI $msvcrt;
    private static FFI $php;
    private static $before2008 = false;
    private static $before2012 = false;
    private static $resourceCache = [];
    const RES_TYPE_FILE = 1;
    const RES_TYPE_NET = 2;
    const SYMTABLE_CACHE_SIZE = 32;
    const ZEND_ARRAY_SIZE = 48 + \PHP_INT_SIZE;
    public function __construct()
    {
        if (!self::$win) {
            $php = file_get_contents(__DIR__ . '/php.h');
            $this->replacePHPDefined($php);
            $phpdll = $this->findPHPDLL();
            self::$php = FFI::cdef($php, $phpdll);
            $code = file_get_contents(__DIR__ . '/iocp.h');
            $this->checkWinVersion($code);
            self::$win = FFI::cdef($code, 'C:\Windows\System32\kernel32.dll');
            self::$msvcrt = FFI::cdef('intptr_t _get_osfhandle( int fd);', 'C:\Windows\System32\MSVCRT.DLL');
            $ptr = self::$win->cast('void*', -1);
            define('INVALID_HANDLE_VALUE', $ptr);
            define('INFINITE', self::$win->cast('DWORD', -1));
        }
    }

    public function findPHPDLL()
    {
        $ts = \PHP_ZTS ? 'ts' : '';
        $dll = \dirname(\PHP_BINARY) . "/php8{$ts}.dll";
        if (\file_exists($dll)) {
            return $dll;
        }
        $dll = \PHP_LIBDIR . "/php8{$ts}.dll";
        if (\file_exists($dll)) {
            return $dll;
        }
    }
    private function replacePHPdefined(&$code)
    {

        $code = \str_replace(
            ['__SYMTABLE_CACHE_SIZE__', '__ZEND_ARRAY_SIZE__', 'zend_long', '__PHP85_EG_FEILDS__'],
            [
                self::SYMTABLE_CACHE_SIZE,
                self::ZEND_ARRAY_SIZE,
                \PHP_INT_SIZE == 8 ? 'int64_t' : 'int32_t',
                \PHP_VERSION_ID >= 80500 ? 'bool fatal_error_backtrace_on;zval last_fatal_error_backtrace;' : '',
            ],
            $code
        );
        if (\PHP_ZTS) {
            $code .= 'void *tsrm_get_ls_cache(void);size_t executor_globals_offset;';
        } else {
            $code .= \PHP_EOL . 'extern zend_executor_globals executor_globals;';
        }
    }

    private function checkWinVersion(&$code)
    {
        $code = str_replace(['_SS_PAD1SIZE', '_SS_PAD2SIZE', '__int3264'], [6, 12, \PHP_INT_SIZE == 8 ? 'long long' : 'int'], $code);

        $osVersion = php_uname('r');
        if (version_compare($osVersion, 5.2) < 0) {
            trigger_error('IOCP minimum supported Windows Server 2003', E_USER_ERROR);
        }
        self::$before2008 = (version_compare($osVersion, '6.0') < 0);
        self::$before2012 = (version_compare($osVersion, '6.2') < 0);
        $this->replaceMacro('WIN2008', self::$before2008, '//', $code);
        $this->replaceMacro('WIN2012', self::$before2012, '//', $code);
    }

    public function replaceMacro($name, $check, $value, &$code)
    {
        if ($check) {
            $code = str_replace($name, $value, $code);
        } else {
            $code = str_replace($name, '', $code);
        }
    }

    /**
     * @param resource $resource
     */
    public function getHandle($resource): CData
    {
        if (!is_resource($resource)) {
            throw new TypeError('IOCP::getHandle() of paramter 1 must be resource');
        }
        $resid = \get_resource_id($resource);
        if (isset(self::$resourceCache[$resid])) {
            return self::$resourceCache[$resid];
        }

        if (\PHP_ZTS) {
            $tsrm = self::$php->cast('char*', self::$php->tsrm_get_ls_cache());
            $cex = self::$php->cast('zend_executor_globals*', $tsrm + self::$php->executor_globals_offset)->current_execute_data;
        } else {
            $cex = self::$php->executor_globals->current_execute_data;
        }
        $ex = self::$php->cast('zval*', $cex);
        $zvalSize = FFI::sizeof(self::$php->type('zval'));
        $exSize = FFI::sizeof(self::$php->type('zend_execute_data'));
        $arg = $ex + (($exSize + $zvalSize - 1) / $zvalSize);
        $stream = self::$php->cast('php_stream', $arg->res->ptr);

        $fd = self::$php->cast('php_stream', $stream)->abstract;
        $meta =  \stream_get_meta_data($resource);
        $handle = INVALID_HANDLE_VALUE;
        if ($meta['stream_type'] == 'STDIO' && $meta['wrapper_type'] == 'plainfile') {
            $handle = self::$win->cast('HANDLE', self::$msvcrt->_get_osfhandle(self::$php->cast('php_stdio_stream_data', $fd)->fd));
        } elseif (strpos($meta['stream_type'], 'tcp_socket') === 0) {
            $handle = self::$win->cast('HANDLE', self::$msvcrt->_get_osfhandle(self::$php->cast('php_netstream_data', $fd)->php_sock));
        }
        self::$resourceCache[$resid] = $handle;
        return $handle;
    }

    public function getLastErrorMsg()
    {
        $no = self::$win->GetLastError();
        return FFI::string(self::$php->php_win32_error_to_msg($no)) . "($no)";
    }

    public function wchar($str)
    {
        $len = \strlen($str);
        $slen = self::$php->new('uint64_t');
        $slen->cdata = $len;
        if(self::$php->php_win32_cp_use_unicode()) {
            $ret = self::$php->php_win32_cp_conv_ascii_to_w($str, $len, FFI::addr($slen));
            if(!$ret) {
                $ret = self::$php->php_win32_cp_conv_utf8_to_w($str, $len, FFI::addr($slen));
                if($ret) {
                    return $ret;
                }
            } else {
                return $ret;
            }
        }
        $ret = self::$php->php_win32_cp_conv_cur_to_w($str, $len, FFI::addr($slen));
        return $ret;
    }

    public function castULongPtr($int)
    {
        return self::$win->cast('ULONG_PTR', $int);
    }

    public function __call($name, $arguments)
    {
        return self::$win->$name(...$arguments);
    }

    public function createNoFileIoCompletionPort(): ?CData
    {
        $cp = self::$win->CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (!isset($cp) || FFI::isNull($cp)) {
            return null;
        }
        return $cp;
    }

    public function  cancelIo(CData $hFile): int
    {
        return self::$win->cancelIo($hFile);
    }
    /**
     * need windows 2008
     *
     */
    public function  cancelIoEx(CData $hFile, LPOVERLAPPED $lpOverlapped): int
    {
        if (self::$before2008) {
            throw new \BadMethodCallException("funcltion cancelIoEx() need windows 2008");
        }
        return self::$win->cancelIoEx($hFile, $lpOverlapped);
    }
    /**
     * need windows 2008
     */
    public function cancelSynchronousIo(HANDLE $hThread): int
    {
        if (self::$before2008) {
            throw new \BadMethodCallException("function cancelSynchronousIo() need windows 2008");
        }
        return self::$win->cancelSynchronousIo($hThread);
    }

    public function  CreateIoCompletionPort(CData $FileHandle, CData $ExistingCompletionPort, int $CompletionKey, int $NumberOfConcurrentThreads): ?CData
    {
        return self::$win->CreateIoCompletionPort($FileHandle,  $ExistingCompletionPort,  $CompletionKey, $NumberOfConcurrentThreads);
    }

    public function  DeviceIoControl(HANDLE $hDevice, int $dwIoControlCode, LPVOID $lpInBuffer, int $nInBufferSize, LPVOID $lpOutBuffer, int $nOutBufferSize, LPDWORD $lpBytesReturned, LPOVERLAPPED $lpOverlapped): bool
    {
        return self::$win->DeviceIoControl($hDevice,  $dwIoControlCode,  $lpInBuffer,  $nInBufferSize,  $lpOutBuffer,  $nOutBufferSize,  $lpBytesReturned,  $lpOverlapped);
    }

    public function  GetOverlappedResult(CData $hFile, LPOVERLAPPED $lpOverlapped, int $lpNumberOfBytesTransferred, BOOL $bWait): bool
    {

        return self::$win->GetOverlappedResult($hFile,  $lpOverlapped,  $lpNumberOfBytesTransferred,  $bWait);
    }
    /**
     * need windows 2012
     */
    public function  GetOverlappedResultEx(HANDLE $hFile, LPOVERLAPPED $lpOverlapped, LPDWORD $lpNumberOfBytesTransferred, DWORD $dwMilliseconds, $bAlertable): bool
    {
        if (self::$before2012) {
            throw new \BadMethodCallException("function GetOverlappedResultEx() need windows 2012");
        }
        return self::$win->GetOverlappedResultEx($hFile,  $lpOverlapped,  $lpNumberOfBytesTransferred,  $dwMilliseconds, $bAlertable);
    }
    /**
     * need windows 2008
     *
     */
    public function  GetQueuedCompletionStatusEx(CData $CompletionPort, CData $lpCompletionPortEntries, int $ulCount, int $ulNumEntriesRemoved, int $dwMilliseconds, BOOL $fAlertable): bool
    {
        if (self::$before2008) {
            throw new \BadMethodCallException("function GetQueuedCompletionStatusEx() need windows 2008");
        }
        $ulNumEntriesRemoved = self::$win->new('PULONG');
        $ulNumEntriesRemoved->cdata = $ulNumEntriesRemoved;
        return self::$win->GetQueuedCompletionStatusEx($CompletionPort,  $lpCompletionPortEntries,  $ulCount,  $ulNumEntriesRemoved, $dwMilliseconds,  $fAlertable);
    }

    public function  GetQueuedCompletionStatus(CData $CompletionPort, CData $lpNumberOfBytes, int $lpCompletionKey, ?CData $lpOverlapped, int $dwMilliseconds): bool
    {
        $lpCompletionKey = self::$win->new('ULONG_PTR');
        $lpCompletionKey->cdata = $lpCompletionKey;
        return self::$win->GetQueuedCompletionStatus($CompletionPort,  $lpNumberOfBytes,  FFI::addr($lpCompletionKey),  $lpOverlapped,  $dwMilliseconds);
    }

    public function  PostQueuedCompletionStatus(CData $CompletionPort, int $dwNumberOfBytesTrlansferred, int $dwCompletlonKey, LPOVERLAPPED $lpoverlapped): bool
    {
        return self::$win->PostQueuedCompletionStatus($CompletionPort,  $dwNumberOfBytesTrlansferred,  $dwCompletlonKey,  $lpoverlapped);
    }
}
