<?php
/**
 *  ASSERTを有効にするかどうか
 */
define('SCMD_ASSERT_ENABLE',     true);
/**
 *  JAMファイルのAppNameに強制的にサフィックスをつけるかどうか
 */
define('SCMD_JAD_APPNAME_PREFIX','RD-');

/**
 *  パッケージアプリパス
 */
define('SCMD_PACKAGE_APP_DIRECTORY','/home/sites/sc-goo.spicysoft.com/goo1.1/debug/goo_bin');
/**
 *  パッケージリソースパス
 */
define('SCMD_PACKAGE_RES_DIRECTORY','/home/sites/sc-goo.spicysoft.com/goo1.1/debug/goo_res');
/**
 *  ダウンロードページのテンプレート
 */
define('SCMD_PACKAGE_HTM_DIRECTORY','/home/sites/sc-goo.spicysoft.com/goo1.1/debug/template');
/**
 *  ログファイルの保存ディレクトリ
 */
define('SCMD_LOG_DIRECTORY','/home/logs/sc-goo/debug');
/**
 *  お知らせ情報ローカルパス
 */
define('GOO_INFO_PATH',     '/home/sites/sc-goo.spicysoft.com/goo1.1/debug-conf/info.txt');

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
 *  ログを出力する場合は1.しない場合は0.
 */
define('SCMD_LOG_TRACE',    1);
/**
 *  新着メール件数を問い合わせするHOST  goo商用環境
 */
define('GOO_NEW_MAIN_COUNT_HOST',"shiva.mail.goo.ne.jp");
/**
 *  新着メール件数問い合わせのためPOSTするURL    goo商用環境
 */
define('GOO_NEW_MAIL_COUNT_PATH',"/info/arrival.ghtml");

?>