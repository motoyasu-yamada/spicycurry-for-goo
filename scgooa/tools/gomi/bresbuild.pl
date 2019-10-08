$makePath   = $ARGV[0];
$resPath    = $ARGV[1];
$headerPath = $ARGV[2];

print "ResBuild ver 0.1\n";
print "MakeFile  =$makePath\n";
print "ResFile   =$resPath\n";
print "HeaderFile=$headerPath\n";

open MAKE,  $makePath
    or die "メイクファイルを読み込めない";
open HEADER,">".$headerPath
    or die "ヘッダファイルを書き込めない";

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
print HEADER "/* 更新日:".(1900+$year)."/".$mon."/".$mday." ".$hour.":".$min.":".$sec."*/\n";

$resources = 0;
while(<MAKE>) {
    if (/^\s*#.+$/) {
        next;
    }
    
    if (/\s*STRING\s+(\w+)\s+(.+)\s*$/) {
        &define($1,$resources);
        string($2);
        $resources++;
        next;
    }
    if (/\s*BINARY\s+(\w+)\s+(.+)\s*$/) {
        &define($1,$resources);
        binary($2);
        $resources++;
        next;
    }
    die "不正なメイクファイル:".$_;
}
close MAKE;



#   リソースブロック数を出力
    open RES,   ">".$resPath;
    binmode(RES);  
    
    $head_length = 2 + 2 + 2 * $resources;
    $data_length = length($RDATA);
    $total_length = $head_length + $data_length;
    if (0xFFFF < $total_length) {
        die "64Kを超えています";
    }
    
    $RHEAD  = pack("S",$total_length);
    $RHEAD .= pack("S",$resources);
    for ($n = 0;$n < $resources;$n++) {
        $start  = $head_length + $RSTAT[$n];
        print $start."\n";
        $RHEAD .= pack("S",$start);
    }
    
    syswrite RES,$RHEAD,length($RHEAD);
    syswrite RES,$RDATA,length($RDATA);

    close RES;


#   識別子を出力する
    sub define() {
        local($name,$id) = @_;
        print HEADER "#define ".$name." ".$id."\n";
    }

#   バイナリリソースを解析
    sub binary() {
        local($path) = @_;
        
        local($length);
        $length = -s $path;
        
        open BINARY,$path or die $path."を読み込めない";
        binmode(BINARY); 
        local($BINARY);
        sysread BINARY,$BINARY,$length;
        
        close BINARY;

        $RSTAT[$resources] = length($RDATA);
        $RDATA            .= $BINARY;
    }
    
#   文字列リソースを解析
    sub string() {
        local($path) = @_;

        open STRING,$path or die $path."を読み込めない";

        local($index);
        $index = 0;
        while (<STRING>) {
            if (/^\s*#.+$/ or /^\s*$/) {
                next;
            }
            if (/\s*(\w+)\s+(.+)$/) {
                &define($1,$index);
                local($string);
                $string = $2;
                $string =~ s/\\t/\t/;
                $string =~ s/\\n/\n/;
                $strings[$index] = $string;
                $index++;
                next;
            }
            die "不正な文字列:".$_;
        }
    
        close STRING;
    
        $RSTAT[$resources] = length($RDATA);
        $RDATA            .= pack("S",$index);
        local($n);
        for ($n = 0;$n < $index;$n++) {
            $string = $strings[$n];
            $RDATA .= pack("S",length($string));
            $RDATA .= $string;
        }
    }