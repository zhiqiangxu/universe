<?php

$configs = ReactHandler::$php->config['db'];

if (empty($configs[''])) {
    throw new Exception("db-> is not found.");
}


