<?php
    require_once 'info.php';

    function action()
    {   
        $r = $_REQUEST;
        @mb_convert_variables('EUCJP-WIN','Windows-31J',$r);

        $info = new GooInfo();
        if ($info->loadFromRequest($r)) {
            $org = new GooInfo();
            $org->load();
            $org->paste('org');
            $info->paste();

            include_once 't_info_confirm.php';
        } else {
            include_once 't_info_confirm_err.php';
        }
    }

?>