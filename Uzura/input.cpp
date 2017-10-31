/*
 *	EvenShooter
 *	入力系統括
 *	2012 / 05 / 01
 *	jskny
*/


#include	<aen.h>
#include	"input.h"


using namespace std;


char Keybord::g_mapNow[0xFF];
char Keybord::g_mapOld[0xFF];
char Keybord::g_mapNowInput[0xFF];


aen::gl::input::Mouse g_mouse;


// 押されていれば true
bool Keybord::isOn(int code)
{
	if (code < 0 || code > 0xFF) {
AEN_ASSERT_KILL(0, "over flow.");
	}


	return (g_mapNow[code] != 0 ? true : false);
}


bool Keybord::isOnOld(int code)
{
	if (code < 0 || code > 0xFF) {
AEN_ASSERT_KILL(0, "over flow.");
	}


	return (g_mapOld[code] != 0 ? true : false);

}


bool Keybord::isNowOn(int code)
{
	if (code < 0 || code > 0xFF) {
AEN_ASSERT_KILL(0, "over flow.");
	}


	return (g_mapNowInput[code] != 0 ? true : false);
}


// キー入力があったことにする。
void Keybord::On(int code)
{
	if (code < 0 || code > 0xFF) {
AEN_ASSERT_KILL(0, "over flow.");
	}


	Keybord::g_mapNow[code] = 1;
	Keybord::g_mapNowInput[code] = 1;
	Keybord::g_mapOld[code] = 0;
}


// キー入力がなかったことにする。
void Keybord::Off(int code)
{
	if (code < 0 || code > 0xFF) {
AEN_ASSERT_KILL(0, "over flow.");
	}


	Keybord::g_mapNow[code] = 0;
	Keybord::g_mapNowInput[code] = 0;
}


// 更新。
void Keybord::update(void)
{
	aen::gl::input::Keyboard a;
	a.getKey(Keybord::g_mapNow);
	a.getKeyOld(Keybord::g_mapOld);
	a.getKeyNowInput(Keybord::g_mapNowInput);
}


//---------------------------------------------------------
// Mouse
//---------------------------------------------------------


int Mouse::x(void)
{
	return (g_mouse.x());
}


int Mouse::y(void)
{
	return (g_mouse.y());
}


int Mouse::vx(void)
{
	return (g_mouse.vx());
}


int Mouse::vy(void)
{
	return (g_mouse.vy());
}


bool Mouse::isOn(int code)
{
	return (g_mouse.isInput(static_cast<aen::gl::input::Mouse::KeyCode>(code)));
}


bool Mouse::isNowOn(int code)
{
	return (g_mouse.isNowInput(static_cast<aen::gl::input::Mouse::KeyCode>(code)));
}


int Mouse::wheel(void)
{
	return (g_mouse.wheel());
}


void Mouse::update(void)
{
}

