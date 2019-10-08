<script Language="JavaScript">
function update()
{
    timer = document.myform.type[2].checked;
    color = 
        document.myform.starty.disabled = !timer;
        document.myform.startm.disabled = !timer;
        document.myform.startd.disabled = !timer;
        document.myform.endy.disabled   = !timer;
        document.myform.endm.disabled   = !timer;
        document.myform.endd.disabled   = !timer;

    msg = !document.myform.type[0].checked;
        document.myform.title.disabled  = !msg;
        document.myform.url.disabled    = !msg;

}

</script>
<body onLoad="update()">
<h1>お知らせ更新機能</h1>

<hr>

<form action="info_confirm.php" method=POST name=myform>

<input type=radio name="type" value="none"    <?php param('checked_none'  ); ?> onClick="update();">
1. お知らせを表示しない<br>
<input type=radio name="type" value="exists"  <?php param('checked_exists'); ?> onClick="update();">
2. お知らせを表示する<br>
<input type=radio name="type" value="timer:"  <?php param('checked_timer' ); ?> onClick="update();">
3. お知らせの表示期間を指定する<br>

配信期間：<br>
<input type=text name="starty" value="<?php param('starty'  ); ?>" size="5">年
<input type=text name="startm" value="<?php param('startm'  ); ?>" size="3">月
<input type=text name="startd" value="<?php param('startd'  ); ?>" size="3">日
<input type=text name="starth" value="<?php param('starth'  ); ?>" size="3">時
<input type=text name="startn" value="<?php param('startn'  ); ?>" size="3">分

～
<input type=text name="endy" value="<?php param('endy'  ); ?>" size="5">年
<input type=text name="endm" value="<?php param('endm'  ); ?>" size="3">月
<input type=text name="endd" value="<?php param('endd'  ); ?>" size="3">日
<input type=text name="endh" value="<?php param('endh'  ); ?>" size="3">時
<input type=text name="endn" value="<?php param('endn'  ); ?>" size="3">分
<br>

<hr>

文言:<br>
<input type=text name="title" value="<?php param('title'); ?>" size=32><br>
リンク先:<br>
<input type=text name="url" value="<?php param('url'); ?>" size=32><br>

<br>

<input type=submit value="お知らせ更新">

</form>
</body>
