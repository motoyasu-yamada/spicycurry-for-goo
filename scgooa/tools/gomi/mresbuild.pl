use strict;
use strict 'vars';

our($makePath)     = $ARGV[0];
our($_resPath)     = $ARGV[1];
our($_headerPath)  = $ARGV[2];
our($package)      = $ARGV[3];
our($gamen)        = $ARGV[4];
#   最大リソースファイルサイズ
our($maxlength)    = int($ARGV[5]);
our($resPath)    = '';
our($headerPath) = '';

our($RSTAT);
our($RDATA);
our($RHEAD);
our($resources);


print "ResBuild ver 1.0\n";
print "\n";
print "Package     = $package\n";
print "MakeFile    = $makePath\n";
print "ResFile     = $_resPath\n";
print "HeaderFile  = $_headerPath\n";
print "Gamen       = $gamen\n";

my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
print HEADER "/* 更新日:".(1900+$year)."/".$mon."/".$mday." ".$hour.":".$min.":".$sec."*/\n";

&build($gamen);

    #
    #   Build
    #
    sub build() {
        my($gamen) = @_;
        my($path);
        my($name);

        $resPath    = $_resPath;
        $resPath    =~ s/\$gamen/$gamen/;
        $headerPath = $_headerPath;
        $headerPath =~ s/\$gamen/$gamen/;
        print "---- Gamen: [$gamen] $resPath,$headerPath\n";
        open MAKE,  $makePath       or die "メイクファイルを読み込めない:$makePath";
        open HEADER,">".$headerPath or die "ヘッダファイルを書き込めない:$headerPath";

        $resources = 0;
        while(<MAKE>) {
            if (/^\s*#.+$/) {
                next;
            }
            if (/^\s*$/) {
                next;
            }
            if (/\s*STRING\s+(\w+)\s+(.+)\s*$/) {
                $path = $2;
                $name = $1;
                $path =~ s/\$\{gamen\}/$gamen/;
                $path =~ s/\$\{package\}/$package/;
                &define($name,$resources);
                &parse_string($path,0,0);
                $resources++;
                next;
            }
            if (/\s*BINARY\s+(\w+)\s+(.+)\s*$/) {
                $path = $2;
                $name = $1;
                $path =~ s/\$\{gamen\}/$gamen/;
                $path =~ s/\$\{package\}/$package/;
                &define($name,$resources);
                &binary($path);
                $resources++;
                next;
            }
            die "不正なメイクファイル:".$_;
        }
        close MAKE;


    #   リソースブロック数を出力
        open RES,   ">".$resPath;
        binmode(RES);  
    
        my($head_length ) = 2 + 2 + 2 * $resources;
        my($data_length ) = length($RDATA);
        my($total_length) = $head_length + $data_length;
        if ($main::maxlength < $total_length) {
            print " $main::maxlengthを超えています $main::respath($total_length)";
            exit 1;
        }

        $RHEAD  = pack("S",$total_length);
        $RHEAD .= pack("S",$resources);
        
        my($n);
        for ($n = 0;$n < $resources;$n++) {
            my($start);
            $start  = $head_length + $main::RSTAT[$n];
            # print $start."\n";
            $main::RHEAD .= pack("S",$start);
        }
    
        syswrite RES,$main::RHEAD;
        print sprintf("ヘッダーサイズ %04x (%d)\n",length($main::RHEAD),length($main::RHEAD));
        syswrite RES,$main::RDATA;
        close RES;

        $main::RSTAT = undef;
        $main::RDATA = undef;
        $main::RHEAD = undef;
    }

#   識別子を出力する
    sub define() {
        my($name,$id) = @_;
        print HEADER "#define ".$name." ".$id."\n";
    }

    #
    #   バイナリリソースを解析
    #
    sub binary() {
        my($path) = @_;
        my($length);
        my($BINARY);

        if (!(-e $path)){
            print $path."は存在しません";
            die;
        }
        $length = -s $path;
        open BINARY,$path or die $path."を読み込めない";
        binmode(BINARY);
        sysread BINARY,$BINARY,$length;
        close BINARY;

        print sprintf ("バイナリ $path -> size %04x from %04x\n",length($BINARY),length($RDATA));
        $main::RSTAT[$resources] = length($RDATA);
        $main::RDATA            .= $BINARY;
    }

    #
    #   文字列リソースを解析
    #
    sub parse_string() {
        my($path,$index,$child) = @_;
        my($string);
        print "ストリング $path\n";
        open STRING,$path
            or die $path."を読み込めない";

        while (<STRING>) {
            #   include resource.
            if (/^\s*#include\s+\"(.+)\"\s*$/) {
                $path = $1;
                $path =~ s/\$\{gamen\}/$main::gamen/;
                $path =~ s/\$\{package\}/$main::package/;
                $index = parse_string($path,$index,'true');
                next;
            }
            if (/^\s*#.+$/ or /^\s*$/) {
                next;
            }
            if (/\s*(\w+)\s+\"(.+)\"\s*$/) {
                &define($1,$index);
                $string = $2;
                $string =~ s/\\t/\t/;
                $string =~ s/\\n/\n/;
                $main::strings[$index] = $string;
                $index++;
                next;
            }
            if (/\s*(\w+)\s+(.+)\s*$/) {
                &define($1,$index);
                $string = $2;
                $string =~ s/\\t/\t/;
                $string =~ s/\\n/\n/;
                $main::strings[$index] = $string;
                $index++;
                next;
            }
            die "不正な文字列:".$_;
        }
    
        close STRING;
        if ($child eq 'true') {
            return $index;
        }
        
        print "書込位置 ".length($RDATA)."\n";
        $main::RSTAT[$resources] = length($RDATA);
        $main::RDATA            .= pack("S",$index);
        
        print "文字列数 $index\n";
        my($n);
        for ($n = 0;$n < $index;$n++) {
            $string = $main::strings[$n];
            $main::RDATA .= pack("S",length($string));
            $main::RDATA .= $string;
        }
        
        return $index;
    }

