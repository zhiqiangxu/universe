<?php
namespace Handler\MVC\Http;

use Handler\Design\IController;
use Handler\Component\Utility;
use Handler\Component\Cache;

class Controller implements IController
{

    private $_template_dir = WEBPATH . '/views/http/';
    private $_smarty = null;
    private $_init_session_value = null;

    const SESSION_ID = 'HANDLER_SESSION';
    const SESSION_ID_EXPIRE = 86400000;
    const SESSION_CACHE_EXPIRE = 3600;


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
            $response->headers->push(new \StringPair($args[0], $args[1]));
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

    function setCookie($name, $value = '', $expire = 0, $path = '/', $domain = '', $secure = false, $httponly = false)
    {

        $cookie_values = [ "$name=" . urlencode($value) ];

        if ($expire) {
            $cookie_values[] = "expires=" . gmdate('D, d M Y H:i:s T', time() + $expire);
            $cookie_values[] = "Max-Age=$expire";
        }

        if ($path) $cookie_values[] = "path=$path";
        if ($domain) $cookie_values[] = "domain=$domain";
        if ($secure) $cookie_values[] = "secure";
        if ($httponly) $cookie_values[] = "HttpOnly";

        $this->header('Set-Cookie', implode('; ', $cookie_values));

    }

    private function sessionCacheGroup()
    {

        $session_config = \Handler::$php->config['session'];
        return ($session_config && !empty($session_config['cache_group'])) ? $session_config['cache_group'] : 'default';

    }

    function sessionStart()
    {

        if (!is_null($this->_init_session_value)) return;

        if (!empty($_COOKIE[self::SESSION_ID])) {

            $session_data = Cache::getInstance($this->sessionCacheGroup())->get($_COOKIE[self::SESSION_ID]);
            if ($session_data) $this->_init_session_value = $_SESSION = $session_data;

        } else {

            $session_id = Utility::generateRandomString(40);
            $this->setCookie(self::SESSION_ID, $session_id, self::SESSION_ID_EXPIRE);
            $_COOKIE[self::SESSION_ID] = $session_id;

        }

        $this->_init_session_value = $_SESSION = [];

    }

    function __destruct()
    {

        if (!is_null($this->_init_session_value) && ($_SESSION !== $this->_init_session_value)) {

            Cache::getInstance($this->sessionCacheGroup())->set(
                $_COOKIE[self::SESSION_ID],
                $_SESSION,
                self::SESSION_CACHE_EXPIRE
            );

        }

    }
}
