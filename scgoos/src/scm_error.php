<?php
//  $Id: scm_error.php,v 1.1.1.1 2005/03/04 10:31:17 m_yamada Exp $

/*
*   ユーザエラーコード
*/
define('SCM_UERR_UNKNOWN',                 0000);
//  下記コード
define('SCM_UERR_NO_ACCOUNT',              1000);
define('SCM_UERR_CRITICAL_DB_ERROR',       1001);
define('SCM_UERR_ILLEGAL_REQUEST',         1002);
define('SCM_UERR_POP_GET_MAIL_COUNT',      2001);
define('SCM_UERR_POP_GET_MAIL_HEAD',       2002);
define('SCM_UERR_POP_MAIL_EXPIRED',        2003);
define('SCM_UERR_POP_RETR_MAIL',           2004);
define('SCM_UERR_IMAP_GET_BODY',           2005);
define('SCM_UERR_POP_GET_ATTACH',          2006);
define('SCM_UERR_POP_OVER_SIZE_ATTACH',    2007);
define('SCM_UERR_NO_ACCOUNT_LIST',         2008);
define('SCM_UERR_CRITICAL_EXPORT_ADDBOOK', 2009);
define('SCM_UERR_NO_ADDBOOK',              2010);
define('SCM_UERR_SMTP_CONNECTION',         2011);
define('SCM_UERR_POP_NO_CONNECTION',       2012);
define('SCM_UERR_CONNECTION_REFUSED',      2013);
define('SCM_UERR_PASSWORD_EMPTY',          2014);
define('SCM_UERR_INVALID_PASSWORD',        2015);
define('SCM_UERR_SMTP_NOT_FINISHED',       2016);
define('SCM_UERR_SMTP_NO_RCPT_TO',         2017);
define('SCM_UERR_INVALID_ACCOUNT_ID',      2018);

/*
*   ユーザエラーメッセージ文字列
*/
$SCM_UERR = array(
    SCM_UERR_NO_ACCOUNT             => array('そのアカウントは既に削除済みです。',
                                            'can not account from database.'),
    SCM_UERR_POP_GET_MAIL_COUNT    => array('受信サーバからメール件数を取得できませんでした。',
                                            'pop can not get_mail count.'),
    SCM_UERR_POP_GET_MAIL_HEAD     => array('受信サーバからメールヘッダを取得できませんでした。',
                                            'pop can not top mail.'),
    SCM_UERR_POP_MAIL_EXPIRED      => array('受信サーバからメールは既に削除されています。',
                                            'pop already deleted mail.'),
    SCM_UERR_POP_RETR_MAIL         => array('受信サーバからメールを取得できませんでした。',
                                            'pop can not retr mail.'),
    SCM_UERR_IMAP_GET_BODY         => array('受信サーバからメールを取得できませんでした。',
                                            'imap can not get mail.'),
    SCM_UERR_POP_GET_ATTACH        => array('指定した添付ファイルは存在しません。',
                                            'pop/imap can not find attach.'),
    SCM_UERR_POP_OVER_SIZE_ATTACH  => array('指定した添付ファイルは大きすぎます。',
                                            'pop/imap attach file is over size.'),
    SCM_UERR_NO_ACCOUNT_LIST        => array('アカウントが設定されていません。',
                                            'no accounts list.'),
    SCM_UERR_CRITICAL_DB_ERROR      => array('サーバーの致命的な障害',
                                            'db connection failed.'),
    SCM_UERR_CRITICAL_EXPORT_ADDBOOK=> array('致命的な障害でアドレス帳をエクスポートできませんでした。',
                                            'db: addbook insert error.'),
    SCM_UERR_NO_ADDBOOK             => array('アドレス帳はサーバーに登録されていません。',
                                            'addbook empty addbook.'),
    SCM_UERR_ILLEGAL_REQUEST        => array('不正な要求です',
                                            'illegal request'),
    SCM_UERR_SMTP_CONNECTION        => array('送信サーバに接続できませんでした。',
                                            'smtp connection failed.'),
    SCM_UERR_POP_NO_CONNECTION      => array('受信サーバに接続できませんでした。',
                                             'pop: no connection.'),
    SCM_UERR_CONNECTION_REFUSED     => array('受信サーバが接続を拒否しました。',
                                             'pop: connection refused'),
    SCM_UERR_PASSWORD_EMPTY         => array('パスワードが空です',
                                             'pop: password empty.'),
    SCM_UERR_INVALID_PASSWORD       => array('パスワードが間違っています',
                                             'pop: invaid password.'),
    SCM_UERR_SMTP_NOT_FINISHED      => array('送信中に障害が発生しました',
                                             'smtp: not finished.'),
    SCM_UERR_SMTP_NO_RCPT_TO        => array('送信先が間違っています',
                                             'smtp: invalid rcpt to.'),
    SCM_UERR_INVALID_ACCOUNT_ID     => array('そのアカウントは既に削除されています',
                                             'account id is 0xff'),
);

?>