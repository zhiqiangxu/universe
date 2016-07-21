<?php
namespace Model;


class Project extends Base
{
    const Table = 'project';

    const F_id = 'id', F_kickoff_date = 'kickoff_date', F_deadline = 'deadline',
          F_online_date = 'online_date', F_dev_branch = 'dev_branch', F_description = 'description',
          F_rd = 'rd', F_qa = 'qa', F_pm = 'pm';

    //后台用
    function getList($where, $limit)
    {
        $db = $this->getMysql();

        $total = $db->count(self::Table, $where);

        $list = $db->find(self::Table, $where, $limit);

        return [$total, $list];
    }


    function getOne($id)
    {
        $db = $this->getMysql();

        return $db->get(self::Table, [self::F_id => $id]);
    }


}
