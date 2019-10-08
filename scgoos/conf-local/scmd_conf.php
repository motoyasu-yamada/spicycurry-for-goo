<?php
//  パッケージ名
define('SCMD_PACKAGE_NAME','goo-local');
//  ASSERTを有効にするかどうか
define('SCMD_ASSERT_ENABLE',     true);
//  JAMファイルのAppNameに強制的にサフィックスをつけるかどうか
define('SCMD_JAD_APPNAME_PREFIX','YMD-');

//  パッケージアプリパス
define('SCMD_PACKAGE_APP_DIRECTORY','c:/eclipse/workspace/scgoo/output/goo_bin');
//  パッケージリソースパス
define('SCMD_PACKAGE_RES_DIRECTORY','c:/eclipse/workspace/scgoo/output/goo_res');
//  パッケージテンプレートパス
define('SCMD_PACKAGE_HTM_DIRECTORY','c:/eclipse/workspace/scgoo/scgoos/template');

//  ログ出力する
define('SCMD_LOG_TRACE',    1);
//  ログファイルの保存ディレクトリ
define('SCMD_LOG_DIRECTORY','c:/eclipse/workspace/scgoo/temp/log');

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
 *  お知らせ情報ローカルパス
 */
define('GOO_INFO_PATH',"c:/eclipse/workspace/scgoo/scgoos/conf-local/info.txt");
/**
 *  新着メール情報通知:ホスト(shiva.mail.goo.ne.jp)
 */
define('GOO_NEW_MAIN_COUNT_HOST','cm150.mail.goo.ne.jp');
/**
 *  新着メール情報通知:パス(/info/arrival.ghtml)
 */
define('GOO_NEW_MAIL_COUNT_PATH','/info/arrival.ghtml');

?>
