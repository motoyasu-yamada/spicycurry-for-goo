<?php
    require_once 'scmd_conf.php';

    $dsn_type     = $SCMD_DSN['phptype'];
    $dsn_host     = $SCMD_DSN['hostspec'];
    $dsn_db       = $SCMD_DSN['database'];
    $dsn_user     = $SCMD_DSN['username'];
    $dsn_password = $SCMD_DSN['password'];
    define('LA_DSN',      "jdbc:${dsn_type}://${dsn_host}/${dsn_db}?user=${dsn_user}&password=${dsn_password}");

    foreach($LA_LOGS as $log) {
        analize('access.' . $log);
    }

    function analize($log)
    {
        $orglog = LA_LOGDIR .  '/'.$log;
        $dstlog = LA_LOGDSTDIR.'/'.date('YmdHis').'.'.$log;
        $dsn  = LA_DSN;
        $cp   = LA_CLASSPATH;
        $java = LA_JAVA;
        $plog = LA_LOGDSTDIR.'/trace.log';

        `echo 'LOG ANALIZE $orglog => $dstlog' >>$plog`;

        $cmd    = "mv $orglog $dstlog";
        `$cmd >>$plog 2>&1`;

        $cmd  = "$java -cp $cp spicyproxy.accesslog.Analizer \"$dsn\" \"$dstlog\" 2";
        `$cmd >>$plog 2>&1`;
    }

?>