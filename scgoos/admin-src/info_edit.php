<?php
    require_once 'info.php';
    function action()
    {
        $info = new GooInfo();
        $info->load();
        $info->paste();
        include_once 't_info_edit.php';
    }
?>

