<?php
    require_once 'log.php';


    function action()
    {
        $access = execute();
        if (is_array($access)) {
            if (0 < count($access)) {
                include_once 't_unique_done.php';
            } else {
                include_once 't_unique_done_empty.php';
            }
        } else {
            include_once 't_unique_done_err.php';
        }
    }
    
    function execute()
    {
        $r = $_REQUEST;
        $start = load_date($r,'starty','startm','startd');
        $end   = load_date($r,'endy',  'endm',  'endd'  );
        if (is_a($start,'INVALID_DATE') || is_a($end,'INVALID_DATE')) {
            return false;
        }
        if ($start && $end && 0 < strcmp($start,$end)) {
            return false;
        }
        $access = AccessLogLoader::getUnuqueLog($start,$end);
        return $access ;
    }


?>