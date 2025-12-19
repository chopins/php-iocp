<?php
use Toknot\IOCP;
include dirname(__DIR__).'/src/IOCP.php';
$iocp = new IOCP;
$ciocp = $iocp->createNoFileIoCompletionPort();
if(!$ciocp) {
    die('init IOCP error');
}
$stream = stream_socket_server("tcp://0.0.0.0:8000", $errno, $errstr);
$listenSock = $iocp->getHandle($stream, IOCP::RES_TYPE_NET);

$iocp->CreateIoCompletionPort($listenSock, $ciocp, 1, 0);
