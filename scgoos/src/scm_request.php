<?php
require_once 'scmd_conf.php';
require_once 'scm_imode.php';

/*
*   リクエスト情報
*/
$SCM_REQUEST = array(
    #   内部ユーザID
    'user_id'         => 'unknown',
    #   プラットフォームID (504i/505i)
    'platform_id'     => 'unknown',
    #   機種識別子
    'dev_id'          => 'unknown',
    #   アップストリーム
    'upstream_size'   => 0,
    #   ダウンストリーム
    'downstream_size' => 0
);

/*
*   リクエスト情報を解析する
*/
function scm_parse_request()
{
    global $SCM_REQUEST;
    global $SCM_MAX_UPSTREAM_SIZE;
    global $SCM_MAX_DOWNSTREAM_SIZE;

    $ua = scm_get_array($_SERVER,'HTTP_USER_AGENT');
    $platform = SCM_PLATFORM_ERROR;

    SCM_Log::trace("UserAgent:".$ua);

    // (504i)  DoCoMo/1.0/F504i/c10/TB
    if (ereg('^DoCoMo/1\.0/([A-Z]+504i)/c[0-9]+/.+',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_504I;

    // (504is) DoCoMo/1.0/F504is/c10/TB
    } else if (ereg('^DoCoMo/1\.0/([A-Z]+504iS)/c[0-9]+/.+',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_504I;

    // (505i)  DoCoMo/1.0/D505i/c20/TB/W20H10
    } else if (ereg('^DoCoMo/1.0/([A-Z]+505i)/c[0-9]+/.+',$ua,$match) ||
               ereg('^DoCoMo/1.0/(SH505i)2/c[0-9]+/.+',$ua,$match)
               ) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;

    // (505is) DoCoMo/1.0/D505iS/c20/TB/W20H10
    } else if (ereg('^DoCoMo/1.0/([A-Z]+505iS)/c[0-9]+/.+',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;

    // (505iS2) DoCoMo/1.0/D505iS/c20/TB/W20H10
    } else if (ereg('^DoCoMo/1.0/(SH505iS2)/c[0-9]+/.+',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;

    // (505iGPS) DoCoMo/1.0/F505iGPS/c20/TB/W20H10
    } else if (ereg('^DoCoMo/1.0/(F505iGPS)/c[0-9]+/.+',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;

    // (506i)  DoCoMo/1.0/D506i/c20/TB/W20H10
    } else if (ereg('^DoCoMo/1.0/([A-Z]+506i[A-Za-z0-9]*)/c[0-9]+/.+',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;

    //   x90X
    } else if (ereg('^DoCoMo/2.0 ([A-Z]+90[0-9]i[A-Za-z0-9]*)\(c[0-9]+;T[BCDJ].*\).*$',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_900I;

    //   x700X
    } else if (ereg('^DoCoMo/2.0 ([A-Z]+70[0-9]i[A-Za-z0-9]*)\(c[0-9]+;T[BCDJ].*\).*$',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;


    //   x85X
    } else if (ereg('^DoCoMo/2.0 ([A-Z]+85[0-9]i[A-Za-z0-9]*)\(c[0-9]+;T[BCDJ].*\).*$',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_505I;


    //  FOMA    DoCoMo/2.0 F2051(c100;TB)
    } else if (ereg('^DoCoMo/2.0 ([A-Z]+[0-9]+[A-Za-z]*)\(c[0-9]+;T[BCDJ].*\).*$',$ua,$match)) {
        $device   = strtolower($match[1]);
        $platform = SCM_PLATFORM_FOMA2;

    } else {
        SCM_Log::error($ua);
        return;
    }

    SCM_Log::trace("Platform:".$platform);

    $SCM_REQUEST['dev_id']          = $device;
    $SCM_REQUEST['platform_id']     = $platform;
    $SCM_REQUEST['upstream_size']   = $SCM_MAX_UPSTREAM_SIZE  [$platform] - SCM_API_HEAD_SIZE;
    $SCM_REQUEST['downstream_size'] = $SCM_MAX_DOWNSTREAM_SIZE[$platform] - SCM_API_HEAD_SIZE;
}

?>