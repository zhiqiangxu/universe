<?php
namespace App\Model;

use Handler\MVC\Model;


class Project extends Model
{
    const TABLE = 'project';
    const DB_GROUP = 'default';

    const F_id = 'id', F_kickoff_date = 'kickoff_date', F_deadline = 'deadline',
          F_online_date = 'online_date', F_dev_branch = 'dev_branch', F_description = 'description',
          F_rd = 'rd', F_qa = 'qa', F_pm = 'pm';

    //后台用
    function getList($params)
    {

        $list = $this->find($params);

        $total = $this->count($params);


        return [$total, $list];
    }


    function getOne($id)
    {
        $db = $this->getMysql();

        return $db->get(self::Table, [self::F_id => $id]);
    }


}
