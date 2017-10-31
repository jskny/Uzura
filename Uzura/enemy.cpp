/*
 *	Uzura
 *	Enemy
 *	2012 / 10 / 18
 *	jskny
*/


#include	<sstream>
#include	<boost/shared_ptr.hpp>
#include	<aen.h>
#include	"uzura.h"
#include	"enemy.h"
#include	"actor.h"
#include	"player.h"
#include	"bullet.h"
#include	"effect.h"

#include	"scripting.h"


using namespace std;


// 敵キャラの id 一覧。
std::list<unsigned int>	g_idListEnemy;


bool InitEnemys(void)
{
	g_managerProcessDraw.clear();
	g_managerProcessUpdate.clear();


	auto itr = g_idListEnemy.begin();
	while (itr != g_idListEnemy.end()) {
		unsigned int key = *itr;
		auto tmp = g_mapActors.find(*itr);


		if (tmp == g_mapActors.end()) {
			itr = g_idListEnemy.erase(itr);
			continue;
		}


		// 要素を消す。
		g_mapActors.erase(*itr);
		++itr;
	}


	g_idListEnemy.clear();
	return (true);
}


// アクターを消すため少々改造。
void ProcessEnemy::kill(void)
{
	if (this->m_actorId != 0) {
		ActorDel(this->m_actorId);

		for (auto itr = g_idListEnemy.begin(); itr != g_idListEnemy.end(); ++itr) {
			if (*itr == this->m_actorId) {
				g_idListEnemy.erase(itr);
				break;
			}
		}


cout << "Enemy killd. [" << this->m_actorId << "]" << endl;
	}


	Process::kill();
}


aen::gl::Model GetModelEnemy(const char* const id)
{
	std::ostringstream oss;
	oss << "./data/models/enemys/" << id << "/data.mqo";
	return (GetModel(oss.str()));
}


// 設定ファイルからパラメーターを生成。
EnemyParam EnemyParam::CreateFromScript(const char* const id)
{
static	hash_map<std::string, EnemyParam> g_tmpMap;
	EnemyParam ret;
	gscripter::Scripter scripter;


	std::ostringstream oss;
	oss.str("");
	oss << "./data/conf/enemys/" << id << "/conf.txt";
	auto itr = g_tmpMap.find(oss.str());
	if (itr != g_tmpMap.end()) {
		return (itr->second);
	}


	// 基底情報の読み込み。
	scripter = ActorParam::CreateFromScript(oss.str(), &ret);


	ret.setScore(scripter.getValue("score"));
	ret.setTimer(scripter.getValue("timer"));
	ret.setType(ret.TYPE_ENEMY);
cout << "Enemy Serialize:" << endl;
ret.serialize(cout);
	g_tmpMap[oss.str()] = ret;
	return (ret);
}


//---------------------------------------------------------
// プロセス
//---------------------------------------------------------


// たまデータを読み込む。
bool ProcessEnemy::load(const char* const id)
{
	if (!id) {
AEN_STREAM_LOG_ERROR() << "null pointer." << endl;
return (false);
	}


	std::ostringstream oss;


	this->m_model = GetModelEnemy(id);
	if (!this->m_model.isLoad()) {
AEN_WARN(g_uzura, "can not load model.");
		return (false);
	}


	return (true);
}


// 描画処理
void ProcessEnemy::draw(void)
{
	if (g_mapActors.find(this->m_actorId) == g_mapActors.end()) {
		// 死んでいるもの。
		this->kill();
		return;
	}


	auto a = boost::dynamic_pointer_cast<EnemyParam>(g_mapActors[this->m_actorId]);
	// モデルの姿勢行列を加算。
	if (!a->getFlagmAddRotatePlayer()) {
		VECTOR v;
		MATRIX m = a->getMatrix();


		// 平行移動を消してかける。後、平行移動を書き戻す。
		v.x = m.m[3][0];
		v.y = m.m[3][1];
		v.z = m.m[3][2];
		m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
		m = MMult(m, GetPlayerRayMatrix());
// cout << "M" << endl;
// aen::SerializeMatrix(cout, m);
		// 平行移動を戻す。
		m = MMult(m, MGetTranslate(v));
// cout << "M2" << endl;
// aen::SerializeMatrix(cout, m);


		a->setMatrix(m);
		a->setFlagmAddRotatePlayer(true);
	}


	// コリジョン更新関連
	a->updateMatrix2Pos();
	a->updateCollision(a->getPos());


	// コリジョン領域を出力
	if (g_uzura->isDebug()) {
		auto d = a->getCbox();
//	 	d.setPos(a->getPos());
		d.draw();
	}


	if (this->getModel().isLoad()) {
		MV1SetMatrix(this->m_model.getHandle(), a->getMatrix());
		// 出力
		MV1DrawModel(this->m_model.getHandle());
	}
	else {
		if (this->getTexture().getSize() != 0) {
			// テクスチャ表示 tweet だから決め打ち。
			DrawBillboard3D(a->getPos(), 0.5f, 0.5f, 600, 0.0f, this->getTexture().getHandle(0), TRUE);
// a->serialize(cout);
		}
	}


	g_mapActors[this->m_actorId] = a;


	// モデルが、スクリーン座標系のどこに出力されているのか取得する。
	VECTOR eye = ConvWorldPosToScreenPos(a->getPos());
	if (eye.z > 0.0f && eye.z < 1.0f) {
		float size = 10 * (VSize(VSub(a->getPos(), GetPlayerPos()))/1000.f);
		// 徐々に色が変わるように。
		aen::gl::DrawPolygon2d(eye.x, eye.y, 6, size*GetRad(25), size, GetColor(0xFF, 0xFF, 0x00), 1, 2);
	}

	return;
}


