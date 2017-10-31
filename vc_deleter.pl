#---------------------------------------
# VC++ �̃S�~�������X�N���v�g
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
	print "go �I�v�V������t���Ȃ��ƁA���s���܂���B" . "\n";
	print "project_name : ��������v���W�F�N�g���B" . "\n";

	return;
}


# �����֐��B
sub Deleter
{
	rmtree("Debug", 1); # rmtree �̑������� true �ɂ���Əo�͂���B
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
		# �폜�����t�@�C������ 0 �Ȃ�폜���Ă��Ȃ��̂ŁA�o�͂��Ȃ��B
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


# ���C���B
sub main
{
	if (@ARGV <= 1) {
		Help();
		return;
	}


	Deleter();
	return;
}

