<?php

class IOCP
{
    private static $ffi;
    public function __construct()
    {
        $code = $this->int3264();
        $code .= file_get_contents(__DIR__ . '/iocp.h');
        self::$ffi = FFI::cdef($code);
    }

    public function int3264()
    {
        $type = 'typedef ';
        $type .= PHP_INT_SIZE === 4 ? 'int ' : $type .= 'long long ';
        return $type . ' __int3264';
    }

}
