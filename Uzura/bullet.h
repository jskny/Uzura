/*
 *	Uzura
 *	Bullet
 *	2012 / 10 / 04
 *	jskny
*/


#pragma once

#ifndef __UZURA_BULLET_H_INCLUDE_GUARD_778294617392042_123_0_7185331__
#define __UZURA_BULLET_H_INCLUDE_GUARD_778294617392042_123_0_7185331__


#include	<aen.h>
#include	"actor.h"


namespace PSL { class variable; }


class BulletParam;
class BulletPattern;
class ProcessPlayerBulletUpdate;
class ProcessPlayerBulletDraw;


extern	std::list<unsigned int> g_idListPlayerBullet;
extern	std::list<unsigned int> g_idListBulletEnemy;


extern	bool		InitBullet(void);
// 弾を生成
extern	unsigned int	AddBullet(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param, std::list<unsigned int>* list);
// たまのパラメーター, モデル, パターンデータ, 発射地点の座標。
extern	unsigned int	AddPlayerBullet(const char* const id, const char* const modelId, const boost::shared_ptr<BulletPattern>& pattern, const VECTOR& cPos, const VECTOR& param);
extern	unsigned int	AddPlayerBullet(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param);
// 敵、弾を打つ。
// たまのパラメーター, モデル, パターンデータ, 発射地点の座標。
extern	unsigned int	AddBulletEnemy(const char* const id, const char* const modelId, const boost::shared_ptr<BulletPattern>& pattern, const VECTOR& cPos, const VECTOR& param);
extern	unsigned int	AddBulletEnemy(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param);


class BulletParam :public ActorParam
{
public:
virtual	bool		init(void)
	{
		if (!ActorParam::init()) {
			return (false);
		}


		this->m_flagScripting = false;
		this->m_patternType = TYPE_NONE;
		this->m_pattern.reset();
		this->m_timer = 0;
		this->m_flagAddRotatePlayer = false;

		return (true);
	}


	// PSL 設定ファイルからパラメーターを生成。
static	BulletParam	CreateFromPsl(const char* const id);
static	const int	TYPE_NONE;
static	const int	TYPE_C;
static	const int	TYPE_SCRIPT;


	BulletParam() :
		ActorParam(),
		m_flagScripting(false),
		m_patternType(TYPE_NONE), m_timer(0), m_flagAddRotatePlayer(false)
	{
		this->m_pattern.reset();
	}


	void		setPattern(const boost::shared_ptr<BulletPattern>& p) { this->m_pattern = p; };
boost::shared_ptr<BulletPattern>	getPattern(void) { return (this->m_pattern); };


	void		setPatternType(const unsigned int t) { this->m_patternType = t; };
	unsigned int	getPatternType(void) const { return (this->m_patternType); };


	void		setTimer(int t) { this->m_timer = t; };
	int		getTimer(void) const { return (this->m_timer); };


	void		setFlagmAddRotatePlayer(bool f) { this->m_flagAddRotatePlayer = f; };
	bool		getFlagmAddRotatePlayer(void) const { return (this->m_flagAddRotatePlayer); };


boost::shared_ptr<PSL::variable>	getScripting(void) const { return (this->m_function); };
	void		setScripting(const boost::shared_ptr<PSL::variable>& c) { this->m_function = c; };


protected:
	bool		m_flagScripting;
	int		m_patternType;
boost::shared_ptr<BulletPattern>	m_pattern;


	int		m_timer;


	// プレイヤーの姿勢行列をモデルに加算したか？
	bool		m_flagAddRotatePlayer;


boost::shared_ptr<PSL::variable>	m_function;


};


// 弾道。
class BulletPattern
{
public:
	// 弾の動作。
static	const unsigned int	MOVE_NONE, MOVE_NORMAL, MOVE_HOMING;


	BulletPattern()
	{
	}


virtual	~BulletPattern()
	{
	}


virtual	BulletParam	update(BulletParam a)
	{
		VECTOR vec = aen::MatrixToPos(a.getMatrix());
		a.setPos(vec);
		return (a);
	}


};


// 指定したベクトルを加算し続ける。
class BulletPatternAddVec :public BulletPattern
{
public:
	BulletPatternAddVec(const VECTOR& v) :
		BulletPattern()
	{
		this->m_addM = MGetTranslate(v);
	}


virtual	~BulletPatternAddVec()
	{
	}


virtual	BulletParam	update(BulletParam a)
	{
		a.setMatrix(MMult(a.getMatrix(), this->m_addM));


		a = BulletPattern::update(a);
		return (a);
	}


protected:
	MATRIX		m_addM;


};


// スクリプティングで弾を処理せよ。
class BulletPatternScripting :public BulletPattern
{
public:
	BulletPatternScripting(int const id) :
		BulletPattern(),
		m_actorId(id)
	{
	}


virtual	~BulletPatternScripting()
	{
	}


virtual	VECTOR		update(VECTOR pos);


protected:
	int		m_actorId;


};


// プレイヤーを追跡し続ける。
class BulletPatternHomingPlayer :public BulletPattern
{
public:
	// 何フレーム追尾し続けるか、
	// 何フレームごとに追跡情報を更新するか、
	// 最大でどの程度の変化を許容するか、
	BulletPatternHomingPlayer(int count, int updateTimer, float speed, float capacity) :
		m_t(0),
		m_counter(count), m_updateTimer(updateTimer), m_speed(speed), m_capacity(capacity),
		m_addX(0.0f), m_addY(0.0f), m_addZ(0.0f)
	{
	}


virtual	~BulletPatternHomingPlayer()
	{
	}


virtual	BulletParam	update(BulletParam a);
protected:
	unsigned int	m_t;
	unsigned int	m_counter;
	unsigned int	m_updateTimer;
	float		m_capacity;

	float		m_speed;
	float		m_addX, m_addY, m_addZ;


};


class ProcessBullet :public aen::system::task::Process
{
public:
virtual	void		update(void) = 0;
virtual	void		init(void) = 0;


	// アクターを消すため少々改造。
virtual	void		kill(void);


	// たまデータを読み込む。
virtual	bool		load(const char* const id);
virtual	void		reset(void)
	{
		this->m_actorId = 0;
		this->m_model.clear();
	}


	void		setModel(const aen::gl::Model& model) { this->m_model = model; };
	aen::gl::Model	getModel(void) const { return (this->m_model); };


	void		setId(unsigned int id) { this->m_actorId = id; };
	int		getId(void) const { return (this->m_actorId); };


	// 描画処理
virtual	void		draw(void);
	// 座標移動処理。
virtual	void		move(void);


	ProcessBullet() :
		aen::system::task::Process(), m_actorId(0)
	{
	}


protected:
	unsigned int	m_actorId;
	aen::gl::Model	m_model;


};


class ProcessBulletUpdate :public ProcessBullet
{
public:
virtual	void		update(void);
virtual	void		init(void);


	ProcessBulletUpdate() :
		ProcessBullet()
	{
	}


protected:


};


class ProcessBulletDraw :public ProcessBullet
{
public:
virtual	void		update(void);
virtual	void		init(void);


	ProcessBulletDraw() :
		ProcessBullet()
	{
	}


protected:


};


#endif // __UZURA_BULLET_H_INCLUDE_GUARD_778294617392042_123_0_7185331__

