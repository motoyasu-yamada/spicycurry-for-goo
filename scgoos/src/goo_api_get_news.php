<?php

require_once 'scm_stream.php';

/**
*   gooのニュースを取得する
*   @param  int          $user_id ユーザID
*   @param  InputStream  $req     入力ストリーム
*   @param  OutputStream $res     出力ストリーム
*
*       ニュースの形式
*           タブ区切り  タイトル32バイト以内    URL
*           必ず5行用意されている
*/
function goo_api_get_news($user_id, $req, &$res) {

    assert(isset($user_id));
    assert(is_a($req,'SCM_InputStream'));
    assert(is_a($res,'SCM_OutputStream'));

    //  POSTする
    $host = "news.goo.ne.jp";
    SCM_Log::trace("host:".$host);
    $port = 80;
    $timeout = 5;

    $fp = @fopen("http://news.goo.ne.jp/private/news/MOBILE/inews.txt","r");
    if (!$fp) {
        //SCM_Log::error("News load fail...");
        //echo "load fail";
        exit;
    }

    $res->writeResultOK();

    $result = "";
    
    for ($i=0;$i<5;$i++) {
    //while (!feof($fp)) {
    //  gooのテキストファイルの最後に空白があるのでfor文で5行に切り替え
    //  ニュースは必ず5行提供されることになっている
        $result = fgets($fp,1024);
        res_divide_news($result,$res);
    }
    
    fclose($fp);
}

/**
*   gooのお知らせを取得する
*   @param  int          $user_id ユーザID
*   @param  InputStream  $req     入力ストリーム
*   @param  OutputStream $res     出力ストリーム
*
*       お知らせの形式
*           タブ区切り  タイトル32バイト以内    URL
*/
function goo_api_get_info($user_id, $req, &$res) {

    /*
    assert(isset($user_id));
    assert(is_a($req,'SCM_InputStream'));
    assert(is_a($res,'SCM_OutputStream'));
    */

    $host = "scx.spicysoft.com";
    SCM_Log::trace("host:".$host);
    $port = 80;
    $timeout = 5;

    $fp = @fopen("http://scx.spicysoft.com/goo/gooInfo.txt","r");
    if (!$fp) {
//        SCM_Log::error("News load fail...");
//        echo "load fail";
        exit;
    }

    $res->writeResultOK();

    $result = "";
    
    $result = fgets($fp,1024);
    res_divide_news($result,$res);
    
    fclose($fp);
}

/**
*   取得したニュースを、タイトル・URLを分けてアプリに返す
*/
function res_divide_news($result,&$res){
    SCM_Log::trace($result);
    
    $splits = split("\t",$result);
    if (is_array($splits) && count($splits) == 2) {
        list($title,$url) = $splits;
    } else {
        // ニュースがもし5行無かった場合はgooニュースへのリンクを表示する
        $title = mb_convert_encoding("gooニュース", "SJIS");
        $url   = "http://gw.mobile.goo.ne.jp/gw/http/news.goo.ne.jp/news/mobile/";
    }
    
    $res->writeString16($title);
    $res->writeString16($url);
}

?>