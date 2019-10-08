<?php

/*
*   機種プラットフォーム情報
*/
define('SCM_PLATFORM_ERROR','');
define('SCM_PLATFORM_504I', '504i');
define('SCM_PLATFORM_505I', '505i');
define('SCM_PLATFORM_FOMA2','foma2');
define('SCM_PLATFORM_900I', '900i');

/*
*   機種別アプリ名
*/
$SCM_DEV_APPLI_FNAME      = array(
    SCM_PLATFORM_504I     => '504i',
    SCM_PLATFORM_505I     => '505i',
    SCM_PLATFORM_FOMA2    => 'foma2',
    SCM_PLATFORM_900I     => '900i',
);

/*
*   アップストリームサイズ
*/
$SCM_MAX_UPSTREAM_SIZE = array(
    SCM_PLATFORM_504I  => 5120,
    SCM_PLATFORM_505I  => 10240,
    SCM_PLATFORM_FOMA2 => 5120,
    SCM_PLATFORM_900I  => 10240,
    SCM_PLATFORM_ERROR => -1
);

/*
*   ダウンストリームサイズ
*/
$SCM_MAX_DOWNSTREAM_SIZE = array(
    SCM_PLATFORM_504I  => 10240,
    SCM_PLATFORM_505I  => 20480,
    SCM_PLATFORM_FOMA2 => 10240,
    SCM_PLATFORM_900I  => 20480,
    SCM_PLATFORM_ERROR => -1
);

/**
 *   リソースマッピング情報
 *   
 *   機種名と同じファイル名のリソースが存在しない場合、
 *   ほぼ同型機種のリソースを利用する.
 */
$SCM_RES_MAP = array(
);

/*
*   アクセスしてくる端末の種別に応じたアプリファイル名を求める
*   不正な端末の場合は強制終了する。
*   @return  false  
*            string ファイル名
*/
function scm_get_app_fname()
{
    global $SCM_REQUEST;
    global $SCM_DEV_APPLI_FNAME;

    $dev_id      = scm_get_array($SCM_REQUEST,'dev_id',     '');
    $platform_id = scm_get_array($SCM_REQUEST,'platform_id','');
    SCM_Log::trace("dev_id = " . $dev_id);
    SCM_Log::trace("platform_id = " . $platform_id);
    assert(is_string($platform_id) && $platform_id != '');

    if (file_exists(SCMD_PACKAGE_APP_DIRECTORY.'/'.$dev_id.'.jam')) {
        return $dev_id;
    }

    $fname = scm_get_array($SCM_DEV_APPLI_FNAME,$platform_id,'');
    if ($fname == '') {
        scm_exit_404("no app fname for $platform_id");
    }
    return $fname;
}

?>