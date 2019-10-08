<?php

    define('CLASSPATH','./:./mysql.jar:/home/webapps/test-04/WEB-INF/classes');
    define('LOGDIR',   '/home/webapps/test-04/WEB-INF/logs');
    define('DSN',      'jdbc:mysql://localhost/db_goo?user=u_goo&password=mA64nCnGqZopgd ');

    analize('access.sql');

    function analize($log)
    {
        $orglog = LOGDIR . '/'.$log;
        $dstlog = './'.date('YmdHis').'.'.$log;
        $cmd    = "mv $orglog $dstlog";
        echo $cmd."\n";
        `$cmd`;

        $dsn  = DSN;
        $cp   = CLASSPATH;
        $cmd  = "java -cp $cp spicyproxy.accesslog.AccessAnalizer \"$dsn\" \"$dstlog\"";
        echo $cmd."\n";
        `$cmd`;
    }
?>
