<?php

/**
 *  お知らせ情報を配信しない
 */
define('GOO_INFO_TYPE_NONE',  'none');
/**
 *  お知らせ情報を配信する
 */
define('GOO_INFO_TYPE_EXISTS','exists');
/**
 *  お知らせ情報をタイマー配信する
 */
define('GOO_INFO_TYPE_TIMER', 'timer:');
/**
 *  お知らせ配信時刻を指定しない場合
 */
define('GOO_INFO_NODATE','------------');

/**
 *
 *
 */
class GooInfo
{
    var $_type;
    var $_start;
    var $_end;
    var $_title;
    var $_url;

    /**
     *  お知らせ情報をアンシリアライズする
     */
    function load()
    {
        $info  = @file_get_contents(GOO_INFO_PATH);
        if (!$info) {
            scm_fatal_html(GOO_INFO_PATH);
        }
        $this->parse($info);
    }

    /**
     *  お知らせ情報をシリアライズして保存する
     */
    function save()
    {
        $dat = $this->pack();
        $fp = @fopen(GOO_INFO_PATH,'w');
        if (!$fp) {
            scm_fatal_html(GOO_INFO_PATH.":".$dat);
        }
        @fwrite($fp,$dat);
        @fclose();
    }

    /**
     *  リクエストから読み込む
     *  @return boolean 正しく読み込めなかったまたは不正な値がある場合は false、正しく読めた場合はfalse。
     */
    function loadFromRequest($r)
    {
        $this->_type = @$r['type'];
        $this->_title = false;
        $this->_url   = false;
        $this->_start = false;
        $this->_end   = false;
        switch($this->_type) {
        case GOO_INFO_TYPE_NONE:
            return true;
        case GOO_INFO_TYPE_EXISTS:
            break;
        case GOO_INFO_TYPE_TIMER:
            $this->_start = GooInfo::load_date($r,'start');
            $this->_end   = GooInfo::load_date($r,'end');
            if (is_a($this->_start,'INVALID_DATE') || is_a($this->_end,'INVALID_DATE')) {
                return false;
            }
            if ($this->_start == GOO_INFO_NODATE && 
                $this->_end   == GOO_INFO_NODATE) {
                return false;
            }
            if ($this->_start != GOO_INFO_NODATE && 
                $this->_end   != GOO_INFO_NODATE && 0 < strcmp($this->_start,$this->_end)) {
                return false;
            }
            break;
        default:
            return false;
        }

        $this->_title = @$r['title'];
        $this->_url   = @$r['url'];

        if (!GooInfo::isValidTitle($this->_title)) {
            echo $this->_title;
            return false;
        }
        if (!GooInfo::isValidURL($this->_url)) {
            echo $this->_url;
            return false;
        }
        return true;
    }

    /**
     *  テンプレートに値を差し込む
     *  @param $prefix string 値を差し込む時のPREFIX
     */
    function paste($prefix = '')
    {
        if ($prefix !== '') {
            $prefix .= ':';
        }
        set_param("${prefix}starty",false);
        set_param("${prefix}startm",false);
        set_param("${prefix}startd",false);
        set_param("${prefix}starth",false);
        set_param("${prefix}startn",false);
        set_param("${prefix}endy",  false);
        set_param("${prefix}endm",  false);
        set_param("${prefix}endd",  false);
        set_param("${prefix}endh",  false);
        set_param("${prefix}endn",  false);
        set_check("${prefix}checked_none",  false);
        set_check("${prefix}checked_exists",false);
        set_check("${prefix}checked_timer", false);

        switch($this->getType()) {
        case GOO_INFO_TYPE_NONE:
            set_check("${prefix}checked_none",   true);
            set_param("${prefix}title", false);
            set_param("${prefix}url",   false);
            break;
        case GOO_INFO_TYPE_EXISTS:
            set_check("${prefix}checked_exists", true);
            set_param("${prefix}title", $this->getTitle());
            set_param("${prefix}url",   $this->getURL());
            break;
        case GOO_INFO_TYPE_TIMER:
            set_check("${prefix}checked_timer",   true);
            list($start,$end)             = $this->getTimer();
            list($starty,$startm,$startd,$starth,$startn) = GooInfo::split_date($start);
            list($endy,  $endm,  $endd,  $endh,  $endn)   = GooInfo::split_date($end);
            set_param("${prefix}starty",$starty);
            set_param("${prefix}startm",$startm);
            set_param("${prefix}startd",$startd);
            set_param("${prefix}starth",$starth);
            set_param("${prefix}startn",$startn);
            set_param("${prefix}endy",  $endy);
            set_param("${prefix}endm",  $endm);
            set_param("${prefix}endd",  $endd);
            set_param("${prefix}endh",  $endh);
            set_param("${prefix}endn",  $endn);
            set_param("${prefix}title", $this->getTitle());
            set_param("${prefix}url",   $this->getURL());
            break;
        default:
            assert(false);
        }
        set_param("${prefix}type",  $this->getType());
    }
    

