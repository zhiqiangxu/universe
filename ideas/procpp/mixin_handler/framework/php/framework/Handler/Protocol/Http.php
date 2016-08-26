<?php
namespace Handler\Protocol;
use Handler\Design\IHttp;

class Http implements IHttp
{

    static function handle($request, $response)
    {

        self::prepareRequest($request);

        if (self::serveStatic()) return;

        global $php;
        $php->request = $request;
        $php->response = $response;

        self::runMVC();

    }

    static private function serveStatic()
    {

        if (preg_match('/\.[a-z]+/', $_SERVER['REQUEST_URI'])) {

            $resource_path = WEBPATH . $_SERVER['REQUEST_URI'];
            global $php;
            if (!file_exists($resource_path)) {

                $php->response->status_code = 404;
                $php->response->reason_phrase = "File not exists";

            } else {

                //TODO compress
                $php->response->body = file_get_contents($resource_path);

            }
            return true;

        }

        return false;

    }

    static private function prepareRequest($request)
    {

        $uri_info = parse_url($request->uri);

        $_SERVER['REQUEST_URI'] = $uri_info['path'];
        parse_str($uri_info['query'], $_GET);
        parse_str($request->body, $_POST);

        $_REQUEST = array_merge($_GET, $_POST);

    }

    static function runMVC()
    {

        // load $route
        $route = require (WEBPATH . '/conf/routes.php');

        $pos = strpos($_SERVER['REQUEST_URI'], '?');
        $path_info = $pos === false ? $_SERVER['REQUEST_URI'] : substr($_SERVER['REQUEST_URI'], 0, $pos);

        $uri = trim($path_info, '/');

        if (!$uri)
            $uri = $route['default_route'];

        foreach ($route['rewrite'] as $rewrite) {
            if (preg_match( '#' . $rewrite['regex'] . '#i', '/' . $uri, $match )) {

                array_shift($match);

                $controller = $rewrite['controller'];
                $view = $rewrite['view'];

                $controller_class = '\\App\\Controller\\Http\\' . $controller;
                $controller_instance = new $controller_class;
                call_user_func_array([$controller_instance, $view], $match);

                return;
            }
        }


        $parts = explode('/', $uri);
        $view = count($parts) > 1 ? array_pop($parts) : 'index';

        $controller_class = '\\App\\Controller\\Http\\' . implode('\\', array_map(function($part) { return ucfirst($part); }, $parts));
        $controller_instance = new $controller_class;
        call_user_func([$controller_instance, $view]);

    }

}
