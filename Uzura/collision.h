/*
 *	Uzura
 *	当たり判定
 *	2012 / 10 / 25
 *	jskny
*/


#pragma once

#ifndef __UZURA_COLLISION_INCLUDE_GUARD_B844B5B1_7F9A_4E9C_8931_B4CC24F5CF96__
#define __UZURA_COLLISION_INCLUDE_GUARD_B844B5B1_7F9A_4E9C_8931_B4CC24F5CF96__


#include	<list>


// リストと指定した id で当たり判定。
// 当たった回数を返す。
extern	int		CollisionListById(std::list<unsigned int>& ids, unsigned int const key, bool flagDecHp = false, int maxCount = 1);
// リスト同士をぶつけて、HP を減らす。
extern	int		CollisionListByListAndLifeDec(std::list<unsigned int>& ids, std::list<unsigned int>& ids2);


#endif // __UZURA_COLLISION_INCLUDE_GUARD_B844B5B1_7F9A_4E9C_8931_B4CC24F5CF96__

