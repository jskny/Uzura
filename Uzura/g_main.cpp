/*
 *	Uzura
 *	カラを破ってもう一度
 *	ゲーム本体
 *	2012 / 08 / 15
 *	jskny
*/


#include	"uzura.h"
#include	"g_main.h"
#include	"g_title.h"
#include	"scripting.h"


using namespace std;


aen::scene::Scene* GMain::update(void)
{
using namespace aen::gl;


g_uzura->getManagerProfile()->push("main");
	aen::scene::Scene* next = this;

/*
g_uzura->getManagerProfile()->push("draw back");
	int color = 255 - 20 + GetScale(20);
	color = GetColor(color, color, color);
	DrawBox(0, 0, WINDOW_W, WINDOW_H, color, TRUE);


	DrawBackEn(g_uzura->getBpm(), 10, GetColor(0xFF, 0x00, 0x00), GetColor(0xFF, 0x20, 0x00));
Blend::push();
Blend::set(Blend::BT_SUB, 0xFF);
	DrawBackLine();
Blend::pop();
	DrawBackBox(0xFF, 0x00, 0xFF);
g_uzura->getManagerProfile()->pop();//("draw back");
*/

	// スクリプティング起動。
g_uzura->getManagerProfile()->push("scripting");
	SendEventCTrigger("scene-update");
g_uzura->getManagerProfile()->pop();//("scripting");


g_uzura->getManagerProfile()->pop(); // main
	// プロファイル出力
	g_uzura->drawProfile();


	input::Keyboard keybord;
	if (keybord.isInput(KEY_INPUT_X)) {
		next = new GTitle();
	}
	return (next);
}


void GMain::init(void)
{
	SendEventCTrigger("scene-finish");
	DoScript("reload");
	DoScript("g_main");
	SendEventCTrigger("scene-init");
}

