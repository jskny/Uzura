/*
 *	Uzura
 *	シーン
 *	2012 / 09 / 12
 *	jskny
*/


#include	<sstream>
#include	<aen.h>
#include	<boost/thread.hpp>
#include	"scripting.h"


#include	"actor.h"
#include	"player.h"
#include	"enemy.h"
#include	"scene.h"
#include	"uzura.h"
#include	"bullet.h"


#include	"collision.h"
#include	"shell.h"


using namespace std;


// init を呼ぶスレッド
extern	void		ThreadCallInitEvent(void);


// 現在のシーン名
string g_sceneNow = "";
bool g_flagSceneLoaded = false;
volatile bool g_flagSceneInitFinish = false;
boost::thread_group g_threadInit;
bool g_flagPause = false;


// スクリプトの init をマルチスレッド化
static boost::mutex g_mtx;
void ThreadCallInitEvent(void)
{
	g_flagSceneInitFinish = false;


	try {
		boost::mutex::scoped_lock look(g_mtx);
		SendEventCTrigger("scene-init");
	}
	catch (LuaPlus::LuaException exp) {
		std::ostringstream oss;
		oss << "LuaEvent error : " << exp.GetErrorMessage() << endl;
		AEN_STREAM_LOG_ERROR() << oss.str();
		cerr << oss.str();
		return;
	}


	g_flagSceneInitFinish = true;
	return;
}


// EvenShooter システムイベントリスナー
bool EventListenerScene::handleEvent(aen::system::event::Event* event)
{
	AEN_NULL_TO_THROW_EXCEPTION_PARAM(event);


const	int eventChangeScene = aen::hash::AdlerStringLower("change-scene");


	// イベントの中身、
	// type		種類
	// value	数値データ ( 座標など )
	// strValue	文字列データ ( マップ名とか、 )
	if (event->getType() == eventChangeScene) {
		// すでに動いているスレッドがあれば、停止せよ。
		g_threadInit.join_all();
		g_flagSceneLoaded = false;


		// シーン変更。
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Change seen start : '" << g_sceneNow << "' to '" << event->getStrValue() << "'." << endl;
		try {
			SendEventCTrigger("scene-finish");


			DoScript("reload");
			if (!DoScript(event->getStrValue().c_str())) {
AEN_STREAM_LOG_ERROR() << "Can not Change seen : '" << g_sceneNow << "' to '" << event->getStrValue() << "'." << endl;
				return (false);
			}


			// SendEventCTrigger("scene-init");
			// スレッド実行開始。
			g_threadInit.create_thread(&ThreadCallInitEvent);
		}
		catch (LuaPlus::LuaException exp) {
			std::ostringstream oss;
			oss << "LuaEvent error : " << exp.GetErrorMessage() << endl;
			AEN_STREAM_LOG_ERROR() << oss.str();
			cerr << oss.str();
			return (false);
		}
		catch (aen::Exception exp) {
			std::ostringstream oss;
			// シリアライズ。
			exp.serialize(oss);
			AEN_STREAM_LOG_ERROR() << oss.str();
			cerr << oss.str();
			return (false);
		}


AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Change seen finish : '" << g_sceneNow << "' to '" << event->getStrValue() << "'." << endl;
cout << "Change seen : '" << g_sceneNow << "' to '" << event->getStrValue() << "'." << endl;


		g_sceneNow = event->getStrValue();
		g_flagSceneLoaded = true;
		return (true);
	}


	return (false);
}


// イベントを送信する。
int Shell_SendEvent(int n, const vector<string>& args)
{
// cout << n << "," << args.size() << endl;
	if (n+1 >= args.size()) {
ShellAddLine("SendEvent >> parameter の数が不正です。");
		return (-1);
	}


	aen::system::event::Event e;
	e.setType(args.at(n+1).c_str());
	int next = 1;
	if (n+2 < args.size()) {
		e.setText(args.at(n+2));
		++next;
	}


	g_uzura->getManagerEvent()->push(e);
ShellAddLine("Event push finish...");
	return (next);
}


