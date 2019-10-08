<?php
    ini_set('include_path',"../conf-yamada;../src;".ini_get('include_path'));

    require_once 'scm_test.php';
    require_once 'log.php';

    echo "getUnuqueLog\n";
    print_r(AccessLogLoader::getUnuqueLog(false,false));

    echo "getDomainLog\n";
    print_r(AccessLogLoader::getDomainLog("20050225","20050304"));

    echo "makeDates";
    print_r(AccessLogLoader::makeDates("20050225","20050304"));
    
?>