// 座標移動処理。
void ProcessEnemy::move(void)
{
	if (g_mapActors.find(this->m_actorId) == g_mapActors.end()) {
		// 死んでいるもの。
		this->m_actorId = NULL;
		this->kill();
		return;
	}


	auto a = boost::dynamic_pointer_cast<EnemyParam>(g_mapActors[this->m_actorId]);
	if (a->getTimer() <= 0) {
		// 破棄処理は kill 内で行われている。
		// g_mapActors.erase(this->m_actorId);
		this->kill();
		return;
	}


	// コリジョン更新関連
	a->updateMatrix2Pos();
	a->updateCollision(a->getPos());
// aen::SerializeVector(cout, a->getCbox().getPos());


// a->serialize(cout);
static	const unsigned int TYPE_NONE = GetHash("none");
static	const unsigned int TYPE_SCRIPTING = GetHash("TYPE_SCRIPTING");


	if (a->getType() == TYPE_SCRIPTING) {
		// スクリプト駆動。
//		auto p = BulletPatternScripting(a->getId());
//		a->setMatrix(MMult(a->getMatrix(), p->getMatrix()));
	}
	else {
		(*a) = a->getPattern()->update((*a));
	}


	if (a->getHp() <= 0) {
		this->kill();
		return;
	}


	a->setTimer(a->getTimer() - 1);


// cout << "A ; "; aen::SerializeVector(cout, a->getPos());
// cout << "B ; "; aen::SerializeVector(cout, ((BulletPatternAddVec*)(a->getPattern().get()))->getVec());
	g_mapActors[this->m_actorId] = a;
	return;
}


//---------------------------------------------------------
// 基底クラスの可愛いい傀儡たち。
//---------------------------------------------------------


void ProcessEnemyUpdate::init(void)
{
}


void ProcessEnemyUpdate::update(void)
{
	this->move();
	if (this->isKill()) {
		return;
	}

}


void ProcessEnemyDraw::init(void)
{
}


void ProcessEnemyDraw::update(void)
{
	this->draw();


	if (this->isKill()) {
		return;
	}


	auto c = g_mapActors[HASH_KEY_PLAYER];
	auto me = g_mapActors[this->m_actorId];
	me->updateMatrix2Pos();


	DrawLine3D(c->getPos(), me->getPos(), GetColor(0xFF, 0xFF, 0x00));
}


//---------------------------------------------------------
// 敵生成
//---------------------------------------------------------


// 敵のパラメーター, モデル, パターンデータ, 生成の座標。
unsigned int AddEnemy(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param)
{
	std::ostringstream oss;
	// たまデータの取得。
	EnemyParam tmp = EnemyParam::CreateFromScript(id);


	// ハッシュキー
	oss << "ENEMY*" << id << ":" << modelId << "@" << "[" << GetActorIdStr() << "]";
	unsigned int key = GetHash(oss.str().c_str());


{
	auto a = boost::shared_ptr<ProcessEnemyUpdate>(new ProcessEnemyUpdate());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);
	g_managerProcessUpdate.attach(a);
}


{
	auto a = boost::shared_ptr<ProcessEnemyDraw>(new ProcessEnemyDraw());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);
	a->load(modelId);
	g_managerProcessDraw.attach(a);
}


	// 生成。
	auto t = boost::shared_ptr<EnemyParam>(new EnemyParam(tmp));
	t->setId(key);
	t->setPos(cPos);
//	t->setPatternType(EnemyPattern::TYPE_NONE);


	// 弾は全て Matrix 管理。
	MATRIX m = MGetIdent();
	m = MMult(m, MGetTranslate(cPos));
	t->setMatrix(m);


	// 何もしない。
