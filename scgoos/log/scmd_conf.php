<?php
 /**
 *  データベースへの接続
 */
$SCMD_DSN = array(
    'phptype'  => "mysql",
    'hostspec' => "sql",
    'database' => "db_goo",
    'username' => "u_goo",
    'password' => "mA64nCnGqZopgd"
);
/**
 *  ログ解析用設定
 */
define('LA_LOGDIR',   '/home/logs/sc-goo/spicyproxy');
/**
 *  ログ解析用設定
 */
define('LA_LOGDSTDIR','/home/logs/sc-goo/spicyproxy');
/**
 *  ログ解析用設定
 */
define('LA_CLASSPATH','/home/webapps/scgoo/WEB-INF/lib/mysql.jar:/home/webapps/scgoo/WEB-INF/classes');
/**
 *  ログ解析用設定
 */
define('LA_JAVA',     '/usr/java/j2sdk/bin/java');
/**
 *  解析対象ログファイル名
 */
$LA_LOGS = array('java01','java02');
?>