<?php
require_once 'scm_log.php';

assert_options (ASSERT_ACTIVE,    1);
assert_options (ASSERT_WARNING,   1);
assert_options (ASSERT_QUIET_EVAL,1);
assert_options (ASSERT_CALLBACK,  'scm_test_assert_handler');

/**
 *  期待する値かどうかチェックする
 */
function assertEquals($e,$v)
{
    if ($e === $v) {
        return;
    }
    echo SCM_Log::get_stack_trace();

    echo "*** ASSERT ***\n";
    echo "expected \n";
    var_dump($e);
    echo "but was \n";
    var_dump($v);
    exit;
}

/**
 *  カスタム assertコールバックハンドラ
 */
function scm_test_assert_handler($file, $line, $code)
{
    $msg   = "*** ASSERT *** '$file'($line) : '$code'";
    echo SCM_Log::get_stack_trace();
    echo $msg;
    SCM_Log::error($msg);
    exit;
}

?>