const auto g_patternNone = boost::shared_ptr<EnemyPatternHomingPlayerAndShotBullet>(new EnemyPatternHomingPlayerAndShotBullet());
	t->setPattern(g_patternNone);


	// 動けや。
	if (t->getTimer() <= 0) {
		t->setTimer(200);
	}


	g_mapActors[key] = t;
	g_idListEnemy.push_back(key);
	return (key);
}


// 弾を撃つ。
// 敵のパラメーター, モデル, 生成の座標。
unsigned int AddEnemy(const char* const id, const char* const modelId, const boost::shared_ptr<EnemyPattern>& pattern, const VECTOR& cPos, const VECTOR& param)
{
	unsigned int k = AddEnemy(id, modelId, cPos, param);
	auto a = g_mapActors[k];


	auto p = boost::dynamic_pointer_cast<EnemyParam>(a);
	p->setPattern(pattern);
	if (p->getTimer() <= 0) {
		p->setTimer(200);
	}

	g_mapActors[k] = p;
	return (k);
}


// ツイート作成の敵を作成。
unsigned int AddTweet(const char* const id, const VECTOR& cPos, const aen::gl::Picture& texture)
{
	std::ostringstream oss;
	// たまデータの取得。
	EnemyParam tmp = EnemyParam::CreateFromScript(id);


	// ハッシュキー
	oss << "TWEET*" << id << ":" << "@" << "[" << GetActorIdStr() << "]";
	unsigned int key = GetHash(oss.str().c_str());


{
	auto a = boost::shared_ptr<ProcessEnemyUpdate>(new ProcessEnemyUpdate());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);
	g_managerProcessUpdate.attach(a);
}


{
	auto a = boost::shared_ptr<ProcessEnemyDraw>(new ProcessEnemyDraw());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);
	a->setTexture(texture);
	g_managerProcessDraw.attach(a);
}


	// 生成。
	auto t = boost::shared_ptr<EnemyParam>(new EnemyParam(tmp));
	t->setId(key);
	t->setPos(cPos);
//	t->setPatternType(EnemyPattern::TYPE_NONE);


	// 弾は全て Matrix 管理。
	MATRIX m = MGetIdent();
	m = MMult(m, MGetTranslate(cPos));
	t->setMatrix(m);


	// 何もしない。
const auto g_patternNone = boost::shared_ptr<EnemyPatternNone>(new EnemyPatternNone());
	t->setPattern(g_patternNone);


	// 動けや。
	if (t->getTimer() <= 0) {
		t->setTimer(200);
	}


	g_mapActors[key] = t;
	g_idListEnemy.push_back(key);
	return (key);
}


//---------------------------------------------------------
// 敵の動作
//---------------------------------------------------------


// 自キャラに向かって進み、60フレームに一度、弾を放つ。
EnemyParam EnemyPatternHomingPlayerAndShotBullet::update(EnemyParam a)
{
	VECTOR mPos = a.getPos();


	// プレイヤーに向かって進む。
	if (this->m_counter % 10 == 0) {
		float speed = 5;
		auto player = g_mapActors[HASH_KEY_PLAYER];
		VECTOR pPos = player->getPos();

		// ベクトルの正規化
		VECTOR rad = VNorm(VGet(
			(pPos.x) - (mPos.x),
			(pPos.y) - (mPos.y),
			(pPos.z) - (mPos.z)
		));


		this->m_addX = rad.x * speed;
		this->m_addY = rad.y * speed;
		this->m_addZ = rad.z * speed;
	}


	// 打てや打てや
	if (this->m_counter % 60 == 0) {
/*		AddBulletEnemy("test", "test", boost::shared_ptr<BulletPatternAddVec>(
			new BulletPatternAddVec(VScale(VGet(this->m_addX, this->m_addY, this->m_addZ), 2))
		), mPos, VGet(0, 0, 0));*/
AddBulletEnemy("test2", "test2", boost::shared_ptr<BulletPatternHomingPlayer>(
			new BulletPatternHomingPlayer(500, 20, 20.0f, 0.0f)
		), mPos, VGet(0, 0, 0));
	}


	if (GetRand(100) == 0) {
		std::ostringstream oss;
		oss << "noise-" << GetRand(5)+1;
AddEffect("noise", oss.str().c_str(), mPos, VGet(0, 0, 0));
	}


	MATRIX m = a.getMatrix();
	m.m[3][0] = mPos.x + this->m_addX;
	m.m[3][1] = mPos.y + this->m_addY;
	m.m[3][2] = mPos.z + this->m_addZ;


	this->m_counter++;
	a.setMatrix(m);
	a = EnemyPattern::update(a);
	return (a);
}


