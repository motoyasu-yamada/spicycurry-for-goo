#   $Id: copyres.pl,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $
#   コピーする
#
#   $Log: copyres.pl,v $
#   Revision 1.1.1.1  2005/03/04 10:31:16  m_yamada
#
#
#   Revision 1.3  2004/10/06 12:31:39  m_murai
#   バグ修正
#
#   Revision 1.2  2004/10/04 00:46:55  m_murai
#   getImage(byte[])がDoJa3.0以降しか対応していないので、
#   一旦スクラッチパッドのテンポラリ（固定場所）に保存して、
#   getImage(int pos)で呼び出すようにして対応。
#
#   最大ダウンロード長が10Kしかない端末のために、
#   テキスト系リソースとイメージ系リソースに分割して別途ダウンロードする形にした。
#
#   Revision 1.1.1.1  2004/09/09 07:30:07  m_murai
#   no message
#
#   Revision 1.1.1.1  2003/10/30 08:23:08  m_yamada
#   2003-10-29
#
#   Revision 1.1.1.1  2003/09/15 04:43:23  od
#
#
#   Revision 1.1  2003/06/21 05:48:27  m_yamada
#   非コンパイルリソースのコピー
#
#
use strict;
use strict 'vars';

our($srcdirPath)  = $ARGV[0];
our($targetsPath) = $ARGV[1];
our($outdirPath)  = $ARGV[2];

print "CopyRes ver 1.0\n";
print "\n";
print "SrcDirPath     = $srcdirPath \n";
print "TargetsPath    = $targetsPath\n";
print "OutDirPath     = $outdirPath \n";

open TARGETS,$targetsPath   or die "ターゲットファイルを読み込めない '$targetsPath'";

while(<TARGETS>) {
    if (/^\s*#.+$/) {
        next;
    }
    if (/^\s*$/) {
        next;
    }
    
    if (/\s*Target\s*=\s*([A-z0-9_]+)/) {
        print "ターゲット $1 のResource を複製中 \n";
        &copy($srcdirPath.'\\'.$1.'\\m0.gif',$outdirPath.'\\m0.gif.'.$1);
        next;
    }
    die "不正な行が見つかりました:$_";
}
close TARGETS;

sub copy() {
    my($src,$dst) = @_;
    print "copying $src -> $dst\n";
    `copy $src $dst` or die("$src を $dst に複製できませんでした。");
}