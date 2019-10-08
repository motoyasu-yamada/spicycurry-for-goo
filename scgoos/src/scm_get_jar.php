<?php
require_once 'scm_imode.php';
require_once 'scmd_conf.php';

/*
*   JARファイル要求
*   @param int $user_id 内部系ユーザID
*/
function scm_get_jar($user_id) 
{
    $fname = scm_get_app_fname();
    $path = SCMD_PACKAGE_APP_DIRECTORY.'/'.$fname.'.jar';

    if (!file_exists($path)) {
        scm_exit_404("jar file is not exists.");
    }
    $size = filesize($path);

    SCM_Log::trace("path : ",$path);
    SCM_Log::trace("size * ",$size);

    header("Content-type: application/java-archiver"); 
    header("Content-Disposition: filename=scm.jar");
    header("Content-Length: $size");
    readfile($path);
}

?>