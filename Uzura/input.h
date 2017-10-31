/*
 *	EvenShooter
 *	入力系統括
 *	2012 / 05 / 01
 *	jskny
*/


#pragma once

#ifndef __EVENSHOOTER_INPUT_INCLUDE_GUARD_470182952_20_73031_307845630193_52_1__
#define __EVENSHOOTER_INPUT_INCLUDE_GUARD_470182952_20_73031_307845630193_52_1__


class Keybord;
class Mouse;


class Keybord
{
public:
	// 押されていれば true
static	bool		isOn(int code);
static	bool		isOnOld(int code);
static	bool		isNowOn(int code);


	// キー入力があったことにする。
static	void		On(int code);
	// キー入力がなかったことにする。
static	void		Off(int code);


	// 更新。
static	void		update(void);


private:
static	char		g_mapNow[0xFF];
static	char		g_mapOld[0xFF];
static	char		g_mapNowInput[0xFF];


};


class Mouse
{
public:
	enum KeyCode {
		/// 左
		KEY_L = 0,
		/// 右
		KEY_R = 1,
		/// 真ん中
		KEY_M = 2
	};


static	int		x(void);
static	int		y(void);
static	int		vx(void);
static	int		vy(void);


static	bool		isOn(int code);
static	bool		isNowOn(int code);


static	int		wheel(void);


static	void		update(void);


};


#endif // __EVENSHOOTER_INPUT_INCLUDE_GUARD_470182952_20_73031_307845630193_52_1__

