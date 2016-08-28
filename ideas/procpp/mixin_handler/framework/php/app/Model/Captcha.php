<?php
namespace Model;


class Captcha extends Base
{
    const EXPIRE = 5*60;
    const IMG_WIDTH = 200;
    const IMG_HEIGHT = 50;

    function generate()
    {
        list($captcha, $base64_img) = $this->_generate();

        for ($i = 0; $i < 3; $i++) {

            $captcha_key = uniqid();

            if (!$this->keyExists($captcha_key)) {

                $this->setCache($captcha_key, $captcha, self::EXPIRE);

                return [$captcha_key, $base64_img];
            }
        }
    }

    function verify($captcha_key, $value)
    {

        $captcha = $this->getCache($captcha_key);

        return $captcha && strtolower($captcha) == strtolower($value);
    }

    function keyExists($captcha_key)
    {
        return $this->getCache($captcha_key);
    }

    // 返回 [验证码字串, base64后的png图片]
    private function _generate()
    {
        $image = imagecreatetruecolor(self::IMG_WIDTH, self::IMG_HEIGHT) or die("Cannot Initialize new GD image stream");

        $background_color = imagecolorallocate($image, 255, 255, 255);
        $text_color = imagecolorallocate($image, 180, 4, 4);
        $line_color = imagecolorallocate($image, 249, 198, 36);
        $pixel_color = imagecolorallocate($image, 180, 4, 4);

        imagefilledrectangle($image, 0, 0, self::IMG_WIDTH, self::IMG_HEIGHT, $background_color);

        // 噪音
        for ($i = 0; $i < 3; $i++) {
            imageline($image, 0, rand() % self::IMG_HEIGHT, self::IMG_WIDTH, rand() % self::IMG_HEIGHT, $line_color);
        }

        for ($i = 0; $i < 1000; $i++) {
            imagesetpixel($image, rand() % self::IMG_WIDTH, rand() % self::IMG_HEIGHT, $pixel_color);
        }

        // 文本部分
        // $letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';
        // 去掉了oOiIlLBQ加了345679
        $letters = 'ACDEFGHJKMNPRSTUVWXYZabcdefghjkmnpqrstuvwxyz3456789';
        $len = strlen($letters);

        $text_color = imagecolorallocate($image, 0, 0, 0);
        $word = "";
        $font_file = WEBPATH . '/static/WishfulWaves.ttf';
        $size = 30;

        $selected_letters = [];
        for ($i = 0; $i < 6; $i++) {

            $letter = $letters[rand(0, $len - 1)];

            $selected_letters[] = $letter;

            $word .= $letter;

        }

        $this->_render_center($image, $font_file, $size, $selected_letters, $text_color);

        ob_start();

        imagepng($image);

        $image_data = ob_get_contents();

        ob_end_clean();

        return [$word, base64_encode($image_data)];

    }

    // 居中显示文字
    private function _render_center($image, $font_file, $size, $letters, $text_color)
    {
        $letter_width = [];
        $total_width = 0;
        foreach ($letters as $letter) {
            $box = imageTTFBbox($size, 0, $font_file, $letter);
            $width = abs($box[4] - $box[0]);

            $letter_width[] = $width;
            $total_width += $width;
        }

        $offset = (self::IMG_WIDTH - $total_width)/2;
        foreach ($letters as $i => $letter) {
            $width = $letter_width[$i];
            imagettftext($image, $size, 0, $offset, 40, $text_color, $font_file, $letter);
            $offset += $width;
        }
    }
}
