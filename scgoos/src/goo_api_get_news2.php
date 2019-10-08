<?php

require_once 'scm_stream.php';
require_once 'scm_misc.php';
require_once 'scm_imode.php';

/**
 *  お知らせ配信時刻を指定しない場合
 */
define('GOO_INFO_NODATE','------------');
define('GOO_NEWS_DUMMY_TITLE','gooニュース');
define('GOO_NEWS_DUMMY_URL',  'http://gw.mobile.goo.ne.jp/gw/http/news.goo.ne.jp/news/mobile/');

function goo_api_get_news2($user_id, $req, &$res)
{
    $action = new GetNews2();
    $action->action($req,$res);
}

function goo_api_get_news1($user_id, $req, &$res)
{
    $action = new GetNews1();
    $action->action($req,$res);
}

function goo_api_get_info1($user_id, $req, &$res)
{
    $action = new GetInfo1();
    $action->action($req,$res);
}

/**
 *  モバイルGOOアプリ ver1通信用処理
 */
class GetNews1 extends GooNews
{
    /**
     *  処理
     */
    function action($req,&$res)
    {
        assert(is_a($req,'SCM_InputStream'));
        assert(is_a($res,'SCM_OutputStream'));

        $dat  = $this->getNews();
        $news = $this->parseNews($dat);

        $this->write($res,$news);
    }

    /**
     *  処理結果をアプリに出力/応答する<br>
     *
     */
    function write(&$res,$news)
    {
        $res->writeResultOK();
        $newsCount = count($news);
        for ($n = 0;$n < $newsCount;$n++) {
            $res->writeString16X($news[$n][0]);
            $res->writeString16X($news[$n][1]);
        }
        while ($n < 5) {
            $res->writeString16X(GOO_NEWS_DUMMY_TITLE);
            $res->writeString16X(GOO_NEWS_DUMMY_URL);
        }
    }
}

/**
 *  モバイルGOOアプリ ver1通信用処理
 */
class GetInfo1 extends GooNews
{
    /**
     *  処理
     */
    function action($req,&$res)
    {
        assert(is_a($req,'SCM_InputStream'));
        assert(is_a($res,'SCM_OutputStream'));

        $url  = GooNews::getInfoURL(); 
        $dat  = $this->getFile($url);
        $info = $this->parseInfo($dat);

        $this->write($res,$info);
    }

    /**
     *  処理結果をアプリに出力/応答する<br>
     *
     */
    function write(&$res,$info)
    {
        if ($info) {
            $res->writeResultOK();
            $res->writeString16X($info[0]);
            $res->writeString16X($info[1]);
        } else {
            $res->writeResultNO();
        }
    }
}

/**
 *  モバイルGOOアプリ ver2通信用処理
 */
class GetNews2 extends GooNews
{
    /**
     *  処理
     */
    function action($req,&$res)
    {
        assert(is_a($req,'SCM_InputStream'));
        assert(is_a($res,'SCM_OutputStream'));
        $aver = $req->readString16();

        $uver = $this->getCurrentVersion();
        $requireVersionup = $aver !== $uver;

        $url  = GooNews::getInfoURL(); 
        $dat  = $this->getFile($url);
        $info = $this->parseInfo($dat);

        $dat  = $this->getNews();
        $news = $this->parseNews($dat);

        $this->write($res,$requireVersionup,$info,$news);
    }

    /**
     *  処理結果をアプリに出力/応答する<br>
     *
     */
    function write(&$res,$requireVersionup,$information,$news)
    {
        $res->writeResultOK();
        $res->writeUI16($requireVersionup  ? ord('1') : ord('0'));
        $res->writeUI16($information       ? ord('1') : ord('0'));
        if ($information) {
            $res->writeString16X($information[0]);
            $res->writeString16X($information[1]);
        } else {
        }
        $newsCount = count($news);
        $res->writeUI16($newsCount);
        for ($n = 0;$n < $newsCount;$n++) {
            $res->writeString16X($news[$n][0]);
            $res->writeString16X($news[$n][1]);
        }
    }
}

/**
 * <h2>通信仕様</h2>
 * <pre>
 *  UI16    バージョンアップが必要かどうか      '1' / '0'
 *  UI16    インフォメーションがあるかどうか    '1' / '0'
 *      STR16   インフォメーションタイトル
 *      STR16   インフォメーションURL
 *  UI16    ニュース件数
 *      STR16   ニュースタイトル
 *      STR16   ニュースURL
 *  </pre>
 *
 * <h2>お知らさせファイル仕様</h2>
 * <pre>
 *      一行目: お知らせなし:   "none\n"
 *              お知らせあり:   "exists\n"
 *              タイマ配信:     "timer:YYYYMMDDHHMM\n"
 *      二行目: <お知らせ情報> \t <お知らせURL>
 * </pre>
 */
