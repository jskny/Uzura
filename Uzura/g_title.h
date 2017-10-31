/*
 *	Uzura
 *	カラを破ってもう一度
 *	タイトル
 *	2012 / 08 / 06
 *	jskny
*/


#pragma once

#ifndef __UZURA_G_TITLE_INCLUDE_GUARD_9283753_08131_276291_489932_2__
#define __UZURA_G_TITLE_INCLUDE_GUARD_9283753_08131_276291_489932_2__


#include	<aen.h>


class GTitle :public aen::scene::Scene
{
public:
virtual	Scene*		update(void);
virtual	void		init(void);



	GTitle() :
		m_count(0), m_countC(0),
		m_angle(0.0f), m_angle2(0.0f),
		m_prevT(0), m_addR(0.0f)
	{
	}


private:
	int		m_count, m_countC;
	float		m_angle, m_angle2;
	int		m_prevT;
	float		m_addR;


};


#endif // __UZURA_G_TITLE_INCLUDE_GUARD_9283753_08131_276291_489932_2__

