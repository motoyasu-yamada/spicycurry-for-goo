<?php
    require_once 'DB.php';

/**
 *  アクセスログファイルを読み込む<br>
 */
class AccessLogLoader
{
    /**
     *  データベースに接続し接続オブジェクトを返す
     *  @return DB データベースへの接続
     */
    function connect()
    {
        global $SCMD_DSN;
        $cn = DB::connect($SCMD_DSN);
        if (DB::isError($cn)) {
            var_dump($cn);
            scm_fatal_html();
        }
        return $cn;
    }

    /**
     *  ユニークユーザのログを取得する
     *  
     */
    function getUnuqueLog($startd,$endd)
    {
        $cn = AccessLogLoader::connect();
        $sql = "SELECT access_date,
                       sum(access_total)  AS access_total,
                       sum(access_unique) AS access_unique
                       FROM  t_log_unique 
                       WHERE (1 = 1) ";
        if ($startd) {
            $sql .= "  AND '$startd' <= access_date";
        }
        if ($endd) {
            $sql .= "  AND access_date <= '$endd'";
        }
        $sql .="       GROUP BY access_date
                       ORDER BY access_date";
        $r = $cn->query($sql);
        assert(!DB::isError($r));
        $logs = array();
        while ($row = $r->fetchRow(DB_FETCHMODE_ASSOC)) {
            array_push($logs,$row);
        }
        $cn->disconnect();
        return $logs;
    }

    /**
     *  指定した開始日と終了日の間の日をリストアップする
     *  @static
     *  @return 日付配列
     */
    function makeDates($startd,$endd)
    {
        assert($startd);
        assert($endd);
        
        $cur  = $startd;
        $dates = array();
        for (;;) {
            if (strcmp($endd,$cur) < 0) {
                break;
            }
            $dates[] = array('date' => $cur);
            $mt  = mktime(0,0,0,(int)substr($cur,4,2),(int)substr($cur,6,2) + 1,(int)substr($cur,0,4));
            $cur = date("Ymd",$mt);
        }
        return $dates;
    }

    /**
     *  ドメイン別アクセスの二次元表を作成する
     *  @param $dates  集計対象期間
     */
    function getDomainLog($dates)
    {
        assert(is_array($dates));
        $startd = $dates[0]['date'];
        $endd   = $dates[count($dates) - 1]['date'];

        $cn    = AccessLogLoader::connect();
        $sql = "SELECT access_domain,
                      sum(access_count) AS access_total
                      FROM t_log_domain
                       WHERE (1 = 1) ";
        if ($startd) {
            $sql .= "  AND '$startd' <= access_date";
        }
        if ($endd) {
            $sql .= "  AND access_date <= '$endd'";
        }
        $sql .= "     GROUP BY access_domain
                      ORDER BY access_total DESC
                      LIMIT 100";

       $r = $cn->query($sql);
       assert(!DB::isError($r));

       $logs = array();
       $num  = 1;
       while ($row = $r->fetchRow(DB_FETCHMODE_ASSOC)) {
            $domain     = $row['access_domain'];
            $accesses   = array();
            foreach($dates as $d) {
                $d2 = $d['date'];
                $sql = "SELECT sum(access_count) AS access_count
                            FROM    t_log_domain
                            WHERE   access_domain = '$domain' AND
                                    access_date   = '$d2'";
                $r2 = $cn->query($sql);
                assert(!DB::isError($r2));
                assert($r2->numRows() == 1);
                $row2 = $r2->fetchRow(DB_FETCHMODE_ASSOC);
                $access_count = (int)$row2['access_count'];
#                if ($access_count == 0) {
#                    $access_count = false;
#                }
                $accesses[] = array('access_count' => $access_count);
            }
            $row['num'] = $num++;
            $row['accesses'] = $accesses;
            array_push($logs,$row);
            $r2  = $cn->query($sql);
        }
        $cn->disconnect();
        reset($dates);
        return $logs;

#        応答イメージ
#        $d = array(array('date' => '20051202'),
#                   array('date' => '20051203'),
#                   array('date' => '20051204'));
#        $a = array(array('num'           => 1,
#                         'access_domain' => 'spicysoft.com',
#                         'access_total'  => 15,
#                         'accesses'      => array(array('access_count' => 10),array('access_count' => 3),array('access_count' => 2))
#                         ),
#                   array('num'           => 2,
#                        'access_domain'  => '2.spicysoft.com',
#                        'access_total'   => 14,
#                        'accesses'       => array(array('access_count' => 10),array('access_count' => 3),array('access_count' => 1))
#                        ),
#                   array('num'           => 3,
#                        'access_domain'  => '3.spicysoft.com',
#                        'access_total'   => 13,
#                        'accesses'       => array(array('access_count' => 10),array('access_count' => 3),array('access_count' => 0))
#                        )
#          );
#        reset($a);
#        reset($d);
#        return array($d,$a);
    }
    
}


?>

