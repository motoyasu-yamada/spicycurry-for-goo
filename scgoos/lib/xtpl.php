<?php

class XTemplate {

/*
    xtemplate class 0.2.4-3
    html generation with templates - fast & easy
    copyright (c) 2000 barnab疽 debreceni [cranx@users.sourceforge.net]
    code optimization by Ivar Smolin <okul@linux.ee> 14-march-2001
    latest stable & CVS version always available @ http://sourceforge.net/projects/xtpl

    tested with php 3.0.11 and 4.0.4pl1

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    version 2.1 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details at 
    http://www.gnu.org/copyleft/lgpl.html
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    $Id: xtpl.php,v 1.1.1.1 2005/03/04 10:31:17 m_yamada Exp $
*/

/***[ variables ]***********************************************************/

var $filecontents="";                               /* raw contents of template file */
var $blocks=array();                                /* unparsed blocks */
var $parsed_blocks=array();                 /* parsed blocks */
var $block_parse_order=array();         /* block parsing order for recursive parsing (sometimes reverse:) */
var $sub_blocks=array();                        /* store sub-block names for fast resetting */
var $VARS=array();                                  /* variables array */

var $file_delim="/\{FILE\s*\"([^\"]+)\"\s*\}/m";  /* regexp for file includes */
var $block_start_delim="<!-- ";         /* block start delimiter */
var $block_end_delim="-->";                 /* block end delimiter */
var $block_start_word="BEGIN:";         /* block start word */
var $block_end_word="END:";                 /* block end word */

/* this makes the delimiters look like: <!-- BEGIN: block_name --> if you use my syntax. */

var $NULL_STRING=array(""=>"");             /* null string for unassigned vars */
var $NULL_BLOCK=array(""=>"");  /* null string for unassigned blocks */
var $mainblock="";
var $ERROR="";
var $AUTORESET=1;                                       /* auto-reset sub blocks */

/*
*   コンストラクタ
*
*   @param  string $file        テンプレートファイルパス
*   @param  string $mainblock   テンプレートメインブロック名
*/
function XTemplate ($file,$mainblock="main")
{
    if (!file_exists($file)) {
        SCM_Log::error("template file not found : $file");
        assert(false);
    }

    $this->mainblock = $mainblock;
    /* read in template file */
    $this->filecontents = $this->r_getfile($file);
    /* preprocess some stuff */
    $this->blocks = $this->maketree($this->filecontents,$mainblock);
    $this->scan_globals();
}


/***************************************************************************/
/***[ public stuff ]********************************************************/
/***************************************************************************/


/*
*   ラジオボタンを出力する
*
*   @param  string $name   ラジオボタンの名前
*   @param  string $value  現在の値
*   @param  string $values ラジオボタンの取りうる値集合
*   @return void
*/
function radio($name,$value,$values) 
{
    assert(is_string($name));
    assert(is_array($values));
    assert(gettype($values[0]) === gettype($value));

    $this->assign($name,$value);
    foreach($values as $i) {
        $checked = '';
        if ($value === $i) {
            $checked = 'checked';
        }
        $this->assign(strtoupper($name.'_'.$i),$checked);
    }
}

/*
*   assign a variable 
*/
function assign ($name,$val="")
{
    if (gettype($name)=="array") {
        while (list($k,$v)=each($name)) {
            $this->VARS[$k]=$v;
        }
    } else {
        $this->VARS[$name]=$val;
    }
}

/***[ parse ]***************************************************************/
/*
*   parse
*   @param  string  $bname  
*   @return void
*/
function parse ($bname)
{
    if (!isset($this->blocks[$bname])) {
        $this->blocks[$bname] = '';
        $this->set_error("parse: blockname [$bname] does not exist");
    }
    $copy = $this->blocks[$bname];

    preg_match_all("/\{([A-Za-z0-9\._]+?)}/",$copy,$var_array);
    $var_array=$var_array[1];

    while (list($k,$v)=each($var_array)) {
        $sub=explode(".",$v);
        if ($sub[0]=="_BLOCK_") {
            unset($sub[0]);
            $bname2=implode(".",$sub);
            if (isset($this->parsed_blocks[$bname2])) {
                $var = $this->parsed_blocks[$bname2];
            } else {
                $var = '';
            }
            $nul=(!isset($this->NULL_BLOCK[$bname2])) ? $this->NULL_BLOCK[""] : $this->NULL_BLOCK[$bname2];
            $var=(empty($var))?$nul:trim($var);
            $copy = str_replace('{'.$v.'}',$var,$copy);
        //  変数を置換する
        } else {
            $var = $this->VARS;
            foreach($sub as $v) {
                if (isset($var[$v])) {
                    $var = $var[$v];
                } else {
                    $var = '';
                }
            }

            if (!isset($this->NULL_STRING[$v])) {
                $nul = $this->NULL_STRING[""];
            } else {
                $nul = $this->NULL_STRING[$v];
            }
            //assert($nul == (!isset($this->NULL_STRING[$v])) ? ($this->NULL_STRING[""]) : ($this->NULL_STRING[$v]));

            if (!isset($var)) {
                $var = $nul;
            }

            //  pregだと置換後の値に$1や\1があると消えてしまう問題の修整
            //  差込
            $copy = str_replace('{'.$v.'}',$var,$copy);
        }
    }
    if (!isset($this->parsed_blocks[$bname])) {
        $this->parsed_blocks[$bname] = '';
    }
    $this->parsed_blocks[$bname].=$copy;
    // reset sub-blocks 
    if ($this->AUTORESET && (!empty($this->sub_blocks[$bname]))) {
        reset($this->sub_blocks[$bname]);
        while (list($k,$v)=each($this->sub_blocks[$bname]))
            $this->reset($v);
    }
}

/***[ rparse ]**************************************************************/
/*
    returns the parsed text for a block, including all sub-blocks.
*/

function rparse($bname) {
    if (!empty($this->sub_blocks[$bname])) {
        reset($this->sub_blocks[$bname]);
        while (list($k,$v)=each($this->sub_blocks[$bname]))
            if (!empty($v)) 
                $this->rparse($v,$indent."\t");
    }
    $this->parse($bname);
}

/***[ insert_loop ]*********************************************************/
/*
    inserts a loop ( call assign & parse )
*/

function insert_loop($bname,$var,$value="") {
    $this->assign($var,$value);     
    $this->parse($bname);
}

/***[ text ]****************************************************************/
/*
    returns the parsed text for a block
*/

function text($bname) {
    return $this->parsed_blocks[isset($bname) ? $bname :$this->mainblock];
}

/***[ out ]*****************************************************************/
/*
    prints the parsed text
*/

function out ($bname) {
    echo $this->text($bname);
}

/***[ reset ]***************************************************************/
/*
    resets the parsed text
*/

function reset ($bname) {
    $this->parsed_blocks[$bname]="";
}

/***[ parsed ]**************************************************************/
/*
    returns true if block was parsed, false if not
*/

function parsed ($bname) {
    return (!empty($this->parsed_blocks[$bname]));
}

/***[ SetNullString ]*******************************************************/
/*
    sets the string to replace in case the var was not assigned
*/

function SetNullString($str,$varname="") {
    $this->NULL_STRING[$varname]=$str;
}

/***[ SetNullBlock ]********************************************************/
/*
    sets the string to replace in case the block was not parsed
*/

function SetNullBlock($str,$bname="") {
    $this->NULL_BLOCK[$bname]=$str;
}

/***[ set_autoreset ]*******************************************************/
/*
    sets AUTORESET to 1. (default is 1)
    if set to 1, parse() automatically resets the parsed blocks' sub blocks
    (for multiple level blocks)
*/

function set_autoreset() {
    $this->AUTORESET=1;
}

/***[ clear_autoreset ]*****************************************************/
/*
    sets AUTORESET to 0. (default is 1)
    if set to 1, parse() automatically resets the parsed blocks' sub blocks
    (for multiple level blocks)
*/

function clear_autoreset() {
    $this->AUTORESET=0;
}

/***[ scan_globals ]********************************************************/
/*
    scans global variables
*/

function scan_globals() {
    reset($GLOBALS);
    while (list($k,$v)=each($GLOBALS))
        $GLOB[$k]=$v;
    $this->assign("PHP",$GLOB); /* access global variables as {PHP.HTTP_HOST} in your template! */
}

/******

        WARNING
        PUBLIC FUNCTIONS BELOW THIS LINE DIDN'T GET TESTED

******/


/***************************************************************************/
/***[ private stuff ]*******************************************************/
/***************************************************************************/

/***[ maketree ]************************************************************/
/*
    generates the array containing to-be-parsed stuff:
  $blocks["main"],$blocks["main.table"],$blocks["main.table.row"], etc.
    also builds the reverse parse order.
*/


function maketree($con,$block) {
    $con2=explode($this->block_start_delim,$con);
    $level=0;
    $block_names=array();
    $blocks=array();
    reset($con2);
    while(list($k,$v)=each($con2)) {
        $patt="($this->block_start_word|$this->block_end_word)\s*(\w+)\s*$this->block_end_delim(.*)";
        if (preg_match_all("/$patt/ims",$v,$res, PREG_SET_ORDER)) {
            // $res[0][1] = BEGIN or END
            // $res[0][2] = block name
            // $res[0][3] = kinda content
            if ($res[0][1]==$this->block_start_word) {
                $parent_name=implode(".",$block_names);
                $block_names[++$level]=$res[0][2];                          /* add one level - array("main","table","row")*/
                $cur_block_name=implode(".",$block_names);  /* make block name (main.table.row) */
                $this->block_parse_order[]=$cur_block_name; /* build block parsing order (reverse) */
                
                /* add contents */
                if (!isset($blocks[$cur_block_name])) {
                    $blocks[$cur_block_name] = '';
                }
                $blocks[$cur_block_name] .= $res[0][3];
                
                /* add {_BLOCK_.blockname} string to parent block */
                if (!isset($blocks[$parent_name])) {
                    $blocks[$parent_name]    = '';
                }
                $blocks[$parent_name].="{_BLOCK_.$cur_block_name}";

                $this->sub_blocks[$parent_name][]    = $cur_block_name;      /* store sub block names for autoresetting and recursive parsing */
                $this->sub_blocks[$cur_block_name][] = "";        /* store sub block names for autoresetting */
            } else if ($res[0][1]==$this->block_end_word) {
                unset($block_names[$level--]);
                $parent_name=implode(".",$block_names);
                $blocks[$parent_name].=$res[0][3];  /* add rest of block to parent block */
            }
        } else { /* no block delimiters found */
            $name = implode(".",$block_names);
            if (!isset($blocks[$name])) {
                $blocks[$name] = '';
            }
            $blocks[$name] .= $this->block_start_delim.$v;
        }
    }
    return $blocks; 
}



/***[ error stuff ]*********************************************************/
/*
    sets and gets error
*/

function get_error()    {
    return ($this->ERROR=="")?0:$this->ERROR;
}


function set_error($str)    {
    $this->ERROR=$str;
}

/***[ getfile ]*************************************************************/
/*
    returns the contents of a file
*/

function getfile($file) {
    if (!isset($file)) {
        $this->set_error("!isset file name!");
        return "";
    }

    if (is_file($file)) {
        if (!($fh=fopen($file,"r"))) {
            $this->set_error("Cannot open file: $file");
            return "";
        }

        $file_text=fread($fh,filesize($file));
        fclose($fh);
    } else { 
        $this->set_error("[$file] does not exist");
        $file_text="<b>__XTemplate fatal error: file [$file] does not exist__</b>";
    }
        
    return $file_text;
}

/***[ r_getfile ]***********************************************************/
/*
    recursively gets the content of a file with {FILE "filename.tpl"} directives
*/


function r_getfile($file)
{
    $text=$this->getfile($file);
    while (preg_match($this->file_delim,$text,$res)) {
        $text2 = $this->getfile($res[1]);
        $var   = $res[0];
        $text  = str_replace("'".$var."'",$text2,$text);
    }
    return $text;
}

} /* end of XTemplate class. */

?>
