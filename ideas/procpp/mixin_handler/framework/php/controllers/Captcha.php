<?php

namespace Controller;

use Model\App;
use Model\Captcha as CaptchaModel;

class Captcha extends Base
{
    function index()
    {
        $app_id = $_GET['appid'];
        $app_secret = $_GET['appsecret'];

        $app = App::getInstance();

        if (!$app->verify($app_id, $app_secret)) return;


        $captcha = CaptchaModel::getInstance();
        list ($captcha_key, $base64_img) = $captcha->generate();

        $this->outputJson(['captcha_key' => $captcha_key, 'base64_img' => $base64_img]);
    }

    function verify()
    {
        $captcha_key = $_GET['captcha_key'];
        $value = $_GET['value'];

        $captcha = CaptchaModel::getInstance();
        echo $captcha->verify($captcha_key, $value) ? 'OK' : 'NG';
    }
}
