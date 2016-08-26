<?php
namespace Handler\MVC\Http;
use Handler\Design\IController;

//TODO make it decoupled from ext
class Controller implements IController
{

    private $_template_dir = WEBPATH . '/views/';
    private $_smarty = null;

    function outputJson($json)
    {

        $this->header('Content-Type', 'application/json; charset=utf-8');

        global $php;
        if ($php->response) $php->response->body = json_encode($json);
        else echo json_encode($json);

    }

    function outputText($text)
    {

        $this->header('Content-Type', 'text/plain; charset=utf-8');

        global $php;
        if ($php->response) $php->response->body = $text;
        else echo $text;

    }

    function header()
    {

        $args = func_get_args();

        global $php;
        if ($php->response) {
            $response = $php->response;
            $response->headers->set($args[0], $args[1]);
        } else \header($args[0], $args[1]);

    }

    function assign()
    {

        $smarty = $this->getSmarty();

        $args = func_get_args();
        if (count($args) == 2) {

            $smarty->assign($args[0], $args[1]);

        } else {

            $smarty->assign($args[0]);

        }

    }

    private function getSmarty()
    {

        if (!$this->_smarty) {
            $this->_smarty = new \Smarty;
            $this->_smarty->compile_dir = '/tmp/cache/templates_c';
        }

        return $this->_smarty;

    }

    function fetch($tpl_file)
    {

        $smarty = $this->getSmarty();

        return $smarty->fetch($this->_template_dir . $tpl_file);

    }

    function outputHtml($tpl_file)
    {

        $html = $this->fetch($tpl_file);

        $this->header('Content-Type', 'text/html; charset=utf-8');

        global $php;
        if ($php->response) $php->response->body = $html;
        else echo $html;

    }

}
