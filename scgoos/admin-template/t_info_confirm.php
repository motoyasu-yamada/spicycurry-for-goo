<h1>お知らせ更新機能</h1>

<hr>

<form action="info_done.php" method=POST>
<input type=hidden name="type"   value="<?php param('type');?>">
<input type=hidden name="starty" value="<?php param('starty');?>">
<input type=hidden name="startm" value="<?php param('startm');?>">
<input type=hidden name="startd" value="<?php param('startd');?>">
<input type=hidden name="starth" value="<?php param('starth');?>">
<input type=hidden name="startn" value="<?php param('startn');?>">

<input type=hidden name="endy"   value="<?php param('endy');?>">
<input type=hidden name="endm"   value="<?php param('endm');?>">
<input type=hidden name="endd"   value="<?php param('endd');?>">
<input type=hidden name="endh"   value="<?php param('endh');?>">
<input type=hidden name="endn"   value="<?php param('endn');?>">

<input type=hidden name="title"  value="<?php param('title');?>">
<input type=hidden name="url"    value="<?php param('url');?>">

お知らせを変更しますか？

<h2>現在</h2>
配信方法:<?php param('type'); ?><br>
配信期間:
<?php param('starty'  ); ?>年
<?php param('startm'  ); ?>月
<?php param('startd'  ); ?>日
<?php param('starth'  ); ?>時
<?php param('startn'  ); ?>分
～
<?php param('endy'  ); ?>年
<?php param('endm'  ); ?>月
<?php param('endd'  ); ?>日
<?php param('endh'  ); ?>時
<?php param('endn'  ); ?>分<br>
お知らせ:<?php param('title'); ?><br>
リンク先:<?php param('url'); ?><br>

<h2>過去</h2>
配信方法:<?php param('org:type'); ?><br>
配信期間:
<?php param('org:starty'  ); ?>年
<?php param('org:startm'  ); ?>月
<?php param('org:startd'  ); ?>日
<?php param('org:starth'  ); ?>時
<?php param('org:startn'  ); ?>分
～
<?php param('org:endy'  ); ?>年
<?php param('org:endm'  ); ?>月
<?php param('org:endd'  ); ?>日
<?php param('org:endh'  ); ?>時
<?php param('org:endn'  ); ?>分
<br>
お知らせ:<?php param('org:title'); ?><br>
リンク先:<?php param('org:url'); ?><br>


<hr>
<input type=submit value="お知らせ配信設定を登録する">
</form>
