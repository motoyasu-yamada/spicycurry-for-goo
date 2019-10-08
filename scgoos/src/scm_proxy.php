<?php
//  All rights reserved by Spicysoft Corporation. 2002-2003

require_once 'scm_imode.php';
require_once 'scm_env.php';
require_once 'scm_request.php';
require_once 'scm_api.php';
require_once 'scm_web_download.php';
require_once 'scm_get_jam.php';
require_once 'scm_get_jar.php';
require_once 'scm_get_res.php';
require_once 'scm_log.php';
require_once 'scm_stream.php';
require_once 'goo_api_get_arrival_mail_count.php';
require_once 'goo_api_get_news2.php';

/**
 *  プロキシー処理メイン
 */
function proxy()
{
    //  最大実行時間の設定
    set_time_limit(0);

    // コールバックを設定する
    if (SCMD_ASSERT_ENABLE) {
        assert_options (ASSERT_ACTIVE,    1);
        assert_options (ASSERT_WARNING,   1);
        assert_options (ASSERT_QUIET_EVAL,1);
        assert_options (ASSERT_CALLBACK,  'scm_assert_handler');
    } else {
        assert_options (ASSERT_ACTIVE,    0);
        assert_options (ASSERT_WARNING,   0);
        assert_options (ASSERT_QUIET_EVAL,0);
        assert_options (ASSERT_CALLBACK,  'scm_assert_handler');
    }

    global $HTTP_RAW_POST_DATA;
    global $SCM_REQUEST;
    $request_uri = $_SERVER['REQUEST_URI'];
    scm_parse_request();

    SCM_Log::trace("request_url : " . $request_uri);

    //  ダウンロードページ
    if (preg_match('/download\.htm/i',$request_uri)) {
        scm_web_download();
        return;
    }
    //  その他のHTMLページ
    if (preg_match('/([A-Za-z0-9\-_]+)\.htm/i',$request_uri,$matches) ||
        preg_match('/(index)\.php/i',$request_uri,$matches)) {
        $htm = $matches[1];
        $htm = SCMD_PACKAGE_HTM_DIRECTORY."/".$htm.".htm";
        include_once $htm;
        return;
    }
    

    //  ユーザIDを取得する
    if (!preg_match("/(.+\/)?([0-9a-zA-Z\-]+)\/[^\/]+/i",$request_uri,$matches)) {
        SCM_Log::error("Illegal Request ($request_uri)");
        return false;
    }
    $user_id                = $matches[2];
    $SCM_REQUEST['user_id'] = $user_id;

    SCM_Log::trace("request_uri = ".$request_uri);
    SCM_Log::trace("user_id     = ".$user_id);
    
    //  ダウンロード(JAM)
    if (preg_match('/([A-Za-z0-9\-]+)\/scm\.jam$/',$request_uri,$matches)) {
        SCM_Log::download();
        scm_get_jam($user_id);
        return;
    }
    //  ダウンロード(JAR)
    if (preg_match('/([A-Za-z0-9\-]+)\/scm\.jar$/',$request_uri,$matches)) {
        SCM_Log::download();
        scm_get_jar($user_id);
        return;
    }
    //  ダウンロード(RES)
    if (preg_match('/([A-Za-z0-9\-]+)\/res\.([A-Za-z0-9\.]+)$/',$request_uri,$matches)) {
        SCM_Log::trace("get RES!!" . $request_uri);
        scm_get_res($user_id,$matches[2]);
        return;
    }

    //  携帯アプリAPI通信

    if (preg_match('/([A-Za-z0-9\-]+)\/api\.(\d+)$/',$request_uri,$matches)) {
        $api_id  = (int)$matches[2];
        $req     = new SCM_InputStream($HTTP_RAW_POST_DATA);
        $res     = new SCM_OutputStream();

        SCM_Log::trace("api_id:".$api_id);
        switch($api_id) {
        case SCM_PROXY_API_GET_ARRIVAL_MAIL:
            goo_api_get_arrival_mail_count($user_id,$req,$res);
            break;
        case SCM_PROXY_API_GET_NEWS:
            goo_api_get_news1($user_id,$req,$res);
            break;
        case SCM_PROXY_API_GET_INFO:
            goo_api_get_info1($user_id,$req,$res);
            break;
        case SCM_PROXY_API_GET_NEWS2:
            goo_api_get_news2($user_id,$req,$res);
            break;
        default:
            assert(false);
            break;
        }

        assert($res->getLength() <= $SCM_REQUEST['downstream_size']);
        SCM_Log::api($res);

        //  API応答を標準出力に出力する
        header("Content-Length: ".$res->getLength());
        echo $res->toString();
        return;
    }
    SCM_Log::error("Illegal Request ($request_uri)");
    assert(false);
}

/**
 *  カスタム assertコールバックハンドラ
 */
function scm_assert_handler($file, $line, $code)
{
    $msg   = "*** ASSERT *** '$file'($line) : '$code'";
    echo $msg;
    SCM_Log::error($msg);
    exit;
}

?>