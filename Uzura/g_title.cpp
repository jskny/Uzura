/*
 *	Uzura
 *	カラを破ってもう一度
 *	タイトル
 *	2012 / 08 / 06
 *	jskny
*/


#include	"uzura.h"
#include	"g_title.h"
#include	"g_main.h"
#include	"scripting.h"


using namespace std;


aen::scene::Scene* GTitle::update(void)
{
using namespace aen::gl;


g_uzura->getManagerProfile()->push("title");
	aen::scene::Scene* next = this;


//	DrawString(100, 100, "あとで作る。Z Key で本体へ", GetColor(0xFF, 0xFF, 0xFF));


	input::Keyboard keybord;
	if (keybord.isInput(KEY_INPUT_Z)) {
		next = new GMain();
	}


	// スクリプティング起動。
g_uzura->getManagerProfile()->push("Lua");
	SendEventCTrigger("scene-update");
g_uzura->getManagerProfile()->pop(); // lua


g_uzura->getManagerProfile()->pop();


	// プロファイル出力
	g_uzura->drawProfile();
	return (next);
}


void GTitle::init(void)
{
	SendEventCTrigger("scene-finish");
	DoScript("reload");
	DoScript("g_title");
	SendEventCTrigger("scene-init");
}

