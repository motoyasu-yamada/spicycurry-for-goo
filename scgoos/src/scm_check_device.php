<?php
require_once 'scm_misc.php';

/**
 *  対応機種チェック
 *  非対応の場合、エラーページを表示
 *  $checkdevice = new SCM_Check_Device();
 */

Class SCM_Check_Device {

    // ユーザエージェント
    var $ua;

    /**
     *  コンストラクタ
     *  @access public
     */
    function SCM_Check_Device(){
        $this->ua = scm_get_array($_SERVER,'HTTP_USER_AGENT');
    }

    /**
     *  対応機種はダウンロードページ、非対応機種はエラーページ
     *  @access public
     *  @return boolean     true:対応, false:非対応
     */
    function canDownload(){
        if($this->_checkDevice()){
            return true;
        } else {
            SCM_Log::error("Un-corresponding: ". $this->ua);
            return false;
        }
    }

    /**
     *  対応機種かチェックする
     *  @access private
     *  @return boolean         true：対応、false：非対応
     */
    function _checkDevice() {

        // (504i)
        if (ereg('^DoCoMo/1\.0/([A-Z]+504i)/c[0-9]+/?.+',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        // (504is)
        } else if (ereg('^DoCoMo/1\.0/([A-Z]+504iS)/c[0-9]+/?.+',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        // (505i)
        } else if (ereg('^DoCoMo/1.0/([A-Z]+505i)/c[0-9]+/?.+',$this->ua,$match) ||
                   ereg('^DoCoMo/1.0/(SH505i)2/c[0-9]+/?.+',$this->ua,$match)
                   ) {
            $device   = strtolower($match[1]);
            return true;

        // (505is)
        } else if (ereg('^DoCoMo/1.0/([A-Z]+505iS)/c[0-9]+/?.+',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        // (505iS2)
        } else if (ereg('^DoCoMo/1.0/(SH505iS2)/c[0-9]+/?.+',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        // (505iGPS)
        } else if (ereg('^DoCoMo/1.0/(F505iGPS)/c[0-9]+/?.+',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        // (506i)
        } else if (ereg('^DoCoMo/1.0/([A-Z]+506i[A-Za-z0-9]*)/c[0-9]+/?.+',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        //  x900i
        } else if (ereg('^DoCoMo/2.0 ([A-Z]+900i[A-Za-z0-9]*)\(c[0-9]+;T[BCDJ].*\).*$',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            if ($device == "n900il"){
                return false;
            }
            return true;

        //   x901
        } else if (ereg('^DoCoMo/2.0 ([A-Z]+901i[A-Za-z0-9]*)\(c[0-9]+;T[BCDJ].*\).*$',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        //   x902
        } else if (ereg('^DoCoMo/2.0 ([A-Z]+902i[A-Za-z0-9]*)\(c[0-9]+;T[BCDJ].*\).*$',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            return true;

        //  FOMA
        } else if (ereg('^DoCoMo/2.0 ([A-Z]+[0-9]+[A-Za-z]*)\(c[0-9]+;T[BCDJ].*\).*$',$this->ua,$match)) {
            $device   = strtolower($match[1]);
            if ($device == "f2051" ||
                $device == "n2001" ||
                $device == "n2002" ||
                $device == "p2002" ||
                $device == "d2101v" ||
                $device == "p2101v" ||
                $device == "t2101v" ||
                $device == "sh2101v"
               ) {
                return false;
            }
            return true;

        } else {
            return false;
        }
    }
}

?>