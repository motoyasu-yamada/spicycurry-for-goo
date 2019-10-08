<?php
require_once 'scm_imode.php';

/**
 *   ユーザからJAMファイル要求
 *   @param  int     $serid  ;が-にエンコードされている端末ID(SerID)
 */
function scm_get_jam($serid)
{
    assert(isset($serid));
    $serid = str_replace("-",";",$serid);

    //  機種に応じたアプリファイル名を求める
    $fname = scm_get_app_fname();

    //  JAMファイルを読み込んで置換
    $path = SCMD_PACKAGE_APP_DIRECTORY.'/'.$fname.'.jam';
    SCM_Log::trace($path);
    $file = @fopen($path,'r');
    if (!$file) {
        scm_exit_404('no jam file');
    }
    $jam = @fread($file,filesize($path));
    if (!$jam) {
        scm_exit_404('empty jam file');
    }
    @fclose($fname);
    $jam  = @mb_convert_encoding($jam,mb_internal_encoding(),'SJIS');

    //  JAMファイルにユーザ情報を埋め込む
    $jam  = ereg_replace('\${UserID}',$serid,$jam);
    $jam  = ereg_replace("(AppName *= *)","\\1".SCMD_JAD_APPNAME_PREFIX,$jam);

    //  JAMファイルを出力する
    $jam  = @mb_convert_encoding($jam,'SJIS',mb_internal_encoding());
    $size = strlen($jam);
    // header("Content-Disposition: filename=$fname");
    header("Content-Length: $size");
    
    echo $jam;

    SCM_Log::trace("*=>".$jam);
}

?>
