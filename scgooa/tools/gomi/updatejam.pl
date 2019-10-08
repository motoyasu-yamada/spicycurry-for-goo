use strict;
use strict 'vars';

my($flags);
my($jampath);
my($jarpath);
my($filename);
my($gmt, $cook, @t, @m, @w); 
my($last_modified);
my($jad);
my($appver);

$flags   = $ARGV[0];
$jampath = $ARGV[1];
$jarpath = $ARGV[2];

$filename = @_;
my(
    $dev,   $ino,   $mode, $nlink,
    $uid,   $gid,   $rdev, $size, 
    $atime, $mtime, $ctime,$blksize, 
    $blocks
)   = stat($jarpath);

@t = localtime(time);
@m = ('Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'); 
@w = ('Sun','Mon','Tue','Wed','Thu','Fri','Sat'); 

$last_modified = sprintf(
    "%s, %02d %s %04d %02d:%02d:%02d", 
    $w[$t[6]],
    $t[3],
    $m[$t[4]],
    $t[5]+1900,
    $t[2],
    $t[1],
    $t[0]);

$appver        = sprintf(
    "%02d%02d%02d%02d%02d",
    $t[5] % 100,
    $t[4] + 1,
    $t[3],
    $t[2],
    $t[1]);

print "$last_modified\n";

open JAM,$jampath;
while (<JAM>) {
    if ($flags =~ /s/i) {
        s/\$\{JarSize\}/$size/;
    }
    if ($flags =~ /l/i) {
        s/\$\{LastModified\}/$last_modified/;
    }
    if ($flags =~ /v/i) {
        s/\$\{AppVer\}/$appver/;
    }
    $jad .= $_;
}
close JAM;

open JAM,"> $jampath";
print JAM $jad;
close JAM;
