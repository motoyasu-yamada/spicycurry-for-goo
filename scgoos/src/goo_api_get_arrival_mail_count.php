<?php

require_once 'scm_stream.php';

/**
*   gooからメールの新着受信件数を取得する.
*   @param  int          $user_id ユーザID
*   @param  InputStream  $req     入力ストリーム
*   @param  OutputStream $res     出力ストリーム
*/
function goo_api_get_arrival_mail_count($user_id,$req,&$res) {
    assert(isset($user_id));
    assert(is_a($req,'SCM_InputStream'));
    assert(is_a($res,'SCM_OutputStream'));

    $host    = GOO_NEW_MAIN_COUNT_HOST;
    $port    = 80;
    $timeout = 5;
    $fp = fsockopen($host, $port, $errno, $errstr, $timeout);
    if (!$fp) {
        scm_fatal_api("$errno : $errstr");
    }

    $body = "u=" . $req->readString16();
    $body_length = strlen($body);

    $postdata = "POST " . GOO_NEW_MAIL_COUNT_PATH ." HTTP/1.0\r\n".
                "Content-Type: application/x-www-form-urlencoded\r\n".
                "Content-Length: $body_length\r\n".
                "\r\n".
                $body."\r\n";

    fputs($fp,$postdata);
    SCM_Log::trace("postdata = ". $postdata);

    $result = "";
    while (!feof($fp)) {
        $result .= fgets($fp,1024);
    }
    fclose($fp);

    SCM_Log::trace($result);

    res_arrival_mail_count($result,$res);
}

/**
*   携帯電話に取得した新着メール件数を返す
*   @param  result  HTTP Response
*   @param  res     出力ストリーム
*/
function res_arrival_mail_count($result,&$res) {
    $result = str_replace("\r\n","\n",$result);
    $result = str_replace("\r","\n",$result);
    list($head,$body) = split("\n\n",$result,2);
    $body = str_replace("\n","",$body);
    $count = @mb_strimwidth($body,0,64,'...','SJIS');
    $res->writeResultOK();
    $res->writeString16 ($count);
}

?>