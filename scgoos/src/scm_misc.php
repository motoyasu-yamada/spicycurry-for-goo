<?php
require_once 'scm_log.php';

/*
*   致命的エラーが発生した場合は
*   ページ上に致命的エラーが発生した旨を表示して終了する
*/
function scm_fatal_api($msg = '') {
    SCM_Log::error($msg);
    $out = new SCM_OutputStream();
    $out->writeUI8(ord('0'));
    $out->writeString16X('障害');
    echo $out->toString();
    @exit;
}

/**
*   error_reporting = E_ALLの場合、配列に該当するインデックスがないのに
*   $array[$key]するとwarningがでるので、インデックスがない場合は、
*   $defaultを返し、インデックスがある場合は、配列のエントリーを返す。
*
*   @param $key     配列から取得するエントリーのインデックス
*   @param $default もし並列に$keyというインデックスのエントリーが
*                   存在しない場合に返すデフォルト値
*/
function scm_get_array($array,$key,$default = "")
{
    if (!is_array($array)) {
        return $default;
    }
    if (array_key_exists($key,$array)) {
        return $array[$key];
    }
    return $default;
}

/*
*   バイナリデータをダンプする
*   @param string $binary バイナリデータ
*/
function scm_dump($binary) {
    $dump = '';
    $pos  = 0;
    $len  = strlen($binary);
    for(;;) {
        $start = $pos;
        if ($len <= $pos) {
            break;
        }
        $hex = '';
        $asc = '';
        for($n = 0;$n < 16; $n++) {
            if ($len <= $pos) {
                break;
            }
            $c = $binary[$pos++];
            $a = ord($c);
            $hex .= str_pad(dechex($a),2,'0',STR_PAD_LEFT);
            $hex .= ' ';
            if ($a < 0x20 || 0x7E < $a) {
                $c = '.';
            }
            $asc .= $c;
        }
        $dump .= str_pad(dechex($start),4,'0',STR_PAD_LEFT).' '.
                 str_pad($hex,50,' ').' : '.$asc."\n";
    }
    return $dump;
}

/**
*   JavaのString.startsWithと同様の関数
*
*   @param string $string  比較対象文字列
*   @param string $pattern この文字列で$stringが始まっているかどうか
*                          ''を渡してはいけない
*   @return boolean true  - $stringは$patternではじめる
*                   false - はじまらない
*/
function scm_starts_with($string,$pattern) {
    assert(is_string($string));
    assert(is_string($pattern));
    $length = strlen($pattern);
    assert(1 <= $length);
    return strcmp(substr($string,0,$length),$pattern) == 0;
}


/**
*   iモードのUTN属性を取得する
*
*   1) DoCoMo/1.0/X503i/c10/ser*********** 
*   -> ser***********
*   2) DoCoMo/2.0 YYYY(c10;serXXXXXXXXXXXXXXX; iccxxxxxxxxxxxxxxxxxxxx) 
*   -> serXXXXXXXXXXXXXXX; iccxxxxxxxxxxxxxxxxxxxx
*
*   @param &string $utn 取得したUTN。取得できなかった場合は''。
*   @return boolean true  - UTNを取得できた
*                   false - UTNを取得できなかった
*/
function scm_get_imode_utn(&$utn)
{
    $ua = scm_get_array($_SERVER,'HTTP_USER_AGENT','');
    if (!$ua || strlen($ua) == 0) {
        return false;
    }

    $utn = '';

    //  50xシリーズ
    if (scm_starts_with($ua,'DoCoMo/1.0')) {
        if (preg_match("/(ser[A-Za-z0-9]{11})$/i",$ua,$matches)) {
            $utn = $matches[1];
            assert(is_string($utn));
            assert(strlen($utn) == 14);
            return true;
        }
    }

    //  FOMAシリーズ
    if (scm_starts_with($ua,'DoCoMo/2.0')) {
        if (preg_match("/\(c\d+\;(ser[A-Za-z0-9]{15}\;icc[A-Za-z0-9]{20})\)$/i",$ua,$matches)) {
            $utn = $matches[1];
            assert(is_string($utn));
            assert(strlen($utn) == 42);
            return true;
        }
        if (preg_match("/\(c\d+\;T[BJCD]\;(ser[A-Za-z0-9]{15}\;icc[A-Za-z0-9]{20})\)$/i",$ua,$matches)) {
            $utn = $matches[1];
            assert(is_string($utn));
            assert(strlen($utn) == 42);
            return true;
        }
        if (preg_match("/\(c\d+\;T[BJCD]\;W\d+H\d+\;(ser[A-Za-z0-9]{15}\;icc[A-Za-z0-9]{20})\)$/i",$ua,$matches)) {
            $utn = $matches[1];
            assert(is_string($utn));
            assert(strlen($utn) == 42);
            return true;
        }
    }
    SCM_Log::error($ua);
    return false;
}

