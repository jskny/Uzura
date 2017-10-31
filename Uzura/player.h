/*
 *	Uzura
 *	Player
 *	2012 / 09 / 19
 *	jskny
*/


#pragma once

#ifndef __UZURA_PLAYER_H_INCLUDE_GUARD_7_367819821_213619830033__
#define __UZURA_PLAYER_H_INCLUDE_GUARD_7_367819821_213619830033__


#include	<aen.h>


class ProcessPlayerMove;
class ProcessPlayerDraw;


extern	bool		InitPlayer(void);


// 自キャラのポジションを取得
extern	VECTOR		GetPlayerPos(void);
// プレーヤーの見ている方向へ向かうベクトルを取得。
extern	VECTOR		GetPlayerRay(void);
// プレーヤーの見ている方向へ向かう行列を取得。
extern	MATRIX		GetPlayerRayMatrix(void);


class ProcessPlayerUpdate :public aen::system::task::Process
{
public:
virtual	void		update(void);
virtual	void		init(void);


};


class ProcessPlayerDraw :public aen::system::task::Process
{
public:
virtual	void		update(void);
virtual	void		init(void);


};


#endif // __UZURA_PLAYER_H_INCLUDE_GUARD_7_367819821_213619830033__

