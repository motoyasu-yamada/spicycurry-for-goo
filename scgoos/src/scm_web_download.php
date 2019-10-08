<?php
require_once 'scm_misc.php';
require_once 'scm_template.php';
require_once 'scm_check_device.php';

/**
 *   ダウンロード用ページ
 */
function scm_web_download()
{
    if (scm_web_c_download($params)) {
        scm_web_v_download($params);
    } else {
        scm_web_v_download_e($params);
    }
}

/**
 *   ダウンロードページコントローラ(mvC)
 *
 *   @param &array $params テンプレートに埋込値の格納配列
 */
function scm_web_c_download(&$params)
{
    // 対応機種チェック
    $checkdevice = new SCM_Check_Device();
    if (!$checkdevice->canDownload()) {
        $params['errname'] = 'un_corresponding';
        return false;
    }

    // UTN取得
    if (!scm_get_imode_utn($ser_id)) {
        SCM_Log::trace("can not get UTN.");
        $params['errname'] = 'no_utn';
        return false;
    }

    $user_id = @str_replace(";","-",$ser_id);
    $params['JAMURL'] = $user_id.'/scm.jam';

    return true;
}


/**
 *  ダウンロードページビュー(mVc)
 *
 *  @param array $params テンプレートへ埋込値の配列
 */
function scm_web_v_download($params)
{
    $xtpl = new XTemplate(SCMD_PACKAGE_HTM_DIRECTORY.'/download.htm');
    $xtpl->assign('JAMURL',$params['JAMURL']);
    $xtpl->parse("main.head");
    $xtpl->parse("main.download");
    $xtpl->parse("main.foot");
    $xtpl->parse("main");
    $xtpl->out("main");
}

/**
 *  不正なリクエストに対するエラーページ(mVc)
 *
 *  @param array $params テンプレートへ埋込値の配列
 */
function scm_web_v_download_e($params)
{
    $xtpl = new XTemplate(SCMD_PACKAGE_HTM_DIRECTORY.'/download_err.htm');
    $xtpl->parse("main.head");
    $xtpl->parse('main.'.$params['errname']);
    $xtpl->parse("main.foot");
    $xtpl->parse("main");
    $xtpl->out("main");
}

?>
