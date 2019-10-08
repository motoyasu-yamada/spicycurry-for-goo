<?php
ini_set('include_path',"..\\conf-yamada;..\\src;".ini_get('include_path'));

include_once 'scm_test.php';
require_once 'info.php';
require_once 'admin_proxy.php';

testPack();
testParse();
testSaveLoad();
testLoadFromRequest();
testSplitDate();

function testSplitDate()
{
    echo "\n";
    $d = GooInfo::split_date("------------");
    assertEquals(5,count($d));
    assertEquals(false,$d[0]);
    assertEquals(false,$d[4]);
    
    
    $d = GooInfo::split_date("200501020345");
    assertEquals(5,count($d));
    assertEquals(2005,$d[0]);
    assertEquals(1,$d[1]);
    assertEquals(2,$d[2]);
    assertEquals(3,$d[3]);
    assertEquals(45,$d[4]);
}

function testPack()
{
    echo "pack\n";

    $info = new GooInfo();
    $info->setNone();
    assertEquals("none\n\n",$info->pack());

    $info->setExists("あ","http://test/");
    assertEquals("exists\nあ\thttp://test/\n",$info->pack());

    $info->setTimer("200501011223","200512313456","あ","http://test/");
    assertEquals("timer:200501011223200512313456\nあ\thttp://test/\n",$info->pack());
}

function testParse()
{
    echo "parse\n";

    $info = new GooInfo();
    $info->parse("none\n\n");
    assertEquals(GOO_INFO_TYPE_NONE,$info->getType());
    
    $info = new GooInfo();
    $info->parse("exists\nあ\t\n");
    assertEquals(GOO_INFO_TYPE_EXISTS,$info->getType());
    assertEquals("あ",$info->getTitle());
    assertEquals("",  $info->getURL());

    $info = new GooInfo();
    $info->parse("timer:200501011234200601015634\nあ\thttp://\n");
    assertEquals(GOO_INFO_TYPE_TIMER,$info->getType());
    $timer = $info->getTimer();
    assertEquals("200501011234", $timer[0]);
    assertEquals("200601015634", $timer[1]);
    assertEquals("あ",       $info->getTitle());
    assertEquals("http://",  $info->getURL());
}

function testSaveLoad()
{
    echo "saveload\n";

    $info = new GooInfo();
    $info->setNone();
    $info->save();
    
    $info2 = new GooInfo();
    $info2->load();
    assertEquals(GOO_INFO_TYPE_NONE,$info2->getType());
    $info2->setExists("あいうえお","http://test/");
    $info2->save();

    $info = new GooInfo();
    $info->load();
    assertEquals(GOO_INFO_TYPE_EXISTS,$info->getType());
}

function testLoadFromRequest()
{
    echo "testLoadFromRequest\n";

    $r = array('type' => 'none');
    $info = new GooInfo();
    $info->loadFromRequest($r);
    assertEquals(GOO_INFO_TYPE_NONE,$info->getType());

    $r = array('type' => 'exists','title' => 'TITLE','url' => 'URL');
    $info = new GooInfo();
    $info->loadFromRequest($r);
    assertEquals(GOO_INFO_TYPE_EXISTS,$info->getType());
    assertEquals('TITLE',$info->getTitle());
    assertEquals('URL',  $info->getURL());

    $r = array('type'   => 'timer:',
               'starty' => '2005','startm' => '1','startd' => '1',
               'starth' => '12','startn' => '1',
               'endy'   => '',    'endm'   => '', 'endd'   => '',
               'title'  => 'TITLE',
               'url'    => 'URL');
    $info = new GooInfo();
    $info->loadFromRequest($r);
    assertEquals(GOO_INFO_TYPE_TIMER,$info->getType());
    list($start,$end) = $info->getTimer();
    assertEquals('200501011201', $start);
    assertEquals(GOO_INFO_NODATE,$end);
    assertEquals('TITLE',$info->getTitle());
    assertEquals('URL',  $info->getURL());

}


?>
