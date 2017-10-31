/*
 *	Uzura
 *	シーン
 *	2012 / 09 / 12
 *	jskny
*/


#pragma once

#ifndef __UZURA_SCENE_H_INCLUDE_GUARD_889112123_61__
#define __UZURA_SCENE_H_INCLUDE_GUARD_889112123_61__


#include	<aen.h>


// Uzura シーン関連イベント処理。
class EventListenerScene :public aen::system::event::Listener
{
public:
	bool		handleEvent(aen::system::event::Event* event);


};


// Uzura シーンを処理する
class MainScene :public aen::scene::Scene
{
public:
virtual	Scene*		update(void);
virtual	void		init(void);


};


#endif // __UZURA_SCENE_H_INCLUDE_GUARD_889112123_61__

