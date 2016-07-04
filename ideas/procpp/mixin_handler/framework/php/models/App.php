<?php
namespace Model;


class App extends Base
{
    const Table = 'app';

    const F_id = 'id', F_secret = 'secret', F_department = 'department', F_ctx = 'ctx';

    function verify($app_id, $app_secret)
    {
        $row = $this->getCache($app_id);

        if (!$row) {

            $db = $this->getMysql();
            $row = $db->get(self::Table, [self::F_id => $app_id]);
            if (!$row) return false;

            $this->setCache($app_id, $row);

        }

        return $app_secret == $row[self::F_secret];
    }

}
