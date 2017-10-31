#---------------------------------------
# CountSourceEnter
# Again のソースコードはどれぐらいあるのか
# 知りたくなったので作る。
# 2011 / 11 / 07
# jskny
#---------------------------------------


use strict;
use File::Copy;
use File::Path;
use FindBin;

use utf8;


# 改行数。
my $g_enterCounter = 0;
# 文字数
my $g_stringLenght = 0;


if (@ARGV > 0)
{
	if ($ARGV[0] ne "go") {
		print "option is unjust.\n";
		exit (0);
	}

	reflexiveFile('.');
}
else {
	print "CountEnter.pl go\n";
	print "\tgo オプションを付けないと実行しません。\n";
	print "\tjskny\n";
	exit (0);
}


print "Found Enter : ${g_enterCounter}\n";
print "Str Length  : ${g_stringLenght}\n";
print "Finish\n";
exit (0);


#---------------------------------------


sub Error
{
	my $errText = @_[0];
	print $errText . "\n";
	exit (0);
}


# ソースコードの行数を取得
sub GetCountFileEnter
{
	my ( $urlFile, $option ) = @_;
	my @flist;
	my $i = 0;

	open(FH, "+< $urlFile") or err("can't open file.");
	eval { flock(FH, 2); };
	seek (FH, 0, 0);
	@flist = <FH>;
	close (FH);


print "Open : ${urlFile} : ";
	# 出力。
	my $strLength = 0;
	foreach my $tl ( @flist ) {
#		print $tl;
		$strLength += length($tl);
		$i++;
	}


	print "( enter = " . scalar(@flist) . ", str = $strLength )". "\n";
	$g_stringLenght += $strLength;
	return ($i);
}


#-------------------------------------------#
#■ディレクトリ配下を全て表示
#-------------------------------------------#

sub reflexiveFile
{
	my $dir = shift(@_); # $_[0] と同じ意味。
	my @list = ();
	my $count = 0;
	# そのディレクトリでのコード数。
	my $dirCountNum = 0;


	#-- カレントの一覧を取得 --#
	opendir(DIR, $dir) or die("Can not open directory:$dir ($!)");
	@list = readdir(DIR);
	closedir(DIR);

	foreach my $file (sort @list)
	{
		next if( $file =~ /^\.{1,2}$/ );	# '.' と '..' はスキップ

		#-- ディレクトリの場合は自分自身を呼び出す --#
		if (-d "${dir}/${file}" ) {
			reflexiveFile("${dir}/${file}");
		}
		elsif (-e "${dir}/${file}") {
			# ファイル発見
			my $buf = "${dir}/${file}";
			if ($buf =~ /(\.c|\.cpp|\.h|\.hpp|\.cc)$/) {
				# ソースファイルらな行数をカウント。
				my $tmp = GetCountFileEnter($buf);

				$g_enterCounter += $tmp;
				$dirCountNum += $tmp;
			}
		}


		# 安全のため時々休憩。
		if ($count > 100) {
			sleep (1);
			$count = 0;
		}

		$count++;
	}


	if ($dirCountNum > 0) {
		# ディレクトリの中の改行数。
		print("@>> ${dir} : ${dirCountNum}\n");
	}
	return;
}

