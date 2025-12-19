<?php

include dirname(__DIR__) . '/src/IOCP.php';
$iocp = new \Toknot\IOCP;
define('GENERIC_READ', 0x80000000);
define('FILE_SHARE_READ', 1);
define('OPEN_EXISTING', 3);
define('FILE_FLAG_OVERLAPPED', 0x40000000);
define('FILE_FLAG_SEQUENTIAL_SCAN', 0x08000000);
$file = 'C:/Users/w/php/test.txt';
$d = $iocp->wchar($file);
var_dump($d);

$handle = $iocp->CreateFileW($d, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED|FILE_FLAG_SEQUENTIAL_SCAN, null);
if($handle == INVALID_HANDLE_VALUE) {
    var_dump($iocp->getLastErrorMsg());
    die('error 0');
}

//$handle = $iocp->getHandle($fp);
$cp = $iocp->createNoFileIoCompletionPort();

if(!$cp) {
    die('error 1');
}

if(!$iocp->CreateIoCompletionPort($handle, $cp, 0 , 0)) {
    $err= $iocp->GetLastError();
    var_dump($err);
    die('error 2');
}
$buffer = $iocp->new('char*');
$bufferSize = 1024;
$bytesRead = $iocp->new('LPDWORD');
$overlapped = $iocp->new('LPOVERLAPPED');

$rest = $iocp->ReadFile(
    $handle,
    $buffer,
    $bufferSize,
    $bytesRead,
    $overlapped,
);

$readByte = $iocp->newLPDWORD();
var_dump('read');
$iocp->GetQueuedCompletionStatus($cp, $readByte, 0, null, -1);
echo 'get end';
$iocp->CloseHandle($cp);
$iocp->CloseHandle($handle);
