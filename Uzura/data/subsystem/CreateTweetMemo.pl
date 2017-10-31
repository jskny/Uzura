#! /usr/local/bin/perl
#---------------------------------------
# Tweet から名刺を生成する。
# use:
#	perl a.pl screen_name text create_at saveUrl
#
# 2012 / 11 / 14
# jskny
#---------------------------------------


# http://x68000.q-e-d.net/~68user/webcgi/sample/perl/image-2.pl


use strict;
use GD;
use utf8;

# $FindBin::Bin で実行ファイルのあるディレクトリのアドレスが取得できる。
use FindBin;


my $userScreenName = $ARGV[0];
my $text = $ARGV[1];
my $createAt = $ARGV[2];


my $width  = 600;			# 画像の幅
my $height = 200;			# 画像の高さ

my $im = new GD::Image($width, $height);

my $black = $im->colorAllocate(  0,   0,   0);
my $str_color = $im->colorAllocate(  0,   0,   0);
my $str_color2 = $im->colorAllocate( 20,   20,  20);
my $bgcolor = $im->colorAllocate(255, 255, 255);

# インタレースを ON
$im->interlaced('true');
# 背景色として塗りつぶし四角を描く
$im->filledRectangle(0, 0, $width-1, $height-1, $bgcolor);
# 外枠を描画
$im->rectangle(0, 0, $width-1, $height-1, $black);


my $font_file ='C:/Windows/Fonts/jadhei01m.ttf';


# Icon を表示せよ。
# 接合せよ。
my $imIcon = GD::Image->new($FindBin::Bin . "/" . $userScreenName . ".png");
$im->copyResized($imIcon, 4, 4, 0, 0, 64, 64, $imIcon->width, $imIcon->height);
$im->rectangle(4, 4, 64+4, 64+4, $black);


# 文字を描画
$im->stringFT($str_color,	# 色
	$font_file, 14,	# フォント・フォントサイズ
	0,	# 回転角度
	80, 40,		# X・Y 座標
	$userScreenName);			# 表示文字列
$im->stringFT($str_color2, $font_file, 8, 0, 400, 40, $createAt);


my $str = $text;
my @tmp = split(//, $str);
my $tmpStr = "";
my $counter = 0;
my @retArray;
for (my $i = 0; $i < @tmp; $i++) {
	$tmpStr .= $tmp[$i];
	$counter++;


	if ($counter >= 20) {
		# 二十文字で折り返す。
		$counter = 0;
		push(@retArray, $tmpStr);
		$tmpStr = "";
	}
}
# 最後のを追加。
push(@retArray, $tmpStr);
$tmpStr = "";


for (my $i = 0; $i < @retArray; $i++) {
	$im->stringFT($str_color2, $font_file, 16, 0, 20, 90 + 22*$i, $retArray[$i]);
}


binmode STDOUT;
open FH, "> " . $ARGV[3];
binmode FH;
print FH $im->png;
close (FH);

exit 0;

