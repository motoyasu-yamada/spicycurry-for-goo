<?php
    require_once 'log.php';

    function action()
    {
        $r = execute();
        if (is_array($r)) {
            list($dates,$domains,$start,$end) = $r;
            if (0 < count($domains)) {
                set_param('dates_count',count($dates));
                set_param('startd',$start);
                set_param('endd',  $end);
                include_once 't_domain_done.php';
            } else {
                include_once 't_domain_done_empty.php';
            }
        } else {
            include_once 't_domain_done_err.php';
        }
    }

    function execute()
    {
        $r = $_REQUEST;
        $start = load_date($r,'starty','startm','startd',false);
        $end   = load_date($r,'endy',  'endm',  'endd'  ,false);
        if (is_a($start,'INVALID_DATE') || is_a($end,'INVALID_DATE')) {
            return false;
        }
        if ($start && $end && 0 <= strcmp($start,$end)) {
            return false;
        }
        
        $dates = AccessLogLoader::makeDates($start,$end);
        $datec = count($dates);
        if ($datec == 0 || 60 < $datec) {
            return false;
        }
        $access = AccessLogLoader::getDomainLog($dates);
        return array($dates,$access,$start,$end);
    }

?>