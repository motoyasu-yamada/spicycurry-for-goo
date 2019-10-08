<h1>ユニークユーザ集計結果</h1>

<table border=1>
<tr><th>日時</th><th>トータルPV</th><th>ユニーク</th></tr>

<?php   while(loop($access)) {  ?>
<tr>
    <td><?php param('access_date');  ?></td>
    <td><?php param('access_total'); ?></td>
    <td><?php param('access_unique');?></td>
</tr>
<?php   } ?>

</table>
