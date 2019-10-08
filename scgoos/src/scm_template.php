<?php
require_once 'xtpl.php';
//require_once 'scm_encode.php';
require_once 'scmd_conf.php';

/*
*   Xtemplateの日本語拡張ライブラリ
*/
class SCM_Template extends XTemplate {

    /*
    *   スパイシーカリー標準のテンプレートジェネレータを作成する
    */
    function standard($file,$mainblock="main")
    {
        $path = SCMD_PACKAGE_HTM_DIRECTORY.'/'.$file;
        return new SCM_Template($path,$mainblock);
    }

    /*
    *   コンストラクタ
    *   @param  string  $encode 出力エンコード
    *   @param  array   $emoji  絵文字アンエスケープ配列
    *                   nullの場合はアンエスケープしない
    */
    function SCM_Template($file,$mainblock="main")
    {
        if (!file_exists($file)) {
            SCM_Log::error("template is not found ($file)");
            assert(false);
        }
        $this->XTemplate($file,$mainblock);
    }

    /*
    *   テンプレートの差込値を割り当てる
    *   日本語の文字を差し込む場合に利用する
    *   絵文字や半角文字を正しくデコードする
    *
    *   @param      string  $name   差込キー
    *   @param      mixed   $value  差込値
    *   @returtn    void
    */
    function assignX($name,$value)
    {
        assert(is_string($name));
        $value = (string)$value;
        $value = htmlspecialchars($value);
        $value = scm_unescape_im_sjis_html($value);
        parent::assign($name,$value);
    }

    /*
    *   テンプレートの差込値を割り当てる
    *   ASCII文字列のみ差し込む
    *
    *   @param      string  $name   差込キー
    *   @param      mixed   $value  差込値
    *   @returtn    void
    */
    function assign ($name,$value)
    {
        if (is_array($value)) {
            parent::assign($name,$value);
            return;
        }
        $value = (string)$value;
        $value = htmlspecialchars($value);
        parent::assign($name,$value);
    }
    
    function insert($name,$value)
    {
        $value = (string)$value;
        parent::assign($name,$value);
    }
};

?>