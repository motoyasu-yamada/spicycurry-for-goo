<?php
//  $Id: scm_stream.php,v 1.1.1.1 2005/03/04 10:31:18 m_yamada Exp $
//  ストリームライブラリ
require_once 'scm_error.php';
// require_once 'scm_encode.php';
// require_once 'scm_emoji.php';

/*
*   バイナリストリームに値を出力する
*/
class SCM_OutputStream {
    var $_stream;

    /*
    *   コンストラクタ
    */
    function SCM_OutputStream() {
        $this->_stream = '';
    }

    /*
    *   携帯アプリAPI通信が成功したことを返す
    *   @return void
    */
    function writeResultOK() {
        $this->writeUI8(ord('1'));
        SCM_Log::trace("API transfer is OK");
    }

    /*
    *   ユーザレベルのエラーが発生した
    *   @param  OutputStream $res
    *   @param  string       $user_msg ユーザ(携帯アプリに通知するメッセージ)
    *   @param  string       $err_msg  ログに記録するメッセージ
    *                                  ASCIIコードであること(日本語禁止)
    *   @return void
    */
    function writeResultNO()
    {
        $user_msg = "エラー";
        $err_msg  = "errorhappend";

        assert(is_string($user_msg));
        assert(is_string($err_msg));
        assert($this->getLength() == 0);

        $this->writeUI8(ord('0'));
        $this->writeString16X($user_msg);

        SCM_Log::trace($err_msg);
    }

    /*
    *   符号なし8ビット整数値を書き込む
    *   @param  v   書き込む整数値 0x00-0xFF
    */
    function writeUI8($v) {
        assert(is_int($v));
        assert(0x00 <= $v && $v <= 0xFF);
        
        $this->_stream .= chr($v);
    }

    /*
    *   符号なし16ビット整数値を書き込む
    *   @param  v   書き込む整数値 0x0000-0xFFFF
    */
    function writeUI16($v) {
        assert(is_int($v));
        assert(0x0000 <= $v && $v <= 0xFFFF);
        $this->writeUI8( $v       & 0xFF);
        $this->writeUI8(($v >> 8) & 0xFF);
    }

    /**
     *   内部エンコードされた文字列を書き込む
     *   @param  s   内部エンコード(絵文字エスケープ)文字列
     */
    function writeString16X($s)
    {
        $s = @mb_convert_encoding($s,'Windows-31J',mb_internal_encoding());
        $s = unescape_entity($s);
        $this->writeString16($s);
    }

    /*
    *   文字列を書き込む
    *   先頭16ビットに文字列長ヘッダ
    *   続いてSJIS形式の文字列を書き込む
    *   @param  s   SJIS形式の文字列
    */
    function writeString16($s)
    {
//      assert(is_string($s));
//      半角カナがあるとEUC_JPと判定される バカ？
//      assert(mb_detect_encoding($s) != mb_internal_encoding());
//      絵文字があるとJISと誤判定される
//      assert(mb_detect_encoding($s) == 'SJIS' ||
//             mb_detect_encoding($s) == 'ASCII');
        assert(strlen($s) <= 0xFFFF);
        $this->writeUI16(strlen($s));
        $this->_stream .= $s;
    }

    /*
    *   文字列を書き込むとどれぐらいの長さになるか測定する
    *   @param  s   SJIS形式の文字列
    */
    function lengthString16($s) {
        assert(is_string($s));
//      assert(mb_detect_encoding($s) == 'SJIS' ||
//             mb_detect_encoding($s) == 'ASCII');
        assert(strlen($s) <= 0xFFFF);
        return strlen($s) + 2;
    }

    /*
    *   文字列を書き込むとどれぐらいの長さになるか測定する
    *   @param  s   SJIS形式の文字列
    */
#    function lengthString16X($s) {
#        assert(is_string($s));
#        assert(strlen($s) <= 0xFFFF);
#        return scm_escape_len($s) + 2;
#    }

    /*
    *   バイナリデータを書き出す
    *   @param string $binary バイナリデータ
    */
    function writeBinary($binary)
    {
        assert(is_string($binary));
        if (strlen($binary) == 0) {
            return;
        }
        assert(0 < strlen($binary));
        $this->_stream .= $binary;
    }

    /*
    *   既に書き出されたサイズを取得する
    */
    function getLength() {
        assert(is_string($this->_stream));
        $r = strlen($this->_stream);
        assert(0x00 <= $r && $r <= 0xffff);
        return $r;
    }

    /*
    *   書き込まれたバイナリストリームを取り出す
    */
    function toString() {
        $r = $this->_stream;
        assert(is_string($r));
        return $r;
    }
};

/**
*   バイナリストリームから値を読み込む
*/
class SCM_InputStream {
    //  読込ストリーム
    var $_stream;
    // 現在の読込位置
    var $_pos;

    /*
    *   コンストラクタ
    *   @param $stream 入力バイナリストリーム
    */
    function SCM_InputStream($stream) {
        if (is_null($stream)) {
            $stream = '';
        }
        assert(is_string($stream));
        $this->_stream = $stream;
        $this->_pos    = 0;
    }

    /*
    *   符号なし8ビット整数値を読込む
    *   @return 整数値  0x00-0xff
    */
    function readUI8()
    {
        if (!($this->_pos < strlen($this->_stream))) {
            scm_fatal_api(scm_binhex($this->_stream));
        }

        $r = ord($this->_stream[$this->_pos++]);

        assert(is_int($r));
        assert(0x00 <= $r && $r <= 0xff);
        return $r;
    }

    /*
    *   符号なし16ビット整数値を読込む
    *   @return 整数値  0x0000-0xffff
    */
    function readUI16() {
        if(!($this->_pos + 1 < strlen($this->_stream))) {
            scm_fatal_api(scm_binhex($this->_stream));
        }
        $r  = $this->readUI8();
        $r += $this->readUI8() << 8;
        assert(is_int($r));
        assert(0x0000 <= $r && $r <= 0xffff);
        return $r;
    }

    /*
    *   SJIS文字列を読込む
    *   
    *   @return 読み込まれたSJIS形式の文字列
    */
    function readString16() {
        $len = $this->readUI16();
        if ($len == 0) {
            $r = '';
        } else {
            if (!(($this->_pos + $len) <= strlen($this->_stream))) {
                scm_fatal_api(scm_binhex($this->_stream));
            }
            $r = substr($this->_stream,$this->_pos,$len);
            $this->_pos += $len;
        }
        assert(is_string($r));
//      半角カナがあるとEUC_JPと判定される バカ？
//      assert(mb_detect_encoding($r) != mb_internal_encoding());
//      絵文字があるとJISと誤判定される
//      assert(mb_detect_encoding($r) == 'SJIS' ||
//             mb_detect_encoding($r) == 'ASCII');
        assert(strlen($r) <= 0xffff);
        return $r;
    }

    /*
    *   絵文字,半角カナを含むSJIS文字列を読込み
    *   内部形式で返す。
    *
    *   @return string 絵文字、半角カナをエスケープした内部文字列
    */
    function readString16X()
    {
        $string = $this->readString16();
        // $string = scm_escape_sjis($string);
        return $string;
    }

    /*
    *   ストリームの長さを求める
    */
    function getLength() 
    {
        return strlen($this->_stream);
    }

    /*
    *   既にストリーム内の全てのデータを読み込んだかどうか
    *   @return boolean true  - 終端
    *                   false - 続きのデータはある
    */
    function isEof() {
        return !($this->_pos < $this->getLength());
    }
};


?>