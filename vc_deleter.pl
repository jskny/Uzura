#---------------------------------------
# VC++ のゴミを消すスクリプト
# 2011 / 08 / 08
# jskny
#---------------------------------------

use strict;
use File::Path;

#---------------------------------------


main();
exit (0);


#---------------------------------------


sub Help
{
	print "VC Deleter" . "\n";
	print "Version 1.000.00" . "\n";
	print "jskny" . "\n";
	print "\n";
	print "vc_deleter.pl go project_name" . "\n";
	print "go オプションを付けないと、実行しません。" . "\n";
	print "project_name : 消去するプロジェクト名。" . "\n";

	return;
}


# 消去関数。
sub Deleter
{
	rmtree("Debug", 1); # rmtree の第二引数を true にすると出力する。
	rmtree("Release", 1);
	rmtree("ipch", 1);

	my $projectName = $ARGV[1];
	my $delUrl = "";
	my $rc = 0;


	$delUrl = ${projectName} . ".sdf";
	$rc = unlink $delUrl;
	if ($rc) {
		print "unlink " . $delUrl . "\n";
	}


	my $inProUrl = "./" . ${projectName} . "/";
	my @delFiles = ( "Log.txt",
			"AtomEngine_Debug.log", "AtomEngine_Error.log",
			"RonoCoco_Debug.log", "RonoCoco_Error.log" );

	foreach my $fileName ( @delFiles ) {
		$delUrl = $inProUrl . $fileName;
		$rc = unlink $delUrl;
		# 削除したファイル数が 0 なら削除していないので、出力しない。
		if ($rc) {
			print "unlink " . $delUrl . "\n";
		}
	}


	$delUrl = $projectName . "/Debug";
	rmtree($delUrl, 1);
	$delUrl = $projectName . "/Release";
	rmtree($delUrl, 1);


	return;
}


# メイン。
sub main
{
	if (@ARGV <= 1) {
		Help();
		return;
	}


	Deleter();
	return;
}

