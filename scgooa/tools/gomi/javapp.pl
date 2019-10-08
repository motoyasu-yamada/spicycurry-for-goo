# $Id: javapp.pl,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $
# VisualC++ CL.EXEを JAVAコードのプリプロセス用に呼び仇巣
#
# 1- プリプロセス処理する
# 起動: javapp.pl <CppFilePath> <JavaFilePath>
# プリプロセス処理をして  <FileName>.java というファイルを生成します
# 元ファイル名/行の対応表 <FileName>.line というファイルを生成します
#
# 2- Antのjavac出力結果のエラー行、エラーファイル名を元のCPPファイル名に変換する
#     起動: javapp.pl
#     標準入力からANTのjavac出力結果を読み取ります
#     エラー位置組合せ <FileName>.java,<LineNo> を 正規表現で取得します
#     <FileName>.lineを読み込み、(<FileName>.java,<LineNo>)を、(<FileName>.cpp,<LineNo>)に変換します
#     変換した結果を出力します
#
#   $Log: javapp.pl,v $
#   Revision 1.1.1.1  2005/03/04 10:31:16  m_yamada
#
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
#   Revision 1.2  2003/06/13 05:01:12  m_yamada
#   warning でも停止するように変更
#   error/warning 停止理由を出力するように
#
#
if ($ARGV[0] eq "pp") {
	&pp;
} else {
	&ppfilter;
}

sub pp() {
	print "* JavaPP\n";
	print "* copyright 2002 spicysoft\n";

	$cppfilepath  = $ARGV[1];
	$javafilepath = $ARGV[2];
	$ppcmd        = $ARGV[3]." ".$ARGV[4]." ".$ARGV[5]." ".$ARGV[6]." ".$ARGV[7];
	$linefilepath = $javafilepath;
	$linefilepath =~ s/(.+)\.java/$1.line/;
	print "* cppfilepath=$cppfilepath\n";
	print "* javafilepath=$javafilepath\n";
	print "* linefilepath=$linefilepath\n";
	print "* $ppcmd\n";

	$javaline = 0;
	$cmd      = "cpp32.exe $ppcmd $cppfilepath  2>&1 |";
	print "> $cmd\n";
	open CPP,$cmd or die "プリプロセス処理ができなかった";
	open JAVA,">".$javafilepath or die "変換結果のJAVAソースコードが保存できない";
	open LINE,">".$linefilepath or die "行情報ファイルが開けない";
	
	$cppline  = -1;
	$cppfname = $cppfilepath;
	while (<CPP>) {
		if ($cppline == -1) {
			$cppline++;
			print "* $_";
			next;
		}
		$javaline++;
		if (/: fatal error C\d\d\d\d:/) {
			print $_;
			die "プロプロセス中に致命的な障害 $_ \n";
		}
		if (/: warning C\d\d\d\d:/) {
			print $_;
			die "プロプロセス中に障害 $_\n";
		}
		if (/^\s*\#line\s+(\d+)\s+\"(.+)\"$/) {
			$cppline  = $1-1;
			$cppfname = $2;
			print (JAVA "\n");
		} else {
			$cppline++;
			print (JAVA $_);
		}
		print (LINE $cppline.",".$cppfname."\n");
	}
	close(CPP);
	close(JAVA);
	close(LINE);
}

sub ppfilter() {
	print "* JavaPP\n";
	print "* copyright 2002 spicysoft\n";
	
	my($indent);
	my($linefilepath);
	my($javaerrline);
	my($javaerrdesc);
	
	$filepath = $ARGV[0];
	$linedir  = $ARGV[1];
	
	open FILE,$filepath or "ファイルが読み込めない";
	
	$error = undef;

	while (<FILE>) {
		if (/^(\s+\[junit\]\s+at\s+[\w\.]+)\((\w+)\.java\:(\d+)\)$/) {
			$indent       = $1;
			$linefilepath = $linedir . "\\" . $2.".line";
			$javaerrline  = $3;
			&get_line($linefilepath,$2.".java",$javaerrline);
			print $indent."(".$cpppath.":".$lineno.")\n";
			$error = true;

		} elsif (/^(\s*)(.+)\.java\:(\d+)\:(.+)$/) {
			$indent       = $1;
			$linefilepath = $2.".line";
			$javaerrline  = $3;
			$javaerrdesc  = $4;
			&get_line($linefilepath,$2.".java",$javaerrline);
			print $indent." ".$cpppath.":".$lineno.":".$javaerrdesc."\n";
			$error = true;

		} else {
			print $_;
		}
	}
	
	close(FILE);
	if ($error) {
		die 0;
	}
}

sub get_line() {
	local($linefilepath,$javapath,$javaerrline) = @_;

	my($line);
	my($lines);
	
	open(LINE,$linefilepath);
	@lines = <LINE>;
	close(LINE);

	$line = $lines[$javaerrline-1];
	if ($line =~ /^(\d+)\,(.+)$/) {
		$cpppath = $2;
		$lineno  = $1;
	} else {
		$cppath  = $javapath;
		$lineno  = $javaerrline;
	}
}
