<?php
include __DIR__ . "/common.php";

define('TESS_DEBUG', false);

//tesseract预处理: 黑白化
function blackwhite(&$img, $min_gray, $max_gray)
{
    $width=imagesx($img);
    $height=imagesy($img);
    $color_pen=imageColorAllocate($img, 255, 255, 255);    //白色
    for ($y=0; $y<$height; ++$y)
    {
        for ($x=0; $x<$width; ++$x)
        {
            $color=imageColorAt($img, $x, $y);
            $rgb=imageColorsForIndex($img, $color);
            $grey=(255-($rgb['red']*0.30+$rgb['green']*0.59+$rgb['blue']*0.11))/255;
            //排除深黑的干扰像素$grey>=0.90, 排除接近白色的背景像素$grey<=0.3
            if ($grey>=$min_gray && $grey<=$max_gray)
            {
                if (TESS_DEBUG) {echo '.';}
                imageSetPixel($img, $x, $y, $color_pen);
            }
            else
            {
                imageSetPixel($img, $x, $y, 0);
                if (TESS_DEBUG) {echo ' ';}
            }
        }
        if (TESS_DEBUG) {echo "\n";}
    }
}

//tesseract预处理: 去除杂点
function reduce_noise(&$img, $font_width, $font_height, $min_weight)
{
    $width=imagesx($img);
    $height=imagesy($img);
    $removes=array();
    for ($y=0; $y<$height; ++$y)
    {
        for ($x=0; $x<$width; ++$x)
        {
            $color=imageColorAt($img, $x, $y);
            if ($color==16777215)    //白色
            {
                $weight=near_weight($img, $width, $height, $font_width, $font_height, $x, $y);
                if (TESS_DEBUG)
                {
                    echo base_convert($weight, 10, 36);
                }
                if ($weight<$min_weight)
                {
                    $removes[]=array($x, $y);    //为避免影响计算周围权重, 只先保存到$removes, 最后才真正remove
                }
            }
            else
            {
                if (TESS_DEBUG)
                {
                    echo ' ';
                }
            }
        }
        if (TESS_DEBUG) {echo "\n";}
    }
    foreach ($removes as $v)
    {
        imageSetPixel($img, $v[0], $v[1], 0);    //黑色
    }
}

//tesseract预处理: 附近权重
function near_weight($img, $img_width, $img_height, $font_width, $font_height, $x, $y)
{
    $cnt=0;
    $min_x = max(0, $x-$font_width);
    $min_y = max(0, $y-$font_height);
    $max_x = min($img_width - 1, $x+$font_width);
    $max_y = min($img_height - 1, $y+$font_height);

    for ($w = $min_x; $w <= $max_x; ++$w)
    {
        for ($h = $min_y; $h <= $max_y; ++$h)
        {
            if (imageColorAt($img, $w, $h)==16777215)
            {
                ++$cnt;
            }
        }
    }

    return $cnt;
}


$options = get_options();
$remaining_args = get_remaining_args();
$image_files = glob_files($remaining_args);

foreach ($image_files as $image_file)
{
    echo $image_file . "\n";
    $img = imageCreateFromJpeg($image_file);
    myassert($img, 'imageCreateFromJpeg failed');
    echo "blackwhite start\n";
    blackwhite($img, 0.51, 1.0);
    imagejpeg($img, $image_file . "_bw.jpg", 100);
    echo "reduce_noise start\n";
    reduce_noise($img, 4, 4, 8);
    $file_img = $image_file . '.new.jpg';
    imagejpeg($img, $file_img, 100);
    $cmd = "tesseract -psm 7 $file_img stdout";
    shell_exec_realtime_output($cmd);
    //unlink($file_img);
}