class GooNews 
{
    /**
     *  お知らせ取得URLを取得する
     *  @static
     */
    function getInfoURL()
    {
        return  GOO_INFO_PATH;
    }

    /**
     * ニュース取得URLを取得する
     *  @static
     */
    function getNewsURL()
    {
        return "http://news.goo.ne.jp/private/news/MOBILE/inews.txt";
    }

    /**
     *  ファイルを取得する<br>
     *  @return 取得できなかった場合はfalse
     */
    function getFile($url)
    {
        $fp  = @fopen($url,"r");
        if (!$fp) {
            scm_fatal_api($url);
        }
        $news = '';
        while ($result = @fgets($fp,1024)) {
            $news .= $result;
        }
        @fclose($fp);
        return $news;
    }

    /**
     *  ニュースを取得する<br>
     * @return 取得したニュースデータファイル(EUCJP-WIN)にエンコード済
     */
    function getNews()
    {
        $url  = GooNews::getNewsURL();
        $dat  = $this->getFile($url);
        return @mb_convert_encoding($dat,'EUCJP-WIN','Windows-31J');
    }

    /**
     *  ニュースデータを解析する<br>
     *  @param $data string ニュースデータ
     *  @return array 文字列配列 [NEWS件数][0:タイトル、1:URL]
     */
    function parseNews($data)
    {
        if (!$data) {
            scm_fatal_api("empty news.");
        }
        $lines = split("\n",$data);
        $newss = array();
        foreach($lines as $line) {
            $line = trim($line);
            if ($line === '') {
                break;
            }
            $cols = split("\t",$line);
            if (!is_array($cols) || count($cols) != 2) {
                scm_fatal_api($line);
            }
            list($title,$url) = $cols;
            $news[0] = $title;
            $news[1] = $url;
            $newss[] = $news;
        }
        return $newss;
    }

    /**
     *  アプリ最新バージョンを取得する
     *  @return string 最新バージョン文字列
     */
    function getCurrentVersion()
    {
        $jam = $this->getJam();
        $ver = $this->getJamVersion($jam);
        return $ver;
    }


    /**
     *  JAMファイルからアプリバージョンを取得する<br>
     *  @param $jam string JAMファイル
     *  @return string アプリバージョン
     */
    function getJamVersion($jam)
    {
        if (!preg_match("/^AppVer *= *(.+)$/mi",$jam,$matches)) {
            scm_fatal_api($jam);
        }
        return trim($matches[1]);
    }

    /**
     * JAMファイルを読み込む<br>
     *  @return string JAMファイル実体
     */
    function getJam()
    {
        $fname   = scm_get_app_fname();
        $jampath = SCMD_PACKAGE_APP_DIRECTORY.'/'.$fname.'.jam';
        $jam     = $this->getFile($jampath);
        return @mb_convert_encoding($jam,mb_internal_encoding(),'SJIS');
    }

    /**
     * お知らせ情報があるかどうかを調べる
     *  @return mixed false -   お知らせが存在しない場合<br>
     *                string[2] お知らせが存在する場合<br>
     *                [0] タイトル<br>
     *                [1] URL<br>
     */
    function parseInfo($data)
    {
        if (!$data) {
            scm_fatal_api("empty info.");
        }
        $lines = split("\n",$data);
        if (!is_array($lines)) {
            scm_fatal_api($data);
        }
        $head = $lines[0];
        if ($head === "none") {
            return false;
        } else if ($head === "exists") {
        } else if (substr($head,0,6) === "timer:") {
            $start   = substr($head,6,12);
            $end     = substr($head,18,12);
            $current = date("YmdHi");
            assert(!($start == GOO_INFO_NODATE && $end != GOO_INFO_NODATE));
            if ($start != GOO_INFO_NODATE &&
                strcmp($current,$start) < 0) {
                return false;
            }
            if ($end != GOO_INFO_NODATE &&
                0 < strcmp($current,$end)) {
                return false;
            }
        } else {
            scm_fatal_api($head);
        }
        if (count($lines) < 2) {
            scm_fatal_api($data);
        }
        $info = $lines[1];
        $cols = split("\t",$info);
        if (!is_array($cols) || count($cols) != 2) {
            scm_fatal_api($info);
        }
        return $cols;
    }
}

?>