// system 関数を呼び出す。
int Shell_System(int n, const vector<string>& args)
{
	if (n+1 >= args.size()) {
ShellAddLine("system >> parameter の数が不正です。");
		return (-1);
	}


	if (system(args.at(n+1).c_str()) == -1) {
std::ostringstream oss;
oss << "Can't found command : " << args.at(n+1) << endl;
ShellAddLine(oss.str().c_str());
	}


ShellExecute(NULL,NULL,args.at(n+1).c_str(), "",NULL,SW_SHOWNORMAL);
	return (1);
}


// 別プロセスで実行。
int Shell_System2(int n, const vector<string>& args)
{
	if (n+1 >= args.size()) {
ShellAddLine("system2 >> parameter の数が不正です。");
		return (-1);
	}


	string argstr;
	int c = 0;
	for (c = n+2; c < args.size(); ++c) {
		argstr += args.at(c) + " ";
	}


	// arg.at(1) を argstr を引数にして起動する。
	HINSTANCE ret = ShellExecute(NULL, "open", args.at(n+1).c_str(), argstr.c_str(), NULL, SW_SHOWNORMAL);
	if (ret == 0) {
		std::ostringstream oss;
		oss << "Can't found command : " << args.at(n+1) << endl;
		ShellAddLine(oss.str().c_str());
	}
/*
ERROR_FILE_NOT_FOUND	ファイルが見つからない
ERROR_PATH_NOT_FOUND	パスが見つからない
ERROR_BAD_FORMAT	無効なEXEファイル
SE_ERR_ACCESSDENIED	ファイルアクセス拒否
SE_ERR_ASSOCINCOMPLETE	ファイル名が正しくない
SE_ERR_DDEBUSY	DDE処理がビジー
SE_ERR_DDEFAIL	DDE処理失敗
SE_ERR_DDETIMEOUT	DDE処理タイムアウト
SE_ERR_DLLNOTFOUND	DLLが見つからない
SE_ERR_FNF	指定ファイルが見つからない
SE_ERR_NOASSOC	実行ファイルのタイプが正しくない
SE_ERR_OOM	メモり不足
SE_ERR_PNF	パスが見つからない
SE_ERR_SHARE	共有違反*/


	return (c);
}


// サブシステムに接続
int Shell_ConnectSubsystem(int n, const vector<string>& args)
{
	if (g_uzura->getHandleSubsystem() != -1) {
		// ハンドル開放。
ShellAddLine("ConnectSubsystem >> Subsystem has already conected... reconect now...");
		CloseNetWork(g_uzura->getHandleSubsystem());
	}


	IPDATA ip;
	ip.d1 = 127;
	ip.d2 = 0;
	ip.d3 = 0;
	ip.d4 = 1;


	int h = ConnectNetWork(ip, 18385);
	if (h == -1) {
ShellAddLine("ConnectSubsystem >> Can't conect subsystem...");
		return (-1);
	}


	g_uzura->setHandleSubsystem(h);
ShellAddLine("Connection is Succeed !!");
	return (0);
}


// サブシステムから切断。
int Shell_CloseSubsystem(int n, const vector<string>& args)
{
	if (g_uzura->getHandleSubsystem() != -1) {
		// ハンドル開放。
		CloseNetWork(g_uzura->getHandleSubsystem());
	}
	else {
ShellAddLine("CloseSubsystem > Subsystem has not conected...");
	}


	g_uzura->setHandleSubsystem(-1);
	return (0);
}


aen::scene::Scene* MainScene::update(void)
{
using namespace aen::gl;


	if (!g_flagSceneInitFinish) {
		// init 実行中。
		DrawLoadingScene();
		return (this);
	}


g_uzura->getManagerProfile()->push("scene");
	aen::scene::Scene* next = this;


	// シェルを起動。
	if (aen::gl::input::Keyboard::isNowInput(KEY_INPUT_RSHIFT)) {
		// 終了は shell で exit を打つ。
		SetFlagRunningShell(true);
	}
	// シェル
	if (IsRunningShell()) {
		ShellDraw();
		ShellInput();
		return (next);
	}


	if (//	input::Joystick::isInputNow(input::Joystick::PAD_1, input::Joystick::START) ||
		input::Joystick::isInputNow(input::Joystick::PAD_1, input::Joystick::SELECT)) {
		g_flagPause = !g_flagPause;
	}


	// 読み込み完了。
	if (g_flagSceneLoaded) {

	// スクリプティング起動。
g_uzura->getManagerProfile()->push("scripting");
if (!g_flagPause) {
	// 計算
	g_uzura->getManagerProfile()->push("update");
		SendEventCTrigger("scene-update");
	g_uzura->getManagerProfile()->pop(); // ("scene-update");
} // g_flagPause


	// 出力
	g_uzura->getManagerProfile()->push("draw");
		SendEventCTrigger("scene-draw");
	g_uzura->getManagerProfile()->pop(); // ("scene-draw");
g_uzura->getManagerProfile()->pop();//("scripting");

	}
/*	else {
DrawString(50, 50, "Warning ! scene script has not loaded.", GetColor(0xFF, 0xFF, 0x00));
	}*/


	if (g_flagPause) {
Blend::push();
Blend::set(Blend::BT_ADD, 0xFF/2);
	DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0x50), TRUE);
