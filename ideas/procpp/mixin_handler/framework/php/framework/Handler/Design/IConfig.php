<?php
namespace Handler\Design;

interface IConfig extends \ArrayAccess
{

    //添加搜索路径
    function addPath($dir);

}
