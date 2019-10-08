<?php
require_once 'scmd_conf.php';
require_once 'scm_log.php';

/**
 *  プロキシー処理メイン
 */
function admin_proxy()
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

    $request_uri = $_SERVER['REQUEST_URI'];
    if (preg_match('/([^\/]+\.php)(\?)?$/',$request_uri,$matches)) {
        $php = $matches[1];
        require_once $php;
        action();
        exit;
    }
    if (preg_match('/([^\/]+)\.htm$/',$request_uri,$matches)) {
        $htm = $matches[1];
        $htm = "t_${htm}.php";
        include_once $htm;
        exit;
    }
    scm_fatal_html($request_uri);
}

/*
*   致命的エラーが発生した場合は
*   ページ上に致命的エラーが発生した旨を表示して終了する
*/
function scm_fatal_html($msg = '') {
    global $php_errormsg;
    echo "<pre>";
    echo "※致命的障害の為処理を停止します※($msg)\n";
    echo "$php_errormsg\n";
    echo "</pre>";
    SCM_Log::error($msg);
    assert(false);
    @exit;
}


/**
 *  カスタム assertコールバックハンドラ
 */
function scm_assert_handler($file, $line, $code)
{
    $msg   = "<pre>";
    $msg  .= "*** ASSERT *** '$file'($line) : '$code'\n";
    $msg  .= SCM_Log::get_stack_trace();
    $msg  .= "</pre>";

    echo $msg;
    //SCM_Log::error($msg);
    @exit;
}


/**
 *  テンプレートへの日付型の値を書式化して差し込む
 *  @param $name   string 差し込む変数名
 *  @param $format string 日付の書式化フォーマット
 */
function param_date($name,$format)
{
    $val = get_param($name);
    $t   = mktime(0,0,0,(int)substr($val,4,2),(int)substr($val,6,2),(int)substr($val,0,4));

    $val = date($format,$t);
    $val = htmlspecialchars($val);
    print_param($val);
}


define('ESCAPE_HTML',0);
define('NOESCAPE',   1);
define('ESCAPE_URL', 2);

/**
 *  テンプレートに埋め込む値を取得する
 */
function param($name,$esc = ESCAPE_HTML)
{
    $val = get_param($name);
    print_param($val);
}

/**
 *  変数の値を出力する
 *  @access private
 *  @param $val 変数の値
 */
function print_param($val,$esc = ESCAPE_HTML)
{
    switch($esc){
    case ESCAPE_HTML:
        $val = htmlspecialchars($val);
        break;
    case ESCAPE_URL:
        $val = urlencode($val);
        break;
    case NOESCAPE:
        break;
    default:
        assert(false);
    }
    echo mb_convert_encoding($val,'Windows-31J','EUCJP-WIN');
}

/**
 *  変数の値を取得する
 *  @access private
 *  @param $name string 変数の名前
 *  @return 変数の値
 */
function get_param($name)
{
    global $var;
    if (!isset($var[$name])) {
        echo "<pre>";
        var_dump($GLOBALS);
        echo "****** $name ****";
        echo "</pre>";
        scm_fatal_html($name);
    }
    $val = $var[$name];
    return $val;
}

/**
 *  テンプレートに埋め込む値を設定する
 */
function set_param($name,$value)
{
    global $var;
    $var[$name] = $value;
}

/**
 *  テンプレートにCHECKED値を設定する
 *  @param value boolean trueの場合はCHCECKEDを差し込む。falseの場合は空を差し込む
 */
function set_check($name,$value)
{
    set_param($name,$value ? "CHECKED" : "");
}

/**
 *  ループ差込
 *  @param $itr array 二次元配列
 *  @return ループを続ける場合はtrue、続けない場合はfalse
 */
function loop(&$itr)
{
    assert(isset($itr));
    assert(is_array($itr));
    $e = current($itr);
    if (!$e) {
        return false;
    }
    next($itr);
    assert(is_array($e));
    reset($e);
    foreach($e as $k => $v) {
        if (is_array($v)) {
            reset($v);
            global $$k;
            $$k = $v;
        } else {
            set_param($k,$v);
        }
    }
    return true;
}

/**
 *  YYYYMMDD形式の日付をあらわす文字列を分割して配列形式に変える。
 */
function split_date($s)
{
    if (!$s) {
        return array(false,false,false);
    }
    $y = (int)substr($s,0,4);
    $m = (int)substr($s,4,2);
    $d = (int)substr($s,6,2);
    return array($y,$m,$d);
}

/**
 *  YYYYMMDD形式の日付をあらわす文字列に変換する
 */
function unit_date($y,$m,$d)
{
    if (!$y && !$m && !$d) {
        return false;
    }
    $y = (int)$y;
    $m = (int)$m;
    $d = (int)$d;
    return str_pad($y,4,'0',STR_PAD_LEFT).
           str_pad($m,2,'0',STR_PAD_LEFT).
           str_pad($d,2,'0',STR_PAD_LEFT);
}

/**
 *  年/月/日が分離したリクエストから、日付をYYYYMMDD形式で取得する。
 *  @param $r array 主に$_REQUEST
 *  @param $namey string 年リクエスト変数名
 *  @param $namem string 月リクエスト変数名
 *  @param $named string 日リクエスト変数名
 *  @param $allowempty boolean 日付を指定しないことを許すかどうか
 *  @return 日付が指定されていない場合はfalse、指定されている場合はYYYYMMDD形式の日付、
 *          不正な形式の場合はINVALID_DATE型のエラーオブジェクト。
 */
function load_date($r,$namey,$namem,$named,$allowempty = true)
{
    $ey = is_empty_request($r,$namey);
    $em = is_empty_request($r,$namem);
    $ed = is_empty_request($r,$named);
    
    if ($ey && $em && $ed) {
        if ($allowempty) {
            return false;
        } else {
            return new INVALID_DATE();
        }
    }
    if ($ey || $em || $ed) {
        return new INVALID_DATE();
    }
    $y = (int)$r[$namey];
    $m = (int)$r[$namem];
    $d = (int)$r[$named];
    if (!checkdate($m,$d,$y)) {
        return new INVALID_DATE();
    }
    return unit_date($y,$m,$d);
}

/**
 *  不正な日付をあらわす型
 */
class INVALID_DATE {};

/**
 *  指定した名前のフォーム変数/QueryString変数がリクエストされたかどうか？
 *  @param $r array リクエスト
 *  @param $name string 変数名
 *  @return true リクエストされていない
 */
function is_empty_request($r,$name)
{
    assert(is_array($r));
    assert(is_string($name));

    if (!array_key_exists($name,$r)) {
        return true;
    }
    $value = trim($r[$name]);
    if ($value === '') {
        return true;
    }
    return false;
}

?>