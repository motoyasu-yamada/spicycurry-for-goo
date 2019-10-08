<?php
//  パッケージ名
define('SCMD_PACKAGE_NAME','goo-yamada');
//  ASSERTを有効にするかどうか
define('SCMD_ASSERT_ENABLE',     true);
//  JAMファイルのAppNameに強制的にサフィックスをつけるかどうか
define('SCMD_JAD_APPNAME_PREFIX','YMD-');

//  パッケージアプリパス
define('SCMD_PACKAGE_APP_DIRECTORY','c:/doc/cvswork/scgoo/output/goo_bin');
//  パッケージリソースパス
define('SCMD_PACKAGE_RES_DIRECTORY','c:/doc/cvswork/scgoo/output/goo_res');
//  パッケージテンプレートパス
define('SCMD_PACKAGE_HTM_DIRECTORY','c:/doc/cvswork/scgoo/scgoo/scgoos/template');

//  ログ出力する
define('SCMD_LOG_TRACE',    1);
//  ログファイルの保存ディレクトリ
define('SCMD_LOG_DIRECTORY','c:/doc/cvswork/scgoo/temp/log');

/**
 *  データベースへの接続
 */
$SCMD_DSN = array(
    'phptype'  => "mysql",
    'hostspec' => "localhost",
    'database' => "db_goo",
    'username' => "root",
    'password' => ""
);

/**
 *  お知らせ情報ローカルパス
 */
define('GOO_INFO_PATH',"c:/doc/cvswork/scgoo/scgoo/scgoos/conf-yamada/info.txt");
/**
 *  新着メール情報通知:ホスト(shiva.mail.goo.ne.jp)
 */
define('GOO_NEW_MAIN_COUNT_HOST','cm150.mail.goo.ne.jp');
/**
 *  新着メール情報通知:パス(/info/arrival.ghtml)
 */
define('GOO_NEW_MAIL_COUNT_PATH','/info/arrival.ghtml');

?>
