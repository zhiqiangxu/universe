<?php
namespace Handler\Design;

interface IController
{

    function outputJson($json);
    function outputText($text);
    function header();
    function assign();
    function fetch($tpl_file);
    function outputHtml($tpl_file);
    function setCookie($name, $value, $expire, $path, $domain, $secure, $httponly);
    function sessionStart();

}
