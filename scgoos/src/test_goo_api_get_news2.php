<?php
ini_set('include_path',"..\\conf-yamada;".ini_get('include_path'));
require_once 'scm_test.php';
require_once 'goo_api_get_news2.php';

testParseInfo();
testParseNews();
testAppVer();
testProtocol();

/**
 *  お知らせ情報の解析
 */
function testParseInfo()
{
    echo "testParseInfo\n";

    $action = new GetNews2();

    $info = $action->parseInfo("none");
    assertEquals(false,$info);

    $info = $action->parseInfo("exists\nINFO\thttp://\n");
    assertEquals(2,count($info));
    assertEquals("INFO",$info[0]);
    assertEquals("http://",$info[1]);

    $info = $action->parseInfo("timer:200501010000200601010000\nINFO\thttp://\n");
    assertEquals(2,     count($info));
    assertEquals("INFO",$info[0]);
    assertEquals("http://",$info[1]);

    $info = $action->parseInfo("timer:----------200612011300\nINFO\thttp://\n");
    assertEquals(false,$info);
}

/**
 *  ニュース情報の解析
 */
function testParseNews()
{
    echo "testParseNews\n";

    $action = new GetNews2();
    $news   = $action->parseNews("NEWS1\tURL1\n".
                                 "NEWS2\tURL2\n".
                                 "NEWS3\tURL3\n");
    assertEquals(3,     count($news));
    assertEquals("NEWS1",   $news[0][0]);
    assertEquals("URL1",    $news[0][1]);
    assertEquals("NEWS3",   $news[2][0]);
    assertEquals("URL3",    $news[2][1]);
}

/**
 *  アプリバージョンの取得
 */
function testAppVer()
{
    echo "testAppVer\n";

    $action = new GetNews2();

    global $SCM_REQUEST;
    $SCM_REQUEST['dev_id']      = 'f900i';
    $SCM_REQUEST['platform_id'] = '900i';
    $action->getJam();

    $ver = $action->getJamVersion("AppName = YAMADA\r\nAppVer=VERSION\r\n");
    assertEquals("VERSION",$ver);
}

function testProtocol()
{
    goo_api_get_news1('',new SCM_InputStream(''),new SCM_OutputStream());
    goo_api_get_info1('',new SCM_InputStream(''),new SCM_OutputStream());
    goo_api_get_news2('',new SCM_InputStream("\1\0V"),new SCM_OutputStream());
}

?>