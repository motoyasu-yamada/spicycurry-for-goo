<?php

/*
*   端末からのリソースファイル要求に応答して
*   リソースファイルを返す。
*
*   @param string $user_id ユーザID
*   @param string $fname   リソースファイル名
*/
function scm_get_res($user_id,$fname)
{
    SCM_Log::trace_var('fname',$fname);
    global $SCM_REQUEST;
    global $SCM_RES_MAP;

    //  機種情報を求める
    assert(array_key_exists('dev_id',$SCM_REQUEST));
    $dev_id      = scm_get_array($SCM_REQUEST,'dev_id',     '');
    $platform_id = scm_get_array($SCM_REQUEST,'platform_id','');

    //  リソースファイルのパスを求めて検証する
    //  リソースファイルの検索順位
    //      <ファイル名>.<機種ID>
    //      <ファイル名>.<同系機機種ID>
    //      <ファイル名>.<プラットフォームID>
    //      <ファイル名>
    $path = SCMD_PACKAGE_RES_DIRECTORY.'/'.$fname.'.'.$dev_id;
    SCM_Log::trace_var('path',$path);
    if (file_exists($path)) {
        scm_get_res_output($path);
        return;
    }

    if (array_key_exists($dev_id,$SCM_RES_MAP)) {
        $dev_id2 = $SCM_RES_MAP[$dev_id];
        $path = SCMD_PACKAGE_RES_DIRECTORY.'/'.$fname.'.'.$dev_id2;
        SCM_Log::trace_var('path',$path);
        if (file_exists($path)) {
            scm_get_res_output($path);
            return;
        }
    }

    $path = SCMD_PACKAGE_RES_DIRECTORY.'/'.$fname.'.'.$platform_id;
    SCM_Log::trace_var('path',$path);
    if (file_exists($path)) {
        scm_get_res_output($path);
        return;
    }
    $path = SCMD_PACKAGE_RES_DIRECTORY.'/'.$fname;
    SCM_Log::trace_var('path',$path);
    if (file_exists($path)) {
        scm_get_res_output($path);
        return;
    }
    scm_exit_404("scm_get_res: search error $fname.");
}

/*
*   リソースファイルを携帯へ応答する
*
*   @param  $path ファイルパス
*   @return void
*/
function scm_get_res_output($path)
{
    if (!@file_exists($path)) {
        scm_exit_404("scm_get_res_output: file not exits $path.");
        return;
    }

    //  ファイル長を求める
    $size = @filesize($path);
    if (!$size) {
        scm_exit_404('scm_get_res_output: file is empty.');
        return;
    }
    if (0xFFFF < $size) {
        scm_exit_404('scm_get_res_output: files size over.');
        return;
    }

    //  リソースファイルを出力する
    @header("Content-Type: Application/octetstream ");
    @header("Content-Length: $size");
    @readfile($path);
}

?>