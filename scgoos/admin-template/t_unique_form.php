<script Language="JavaScript">
function clearStart()
{
    document.myform.starty.value = "";
    document.myform.startm.value = "";
    document.myform.startd.value = "";
}
function clearEnd()
{
    document.myform.endy.value = "";
    document.myform.endm.value = "";
    document.myform.endd.value = "";
}
</script>


<h1>ユニークユーザ集計</h1>

<form action="unique_done.php" method="POST" name="myform">
集計期間：<br>
<input type=text name="starty" value="" size="5">年
<input type=text name="startm" value="" size="3">月
<input type=text name="startd" value="" size="3">日
<input type=button onClick="clearStart()" value="クリア">
～
<input type=text name="endy" value="" size="5">年
<input type=text name="endm" value="" size="3">月
<input type=text name="endd" value="" size="3">日
<input type=button onClick="clearEnd()" value="クリア">
<br>
<input type=submit value="集計する">
</form>

<hr>
<ul char="※">
<li>開始または終了期間を指定しない場合は入力しないでください</li>
</ul>

