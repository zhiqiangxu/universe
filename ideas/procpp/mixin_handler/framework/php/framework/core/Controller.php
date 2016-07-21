<?php
namespace ReactHandler;


class Controller extends Object
{
    private $_tpl_vars = [];
    private $_template_dir = WEBPATH . '/views/';

    function outputJson($json)
    {

        global $php;
        $this->header('Content-Type', 'application/json; charset=utf-8');
        $php->response->body = json_encode($json);

    }

    function outputText($text)
    {

        global $php;
        $this->header('Content-Type', 'text/plain; charset=utf-8');
        $php->response->body = $text;

    }

    function header($name, $value)
    {

        global $php;
        $response = $php->response;
        $response->headers->set($name, $value);

    }

    // for html
    function assign()
    {
        $args = func_get_args();
        if (count($args) == 2) {
            $this->_tpl_vars[$args[0]] = $args[1];
        } else {
            $this->_tpl_vars = $args[0];
        }
    }

    function fetch($tpl_file)
    {
        extract($this->_tpl_vars);

        ob_start();
        include($this->_template_dir . $tpl_file);
        return ob_get_clean();
    }


    function outputHtml($tpl_file)
    {
        $html = $this->fetch($tpl_file);

        global $php;
        $this->header('Content-Type', 'text/html; charset=utf-8');
        $php->response->body = $html;
    }


}