/*
*   HTTPクライアントに404応答を返す
*   @param string $message trace.log/error.logに記録するログ内容
*/
function scm_exit_404($message) 
{
    SCM_Log::error($message);
    @header("HTTP/1.0 404 Not Found");
    exit;
}

/*
*   mb_ereg_replaceの簡易代替関数
*   マルチバイト文字列 stringにおいて、
*   大文字小文字を区別して、パターンpatternに
*   マッチする文字列を replacementに置換し、
*   置換後の文字列を返します。
*
*   @param  string  $pattern
*   @param  string  $replacement
*   @param  string  $string
*   @return string  置換後の文字列
*/
function scm_replace($pattern,$replacement,$string,$encoding = null)
{
    if (is_null($encoding)) {
        $encoding = @mb_internal_encoding();
    }
    //assert($encoding == 'SJIS' || $encoding == 'EUC-JP');
    assert(is_string($pattern));
    assert(is_string($replacement));

    $string   = (string)$string;
    $start    = 0;
    $replaced = '';
    $mblen    = @mb_strlen($pattern);
    $len      = @mb_strlen($string);
    while($start < $len) {
        $find = @mb_strpos($string,$pattern,$start,$encoding);
        //  only PHP 4 >= 4.0.6
        if ($find === false) {
            $sub       = @mb_substr($string,$start);
            $replaced .= $sub;
            break;
        }
        $sub       = @mb_substr($string,$start,$find - $start);
        $replaced .= $sub;
        $replaced .= $replacement;
        $start     = $find + $mblen;
    }

    assert(is_string($replaced));
    return $replaced;
}


/*
*   16進数文字列をバイナリ文字列に変換する
*   @param  string  16進数文字列
*   @return バイナリ文字列
*/
function scm_hexbin($hex)
{
    assert(strlen($hex) % 2 == 0);

    $length = strlen($hex) / 2;
    $binary = '';
    for ($n = 0;$n < $length;$n++) {
        $hex1 = substr($hex,$n*2,2);
        $dec  = hexdec($hex1);
        $binary .= chr($dec);
    }
    return $binary;
}

/*
*   バイナリ文字列を16進数ダンプに変換する
*   @param  string  $bin    バイナリ文字列
*   @return string 16進数ダンプ
*/
function scm_binhex($bin)
{
    assert(is_string($bin));

    $length = strlen($bin);
    $hex = '';
    for ($n = 0;$n < $length;$n++) {
        $hex .= scm_dechex(ord($bin[$n]),2);
    }

    assert(strlen($hex) == $length * 2);
    return $hex;
}


/*
*   整数値を指定した桁数に0で詰めた16進数表記文字列に変換します。
*   @param  int    $dec    16進文字列にする数値
*   @param  int    $col    桁数 1桁～8桁
*   @return string 16進文字列
*/
function scm_dechex($dec,$col)
{
    assert(is_int($dec));
    assert(is_int($col));
    assert(1 <= $col && $col <= 8);

    $hex = str_pad(dechex($dec),$col,'0',STR_PAD_LEFT);

    assert(is_string($hex));
    assert(strlen($hex) == $col);
    return $hex;
}

/*
*   バイナリ文字列同士の各バイトのXORビット演算
*   @param  string $bin1 左辺
*   @param  string $bin2 右辺
*   @return xor結果
*/
function scm_binxor($bin1,$bin2)
{
    $len1 = strlen($bin1);
    $len2 = strlen($bin2);
    $len  = max($len1,$len2);
    $xored = '';
    for ($n = 0;$n < $len;$n++) {
        $byte1 = substr($bin1,$n % $len1,1);
        $byte2 = substr($bin2,$n % $len2,1);
        $xored .= chr(ord($byte1) ^ ord($byte2));
    }

    assert(strlen($xored) == $len);
    return $xored;
}

/*
*   サーバアドレス
*   @return string サーバアドレス文字列
*                  取得できなかった場合 - 
*/
function scm_get_server_addr()
{
    if (isset($_SERVER['SERVER_ADDR'])) {
        $addr = $_SERVER['SERVER_ADDR'];
    } else {
        $addr = '-';
    }
    $addr .= '=';
    if (isset($_SERVER['REMOTE_ADDR'])) {
        $addr .= $_SERVER['REMOTE_ADDR'];
    } else {
        $addr .= '-';
    }
    return $addr;
}

/**
 *  &#NNNNN;形式の絵文字を変換してSJISバイナリ絵文字にする
 *  @param $src string Windows-31J形式の文字列
 *  @return Windows-31J形式の文字列
 */
function unescape_entity($src)
{
    return preg_replace_callback("/&#(\d{5});/","unescape_entity_callback",$src);
}

/**
 *  unescape_entity
 */
function unescape_entity_callback($matches)
{
    $code = (int)$matches[1];
    return chr(($code >> 8) & 0xFF).chr($code & 0XFF);
}

?>