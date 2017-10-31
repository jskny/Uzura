/*
 *	Uzura
 *	Actor
 *	2012 / 09 / 13
 *	jskny
*/


#pragma once

#ifndef __UZURA_ACTOR_H_INCLUDE_GUARD_90_2_3571_03_1__
#define __UZURA_ACTOR_H_INCLUDE_GUARD_90_2_3571_03_1__


#include	<aen.h>
#include	<aen_import_lua.h>


#include	<istream>
#include	<sstream>
#include	<hash_map>


#include	"GScripter/gscripter.h"


extern	const	int		HASH_KEY_PLAYER;
extern	const	int		ACTOR_TYPE_PLAYER;


class ActorParam;


// アクター一覧配置。
extern	std::hash_map<unsigned int, boost::shared_ptr<ActorParam>>	g_mapActors;


// アクターId 生成用の文字列を返す。
extern	std::string		GetActorIdStr(void);


class ActorParam
{
public:
	// 初期化処理等
virtual	bool		init(void);
	// シリアライズ
virtual	void		serialize(std::ostream& out) const;


	// ActorParam を GScripter から生成。
static	gscripter::Scripter	CreateFromScript(const std::string& urlFile, ActorParam* pParam);


virtual	void		reset(void)
	{
		this->m_collision.reset();
		this->m_id = -1;
		this->m_type = -1;

		this->m_matrix = MGetIdent();
		this->m_speed = 0.0f;
		this->m_hp = -1;
	}


	void		setPos(const VECTOR& a) { this->m_collision.setPos(a); };
	void		setId(const int id) { this->m_id = id; };
	void		setType(const int type) { this->m_type = type; };


	VECTOR		getPos(void) const { return (this->m_collision.getPos()); };
	int		getId(void) const { return (this->m_id); };
	int		getType(void) const { return (this->m_type); };


DxLib::MATRIX		getMatrix(void) const { return (this->m_matrix); };
	void		setMatrix(const DxLib::MATRIX& m) { this->m_matrix = m; };


	// スピード
	float		getSpeed(void) const { return (this->m_speed); };
	void		setSpeed(float speed) { this->m_speed = speed; };


	void		setCboxI(const VECTOR& t)
	{
		this->m_collision.ix = t.x;
		this->m_collision.iy = t.y;
		this->m_collision.iz = t.z;
	}

	void		setCboxB(const VECTOR& t)
	{
		this->m_collision.bx = t.x;
		this->m_collision.by = t.y;
		this->m_collision.bz = t.z;
	}


	// コリジョン座標を操作。
	void		updateCollision(const VECTOR& p) { this->m_collision.setPos(p); };
	VECTOR		getCollisionPos(void) const { return (this->m_collision.getPos()); };


	void		setCbox(const aen::CollisionBox3d& tmp) { this->m_collision = tmp; }
aen::CollisionBox3d	getCbox(void) const { return (this->m_collision); };


	void		setHp(int p) { this->m_hp = p; };
	int		getHp(void) const { return (this->m_hp); };


	// マトリックスをベクトルへ変換
	void		updateMatrix2Pos(void);


	ActorParam() :
		m_collision(),
		m_id(0), m_type(0), m_speed(0.0f), m_hp(0)
	{
		this->m_matrix = MGetIdent();
	}


protected:
	DxLib::MATRIX	m_matrix;
aen::CollisionBox3d	m_collision;
	int		m_id;
	int		m_type;
	int		m_hp;
	float		m_speed;


};


// タスクシステム。マネージャー
extern	aen::system::task::Manager	g_managerProcessUpdate;
extern	aen::system::task::Manager	g_managerProcessDraw;


extern	bool		InitActors(void);
extern	bool		ActorDel(unsigned int id);
	// 登録されている全てのアクターを破棄。
extern	void		ActorReset(void);


inline bool ActorParam::init(void)
{
	this->m_collision.reset();
	this->m_id = 0;
	this->m_type = 0;
	this->m_matrix = MGetIdent();
	return (true);
}


inline void ActorParam::serialize(std::ostream& out) const
{
	out << "MATRIX" << std::endl;
	aen::SerializeMatrix(out, this->m_matrix);
	out << std::endl;
	out << "POS  :" << this->m_collision.getPos().x << "," << this->m_collision.getPos().y << "," << this->m_collision.getPos().z << std::endl;
	out << "ID   :" << this->m_id << std::endl;
	out << "TYPE :" << this->m_type << std::endl;
	out << "SPEED :" << this->m_speed << std::endl;
	out << "HP    : " << this->m_hp << std::endl;

	out << "collision (IBox) : ";
	out << this->m_collision.ix << "," << this->m_collision.iy << "," << this->m_collision.iz << std::endl;
	out << "collision (BBox) : ";
	out << this->m_collision.bx << "," << this->m_collision.by << "," << this->m_collision.bz << std::endl;
}


#endif // __UZURA_ACTOR_H_INCLUDE_GUARD_90_2_3571_03_1__

