/*
 *	Uzura
 *	カラを破ってもう一度
 *	2012 / 08 / 06
 *	jskny
*/


// コマンドラインを消す。
// #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")


#include	<iostream>
#include	<aen.h>
#pragma	comment(lib, "AtomEngine.lib")
#include	<boost/thread.hpp>


#include	"uzura.h"
#include	"g_title.h"


#include	"scripting.h"
#include	"input.h"


#include	"scene.h"


#include	"effect.h"


using namespace std;


bool g_flagFullScreen = false;


void OutputHelp(void)
{
	cout << "Uzura" << endl;
	cout << "Version 1.00.00" << endl;
	cout << endl;
	cout << "Presented by team Again." << endl;
	cout << "jskny" << endl;
	cout << "NATCHI" << endl;
	cout << "HAL(KI)" << endl;
	cout << "MITANI" << endl;
	cout << endl;
	cout << "Special thank's." << endl;
	cout << "HOSINO Teacher" << endl;
	cout << "MacRat" << endl;
	cout << "ncaq" << endl;
	cout << endl;
	cout << "Thank you for all !!" << endl;
	cout << endl;
}


// コマンドラインオプションを処理。
void RunCommandLineOption(int argc, char* argv[])
{
	char* str = NULL;
	for (int i = 0; i < argc; ++i) {
		str = argv[i];
		if (!str) {
			break;
		}


		if (aen::util::IsOption(str, "help,h,?")) {
			OutputHelp();
			continue;
		}
		else if (aen::util::IsOption(str, "full")) {
			g_flagFullScreen = true;
			continue;
		}
		else if (aen::util::IsOption(str, "sound-off,off-sound")) {
			aen::gl::sound::SetFlagNonUseSound(true);
			continue;
		}

	}


	return;
}


// 読み込みテスト、
extern	void		TestBulletConfFile(const char* const id);
extern	boost::thread_group		g_threadInit;
int main(int argc, char* argv[])
{
	RunCommandLineOption(argc, argv);
	cout << "Key : " << "{14801281-5939-4E16-BE47-5094C8667A57}" << endl;


	// ゲームパッドのキーマップはデバッグだし・・・
	aen::gl::input::Joystick::loadKeyConfig("./data/keymap/jc-u3312sbk.txt");


try {
	// ロジック作成。
	g_uzura = new UzuraLogic();
	g_uzura->initialize();
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GameSystem startup." << endl;


	WINDOW_W = 800;
	WINDOW_H = 600;
	bool tmp = g_flagFullScreen;


	// 初期化スクリプト実行。
	InitScripting();
	// スタートアップスクリプトの実行。
	if (!RunScript("./data/script/startup.lua")) {
		cerr << "startup script load err." << endl;
	}


	if (tmp) {
		// コマンドラインで true にされているのならそちらを優先されたし。
		g_flagFullScreen = true;
	}


	// 実行後設定を実行。
	ScriptingAddInfoState();
	// ウインドウ生成。
	g_uzura->openWindow("Uzura", WINDOW_W, WINDOW_H, g_flagFullScreen);


	g_uzura->getWindow()->setFps(50);
	g_uzura->getWindow()->setScene(new MainScene());
//	TestBulletConfFile("test");


AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GameSystem start." << endl;
cout << "GameSystem start." << endl;


	while (g_uzura->systemLoop()) {
		// 回れ。
		Keybord::update();
//		Mouse::update();
	}


	// 読み込みスレッドを殺す。
	g_threadInit.join_all();


AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Thread stopping finish." << endl;
cout << "Thread stopping finish." << endl;


AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GameSystem finish." << endl;
cout << "GameSystem finish." << endl;
	g_uzura->closeWindow();
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GamwWindow had closed." << endl;
cout << "GameWindow had closed." << endl;
	g_uzura->finalize();
cout << "UZURA >> See you." << endl;
	AEN_SAFE_DELETE(g_uzura);
}
catch (aen::Exception exp) {
	exp.serialize(cerr);


	if (g_uzura) {
exp.serialize(AEN_STREAM_LOG_ERROR());
		g_uzura->finalize();
		AEN_SAFE_DELETE(g_uzura);
	}
	return (1);
}
catch (...) {
	if (g_uzura) {
AEN_STREAM_LOG_ERROR() << "System has been crash." << endl;
		g_uzura->finalize();
		AEN_SAFE_DELETE(g_uzura);
	}
	return (1);
}


	return (0);
}

