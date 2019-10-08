<?php
ini_set('include_path',"../conf-yamada/;".ini_get('include_path'));

require_once 'scm_misc.php';
require_once 'scm_test.php';

test_unescape_entity();

function test_unescape_entity()
{
    echo "unescape_entity\n";
    $s = unescape_entity("&#63647;");
    assertEquals(2,strlen($s));
    assertEquals(0xF89F,(ord($s[0]) << 8) + ord($s[1]));
}

?>