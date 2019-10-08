<h1>ドメイン集計結果</h1>

集計期間:
<?php param_date('startd','Y/n/j'); ?>～
<?php param_date('endd','Y/n/j'); ?><br>
集計日数:<?php param('dates_count')?><br>


<table border=1>
<tr>
    <th> # </th>
    <th>ドメイン名</th>
    <th>累計</th>
    <?php while(loop($dates)) { ?>
    <th><?php param_date('date','n/j'); ?></th>
    <?php }?>
</tr>

<?php while(loop($domains)) { ?>
<tr>
    <td><?php param("num");           ?></td>
    <td><?php param("access_domain"); ?></td>
    <td><?php param("access_total");  ?></td>
<?php   while(loop($GLOBALS['accesses'])) { ?>
    <td><?php param("access_count"); ?></td>
<?php   } ?>
</tr>
<?php } ?>

</table>
