/*
 *	Uzura
 *	Enemy
 *	2012 / 10 / 18
 *	jskny
*/


/*
 *	とりあえず C で動くものを作ろう。
 *	2012 / 10 / 23
*/


#pragma once

#ifndef __UZURA_ENEMY_H_INCLUDE_GUARD_38912_1467201_2_1__
#define __UZURA_ENEMY_H_INCLUDE_GUARD_38912_1467201_2_1__


#include	<aen.h>
#include	"actor.h"


class EnemyParam;
class EnemyPattern;


// 敵キャラの id 一覧。
extern	std::list<unsigned int>	g_idListEnemy;


extern	bool		InitEnemys(void);
// 敵を生成
// 敵のパラメーター, モデル, パターンデータ, 生成の座標。
extern	unsigned int	AddEnemy(const char* const id, const char* const modelId, const boost::shared_ptr<EnemyPattern>& pattern, const VECTOR& cPos, const VECTOR& param);
extern	unsigned int	AddEnemy(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param);
// ツイート作成の敵を作成。
extern	unsigned int	AddTweet(const char* const id, const VECTOR& cPos, const aen::gl::Picture& texture);


// ベースとなる動作。
extern	VECTOR		MatrixToPos(const MATRIX& m);


class EnemyParam;
class EnemyPattern;
class ProcessEnemy;
class ProcessEnemyUpdate;
class ProcessEnemyDraw;


class EnemyParam :public ActorParam
{
public:
virtual	bool		init(void)
	{
		if (!ActorParam::init()) {
			return (false);
		}


		this->m_flagScripting = false;
		this->m_pattern.reset();
		this->m_timer = 0;
		this->m_flagAddRotatePlayer = false;
		this->m_hp = 0;
		this->m_score = 0;
		this->setType(TYPE_ENEMY);

		return (true);
	}


	// 設定ファイルからパラメーターを生成。
static	EnemyParam	CreateFromScript(const char* const id);


static	const int	TYPE_ENEMY = 339827;
	EnemyParam() :
		ActorParam(),
		m_flagScripting(false),
		m_timer(0), m_flagAddRotatePlayer(false),
		m_hp(0), m_score(0)
	{
		this->m_pattern.reset();
		this->setType(TYPE_ENEMY);
	}


	void		setPattern(const boost::shared_ptr<EnemyPattern>& p) { this->m_pattern = p; };
boost::shared_ptr<EnemyPattern>	getPattern(void) { return (this->m_pattern); };


	void		setTimer(int t) { this->m_timer = t; };
	int		getTimer(void) const { return (this->m_timer); };


	void		setFlagmAddRotatePlayer(bool f) { this->m_flagAddRotatePlayer = f; };
	bool		getFlagmAddRotatePlayer(void) const { return (this->m_flagAddRotatePlayer); };


//boost::shared_ptr<PSL::variable>	getScripting(void) const { return (this->m_function); };
//	void		setScripting(const boost::shared_ptr<PSL::variable>& c) { this->m_function = c; };


	void		setScore(int p) { this->m_score = p; };
	int		getScore(void) const { return (this->m_score); };


virtual	void		serialize(std::ostream& out)
	{
		out << "EnemyParam" << std::endl;
		ActorParam::serialize(out);
		out << "SCORE : " << this->m_score << std::endl;
		out << "TIMER : " << this->m_timer << std::endl;
	}


protected:
	bool		m_flagScripting;
boost::shared_ptr<EnemyPattern>	m_pattern;


	int		m_timer;
	int		m_hp, m_score;


	// プレイヤーの姿勢行列をモデルに加算したか？
	bool		m_flagAddRotatePlayer;


// boost::shared_ptr<PSL::variable>	m_function;


};


class ProcessEnemy :public aen::system::task::Process
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


	void		setTexture(const aen::gl::Picture& t) { this->m_texture = t; };
aen::gl::Picture	getTexture(void) const { return (this->m_texture); };


	void		setId(unsigned int id) { this->m_actorId = id; };
	int		getId(void) const { return (this->m_actorId); };


	// 描画処理
virtual	void		draw(void);
	// 座標移動処理。
virtual	void		move(void);


	ProcessEnemy() :
		aen::system::task::Process(), m_actorId(0)
	{
	}


protected:
	unsigned int	m_actorId;
	aen::gl::Model	m_model;
aen::gl::Picture	m_texture;


};


class ProcessEnemyUpdate :public ProcessEnemy
{
public:
virtual	void		update(void);
virtual	void		init(void);


};


class ProcessEnemyDraw :public ProcessEnemy
{
public:
virtual	void		update(void);
virtual	void		init(void);


};


//---------------------------------------------------------
// パターン
//---------------------------------------------------------


class EnemyPattern
{
public:
virtual	EnemyParam		update(EnemyParam a)
	{
		VECTOR vec = aen::MatrixToPos(a.getMatrix());
// aen::SerializeMatrix(std::cout, a.getMatrix());
// aen::SerializeVector(std::cout, vec);
		a.setPos(vec);
		return (a);
	}


};


// 何もしない。
class EnemyPatternNone :public EnemyPattern
{
public:
virtual	EnemyParam	update(EnemyParam a)
	{
		a = EnemyPattern::update(a);
		return (a);
	}


};


// 自キャラに向かって進み、60フレームに一度、弾を放つ。
class EnemyPatternHomingPlayerAndShotBullet :public EnemyPattern
{
public:
virtual	EnemyParam	update(EnemyParam a);


	EnemyPatternHomingPlayerAndShotBullet() :
		m_counter(0),
		m_addX(0.0f), m_addY(0.0f), m_addZ(0.0f)
	{
	}


protected:
	unsigned int	m_counter;
	float		m_addX, m_addY, m_addZ;


};


#endif // __UZURA_ENEMY_H_INCLUDE_GUARD_38912_1467201_2_1__