Blend::pop();
	}


if (!g_flagPause) {
g_uzura->getManagerProfile()->push("collision");
g_uzura->getManagerProfile()->push("player bullet @ enemy");
	// リストコリジョン
	if (CollisionListByListAndLifeDec(g_idListEnemy, g_idListPlayerBullet)) {
		// ヒット
// SendEventC("ENEMY-DAMAGE");
	}
g_uzura->getManagerProfile()->pop(); // ("player bullet @ enemy");


// 弾 vs 弾 !
g_uzura->getManagerProfile()->push("player bullet @ enemy bullet");
	// リストコリジョン
	if (CollisionListByListAndLifeDec(g_idListBulletEnemy, g_idListPlayerBullet)) {
		// ヒット
// SendEventC("ENEMY-DAMAGE");
	}
g_uzura->getManagerProfile()->pop(); // ("player bullet @ enemy");


g_uzura->getManagerProfile()->push("enemy @ player");
	// リストコリジョン
/*	if (CollisionListById(g_idListEnemy, HASH_KEY_PLAYER)) {
		// ヒット
SendEventC("PLAYER-DAMAGE");
	}*/
g_uzura->getManagerProfile()->pop(); // ("enemy bullet @ player");


g_uzura->getManagerProfile()->push("Enemy bullet @ player");
	// リストコリジョン
	if (CollisionListById(g_idListBulletEnemy, HASH_KEY_PLAYER, true)) {
		// ヒット
SendEventC("PLAYER-DAMAGE");
	}
g_uzura->getManagerProfile()->pop(); // ("Enemy bullet @ player");
g_uzura->getManagerProfile()->pop(); // collision
} // g_flagPause

g_uzura->getManagerProfile()->pop(); // ("scene");


	// プロファイル結果出力
	if (g_uzura->isShowProfile()) {
		g_uzura->drawProfile();
	}


	if (g_uzura->isDebug()) {
		DrawFormatString(0, 500, GetColor(0xFF, 0xFF, 0xFF), "TASKS (UPDATE) : %d", g_managerProcessUpdate.size());
		DrawFormatString(0, 516, GetColor(0xFF, 0xFF, 0xFF), "TASKS ( DRAW ) : %d", g_managerProcessDraw.size());
	}


/*using namespace aen::gl::input;
	if (Joystick::isNowInput(Joystick::PAD_1, Joystick::A)) {
		Joystick::startVibration(Joystick::PAD_1, 1000, 1000);
	}
	if (Joystick::isNowInput(Joystick::PAD_1, Joystick::B)) {
		Joystick::stopVibration(Joystick::PAD_1);
	}
*/


// aen::gl::UpdateCamera(VGet(0, 0, 0));
// aen::SerializeVector(cout, aen::gl::GetCameraPos());
	return (next);
}


void MainScene::init(void)
{
	// シーン関連イベントの受信。
	auto listener = boost::shared_ptr<aen::system::event::Listener>(new EventListenerScene());
	g_uzura->getManagerEvent()->addListener(listener, aen::hash::AdlerStringLower("change-scene"));


	// アクター初期化
	InitActors();


	ShellReset();
	SetFlagRunningShell(false);
	ShellAttachFunction("sendevent", &Shell_SendEvent);
	ShellAttachFunction("system", &Shell_System);
	ShellAttachFunction("system2", &Shell_System2);
	ShellAttachFunction("connect-subsystem", &Shell_ConnectSubsystem);
	ShellAttachFunction("close-subsystem", &Shell_CloseSubsystem);

//	g_flagRunShell = false;
	g_flagPause = false;
}

