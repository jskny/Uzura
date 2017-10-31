/*
 *	Uzura
 *	エフェクト
 *	2012 / 10 / 30
 *	jskny
*/


#pragma once

#ifndef __UZURA_EFFECT_INCLUDE_GUARD_369192_2_212__
#define __UZURA_EFFECT_INCLUDE_GUARD_369192_2_212__


#include	<aen.h>
#include	"actor.h"


extern	std::list<unsigned int> g_idListEffect;


extern	bool		InitEffect(void);
// エフェクト生成
// テクスチャみたいなオブジェクト
// efectId, motionId(画像), 座標, param
extern	unsigned int	AddEffect(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param);
// efectId, soundId
extern	void		PlaySE(const char* const soundId);


class EffectParam;
class ProcessEffect;
class ProcessEffectUpdate;
class ProcessEffectDraw;


// あまゆり関連
//extern	bool		InitAmayuri(void);
//extern	bool		FinalizeAmayuri(void);
//extern	void		UpdateAmayuri(void);


class EffectParam :public ActorParam
{
public:
virtual	bool		init(void)
	{
		if (!ActorParam::init()) {
			return (false);
		}


		this->m_timer = 0;
		return (true);
	}


	// 設定ファイルからパラメーターを生成。
static	EffectParam	CreateFromScript(const char* const id);


	EffectParam() :
		ActorParam(), m_timer(0), m_type(0),
		m_counter(0), m_speed(0),
		m_size(0)
	{
	}


	void		setTimer(int t) { this->m_timer = t; };
	int		getTimer(void) const { return (this->m_timer); };


	void		setType(int i) { this->m_type = i; };
	int		getType(void) const { return (this->m_type); };


	int		getCounter(void) const { return (this->m_counter); };
	void		setCounter(int i) { this->m_counter = i; };
	// アニメーションループスピード
	void		setSpeed(int s) { this->m_speed = s; };
	int		getSpeed(void) const { return (this->m_speed); };

	void		setSize(int s) { this->m_size = s; };
	int		getSize(void) const { return (this->m_size); };


protected:
	int		m_timer;
	int		m_type;
	// テクスチャムービー
	int		m_counter, m_speed;
	int		m_size;


};


class ProcessEffect :public aen::system::task::Process
{
public:
	enum {
		MODE_TEXTURE = 1,
		MODE_SOUND
	};


virtual	void		update(void) = 0;
virtual	void		init(void) = 0;


	// アクターを消すため少々改造。
virtual	void		kill(void);


virtual	bool		loadTexture(const char* const id);
virtual	bool		loadSound(const char* const id);
virtual	void		reset(void)
	{
		this->m_actorId = 0;
		this->m_texture.clear();
		this->m_player.clear();
	}


	void		setId(unsigned int id) { this->m_actorId = id; };
	int		getId(void) const { return (this->m_actorId); };


	// 描画処理
virtual	void		draw(void);
	// 座標移動処理。
virtual	void		move(void);


	void		setTexture(const aen::gl::Picture& picture) { this->m_texture = picture; };
aen::gl::Picture	getTexture(void) const { return (this->m_texture); };


aen::gl::sound::Player	getPlayer(void) const { return (this->m_player); };
	void		setPlayer(const aen::gl::sound::Player& p) { this->m_player = p; };


	ProcessEffect() :
		aen::system::task::Process(), m_actorId(0)
	{
	}


protected:
	unsigned int	m_actorId;
aen::gl::Picture	m_texture;
aen::gl::sound::Player	m_player;


};


class ProcessEffectUpdate :public ProcessEffect
{
public:
virtual	void		update(void);
virtual	void		init(void);


};


class ProcessEffectDraw :public ProcessEffect
{
public:
virtual	void		update(void);
virtual	void		init(void);


};


#endif // __UZURA_EFFECT_INCLUDE_GUARD_369192_2_212__

