<?php
namespace Handler\Design;

interface ILogger
{
    static function getInstance();

    function notice($message);

    function warning($message);

    function fatal($message);
}