    /**
     *  YYYYMMDDHHNN形式の日付をあらわす文字列を分割して配列形式に変える。
     */
    function split_date($s)
    {
        if ($s == GOO_INFO_NODATE) {
            return array(false,false,false,false,false);
        }
        assert(12 <= strlen($s));

        $y = (int)substr($s, 0,4);
        $m = (int)substr($s, 4,2);
        $d = (int)substr($s, 6,2);
        $h = (int)substr($s, 8,2);
        $n = (int)substr($s,10,2);

        return array($y,$m,$d,$h,$n);
    }

    /**
     *  YYYYMMDDHHNN形式の日付をあらわす文字列に変換する
     */
    function unit_date($y,$m,$d,$h,$n)
    {
        if (!$y && !$m && !$d && !$h && !$n) {
            return GOO_INFO_NODATE;
        }
        $y = (int)$y;
        $m = (int)$m;
        $d = (int)$d;
        $h = (int)$h;
        $n = (int)$n;
        return str_pad($y,4,'0',STR_PAD_LEFT).
               str_pad($m,2,'0',STR_PAD_LEFT).
               str_pad($d,2,'0',STR_PAD_LEFT).
               str_pad($h,2,'0',STR_PAD_LEFT).
               str_pad($n,2,'0',STR_PAD_LEFT);
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
    function load_date($r,$prefix,$allowempty = true)
    {
        $namey = "${prefix}y";
        $namem = "${prefix}m";
        $named = "${prefix}d";
        $nameh = "${prefix}h";
        $namen = "${prefix}n";

        $ey = is_empty_request($r,$namey);
        $em = is_empty_request($r,$namem);
        $ed = is_empty_request($r,$named);
        $eh = is_empty_request($r,$nameh);
        $en = is_empty_request($r,$namen);

        if ($ey && $em && $ed && $eh && $en) {
            if ($allowempty) {
                return GOO_INFO_NODATE;
            } else {
                return new INVALID_DATE();
            }
        }
        if ($ey || $em || $ed || $eh || $en) {
            return new INVALID_DATE();
        }
        $y = (int)$r[$namey];
        $m = (int)$r[$namem];
        $d = (int)$r[$named];
        $h = (int)$r[$nameh];
        $n = (int)$r[$namen];

        if (!checkdate($m,$d,$y)) {
            return new INVALID_DATE();
        }
        return GooInfo::unit_date($y,$m,$d,$h,$n);
    }

    /**
     *  正しいお知らせURLかどうか
     *  @access public
     *  @static
     *  @param $title string URLとして正しいかどうかチェックしたい文字列
     *  @return お知らせURLとして正しい場合はtrue
     */
    function isValidURL($url)
    {
        if (!is_string($url)) {
            return false;
        }
        $length = strlen($url);
        if ($length == 0) {
            return true;
        }
        $mlength = mb_strlen($url);
        if ($length != $mlength) {
            return false;
        }
        if (scm_starts_with($url,"http://") ||
            scm_starts_with($url,"https://")) {
            return true;
        }
        return false;
    }

    /**
     *  正しいタイトルかどうか
     *  @access public
     *  @static
     *  @param $title string タイトルとして正しいかどうかチェックしたい文字列
     *  @return お知らせタイトルとして正しい場合はtrue
     */
    function isValidTitle($title)
    {
        if (!is_string($title)) {
            return false;
        }
        $length = strlen($title);
        if (!(1 <= $length && $length <= 64)) {
            return false;
        }
        return true;
    }


    /**
     *  お知らせの形式
     */
    function getType()
    {
        return $this->_type;
    }

    /**
     *  お知らせの期間
     *  @return array[2]
     */
     function getTimer()
     {
        return array($this->_start,$this->_end);
     }

    /**
     *  お知らせのタイトル
     *  @return string タイトル
     */
     function getTitle()
     {
        assert(is_string($this->_title) && $this->_title !== '');
        return $this->_title;
     }

    /**
     *  お知らせのURL
     *  @return string URL
     */
     function getURL()
     {
        assert(is_string($this->_url));
        return $this->_url;
     }


    /**
     *  お知らせを停止する
     */
    function setNone()
    {
        $this->_type = GOO_INFO_TYPE_NONE;
        $this->_start = false;
        $this->_end   = false;
        $this->_title = false;
        $this->_url   = false;
    }

    /**
     *  お知らせを設定する
     */
    function setExists($title,$url)
    {
        $this->_type  = GOO_INFO_TYPE_EXISTS;
        $this->_start = false;
        $this->_end   = false;
        $this->_title = $title;
        $this->_url   = $url;
    }

    /**
     *  タイマーお知らせを設定する
     */
    function setTimer($start,$end,$title,$url)
    {
        assert(!$start || strlen($start) == 12);
        assert(!$end   || strlen($end)   == 12);

        $this->_type  = GOO_INFO_TYPE_TIMER;
        $this->_start = $start;
        $this->_end   = $end;
        $this->_title = $title;
        $this->_url   = $url;
    }
    
    /**
     *  解析する
     *  @param info string お知らせファイルの内容
     *  @access private
     */
    function parse($info)
    {
        $lines = @split("\n",$info);
        if (@count($lines) != 3) {
            scm_fatal_html($lines);
        }
        $head = $lines[0];
        if ($head === GOO_INFO_TYPE_NONE) {
            $this->_type = GOO_INFO_TYPE_NONE;
            $this->_start= false;
            $this->_end  = false;
            return;
        } else if ($head === GOO_INFO_TYPE_EXISTS) {
            $this->_type = GOO_INFO_TYPE_EXISTS;
            $this->_start= false;
            $this->_end  = false;
        } else if (substr($head,0,strlen(GOO_INFO_TYPE_TIMER)) === GOO_INFO_TYPE_TIMER) {
            $this->_type  = GOO_INFO_TYPE_TIMER;
            $head = substr($head,strlen(GOO_INFO_TYPE_TIMER));
            $this->_start = substr($head, 0,12);
            $this->_end   = substr($head,12,12);
        } else {
            scm_fatal_html($head);
        }
        list($this->_title,$this->_url) = @split("\t",$lines[1]);
    }

    /**
     * お知らせ情報をシリアライズ
     *  @return 情報をシリアライズしたお知らせファイルの内容
     *  @access private
     */
    function pack()
    {
        $cont = '';
        $head = $this->_type;
        if ($head === GOO_INFO_TYPE_NONE) {
            $cont = GOO_INFO_TYPE_NONE . "\n\n";
        } else if ($head === GOO_INFO_TYPE_EXISTS) {
            $cont = GOO_INFO_TYPE_EXISTS . "\n".
                    $this->_title        . "\t".
                    $this->_url          . "\n";
        } else if (substr($head,0,strlen(GOO_INFO_TYPE_TIMER)) === GOO_INFO_TYPE_TIMER) {
            $cont = GOO_INFO_TYPE_TIMER  .  
                    $this->_start        . 
                    $this->_end          . "\n".
                    $this->_title        . "\t".
                    $this->_url          . "\n";
        } else {
            scm_fatal_html($head);
        }
        return $cont;
    }

}

?>