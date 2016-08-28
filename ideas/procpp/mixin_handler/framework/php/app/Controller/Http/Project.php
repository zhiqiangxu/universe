<?php

namespace App\Controller\Http;

use App\Model\Project as ProjectModel;
use Handler\MVC\Http\Controller;

class Project extends Controller
{
    const DEFAULT_PAGE = 1;
    const DEFAULT_PAGE_SIZE = 10;

    function index()
    {
        $page = empty($_GET['page']) ? self::DEFAULT_PAGE : $_GET['page'];
        $page_size = empty($_GET['page_size']) ? self::DEFAULT_PAGE_SIZE : $_GET['page_size'];

        $limit = (($page - 1) * $page_size) . ",$page_size";

        $project = new ProjectModel();
        list($total, $project_list) = $project->getList(['true', 'limit' => $limit]);

        $this->assign([
            'page'      => $page,
            'page_size' => $page_size,
            'total'     => $total,
            'project_list' => json_encode($project_list),
        ]);
        $this->outputHtml('project/index.html');
    }
}
