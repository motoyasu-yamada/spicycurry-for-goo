<?php
    require_once 'info.php';
    function action()
    {   
        $r = $_REQUEST;
        @mb_convert_variables('EUCJP-WIN','Windows-31J',$r);
        $info = new GooInfo();
        if ($info->loadFromRequest($r)) {
            $info->save();
            include_once 't_info_done.php';
        } else {
            include_once 't_info_done_err.php';
        }
    }
?>