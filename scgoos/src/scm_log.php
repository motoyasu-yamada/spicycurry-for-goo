<?php
//  $Id: scm_log.php,v 1.1.1.1 2005/03/04 10:31:17 m_yamada Exp $
require_once 'scmd_conf.php';
require_once 'scm_misc.php';

//  ログを一回で出力できるサイズ (1K)
define('SCM_LOG_MAX_OUTPUT',4096);
//  APIダンプの最大サイズ
define('SCM_LOG_MAX_API_DUMP',1024);

class SCM_Log {
    /*
    *   変数の文字列表現をトレースファイルに出力する
    *   @static
    *   @access public
    *   @param  mixed   $var    出力する変数
    */
    function trace_export($var)
    {
        @ob_start();
        @var_dump($var);
        $msg = @ob_get_contents();
        @ob_end_clean();
        SCM_Log::trace($msg);
    }

    /*
    *   変数を出力する
    *   @static
    *   @access public
    *   @param  $name   変数名
    *   @param  $value  変数値
    */
    function trace_var($name,$value)
    {
        SCM_Log::trace("$name = '$value'");
    }

    /*
    *   *** SCMD_LOG_TRACE が有効な場合のみ出力される ***
    *   @static
    *   @access public
    *   @param  String $msg
    */
    function trace($msg)
    {
        //  トレースログ出力がOFFになっている場合は出力しない
        if (SCMD_LOG_TRACE != 1) {
            return;
        }

        SCM_Log::_log('trace.log',$msg);
    }

    /*
    *   *** 常に出力されるログ ***
    *   API通信のダンプログ
    *   @static
    *   @access public
    *   @param  SCM_Response $res 出力データ
    */
    function api($res)
    {
        //  トレースログ出力がOFFになっている場合は出力しない
        if (SCMD_LOG_TRACE != 1) {
            return;
        }

        global $HTTP_RAW_POST_DATA;
        $in  = substr($HTTP_RAW_POST_DATA,0,SCM_LOG_MAX_API_DUMP);
        $out = substr($res->toString(),   0,SCM_LOG_MAX_API_DUMP);
        $uri = '';
        if (isset($_SERVER['REQUEST_URI'])) {
            $uri = $_SERVER['REQUEST_URI'];
        }
        $msg  = "[URI] ".$uri;
        $msg .= "\n";
        $msg .= "[REQ]\n";
        $msg .= scm_dump($in);
        $msg .= "\n";
        $msg .= "[RES]\n";
        $msg .= scm_dump($out);
        $msg .= "\n";

        SCM_Log::_log('api.log',$msg);
    }

    /*
    *   *** 常に出力されるログ ***
    *   下記のアプリケーションの利用に関するアクセスのログ
    *       api*            API通信
    *       res*            リソース取得
    *       account.php     アカウント設定
    *   @static
    *   @access private
    */
    function access()
    {
        $uri = '';
        if (isset($_SERVER['REQUEST_URI'])) {
            $uri = $_SERVER['REQUEST_URI'];
        }
        SCM_Log::_log('access.log',$uri);
        SCM_Log::trace($uri);
    }

    /*
    *   *** 常に出力されるログ ***
    *   下記のダウンロードに関するアクセスのログ。
    *       download.htm
    *       scm.jam
    *       scm.jar
    *   @static
    *   @access private
    */
    function download()
    {
        if (isset($_SERVER['REQUEST_URI'])) {
            SCM_Log::_log('download.log',$_SERVER['REQUEST_URI']);
        } else {
            SCM_Log::_log('download.log',"***");
        }
    }

    /*
    *   *** 常に出力されるログ ***
    *   重要なエラーに関するログを出力する
    *
    *   @static
    *   @access private
    *   @param  String $msg エラーメッセージ
    */
    function error($msg)
    {
        $msg .= "\n";
        $msg .= SCM_Log::get_stack_trace();

        SCM_Log::trace($msg);
        SCM_Log::_log('error.log',$msg);
    }

    /*
    *   共通フォーマットログ出力
    *
    *   [日付] サーバ名:ユーザID:メッセージ
    *   @static
    *   @access private
    *   @param  string  $fname ログファイル名
    *   @param  string  $msg   出力メッセージ
    */
    function _log($fname,$msg)
    {

        if (!is_string($msg)) {
            $msg = (string)$msg;
        }
        $msg = @mb_convert_encoding($msg,mb_internal_encoding(),'JIS,SJIS,EUC-JP');

        global $SCM_REQUEST;
        //  ログファイルのパスを求める
        $path = SCMD_LOG_DIRECTORY.'/'.$fname;

        if (@file_exists($path)) {
            //  空のファイル作成
            $file = @fopen($path,"a+");
            @fputs($file,'');
            @fclose($path);
            //  ログのパーミッションを 7777に変更
            if (@function_exists('chmod')) {
                @chmod($path,0777);
            }
        }

        //  各行のフォーマットを作成
        $date   = date("Y-m-d H:i:s");
        $ip     = scm_get_server_addr();
        $uid    = scm_get_array($SCM_REQUEST,'user_id');
        $dev_id = scm_get_array($SCM_REQUEST,'dev_id' );
        $head = "[$date] $ip/$uid/$dev_id";

        //  各行毎分割してフォーマッティングする
        $msg   = (string)$msg;
        $msg   = @rtrim($msg);
        $msg   = substr($msg,0,SCM_LOG_MAX_OUTPUT);
        $msg   = scm_replace("\r\n","\n",$msg);
        $msg   = scm_replace("\r",  "\n",$msg);
        $lines = @split("\n",$msg);
        $log   = '';
        foreach($lines as $line) {
            $log .= "$head $line\n";
        }

        //  ファイルにログを一括出力する
        $file = @fopen($path,"a+");
        if(!$file) {
            exit;
        }
        @fputs ($file,$log);
        @fclose($file);
    }

    /**
    *   デバッグ情報のトレース
    *   参考文献：WEB+DB PRESS Vol.12（技術評論社,ISBN4-7741-1623-8）,p.205
    *   Java printStackTraceと同様のスタックトレース表示を行う。
    */
    function get_stack_trace()
    {
        $bt  = debug_backtrace();
        $ret = '';
        foreach ($bt as $stack) {
            if (array_key_exists('file',$stack)) {
                $pathinfo = pathinfo($stack["file"]);
                $basename = $pathinfo['basename'];
            } else {
                $basename = '-';
            }
            if (array_key_exists('line',$stack)) {
                $line = $stack['line'];
            } else {
                $line = "-";
            }
            if (array_key_exists('class',$stack)) {
                $class = $stack['class'].'.';
            } else {
                $class = '';
            }

            $ret .= sprintf("  at %s%s(%s:%s)\n",
                            $class,
                            $stack['function'],
                            $basename,
                            $line);
        }
        return $ret;
    }
};

?>