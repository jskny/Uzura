/*
 *	Uzura
 *	Bullet
 *	2012 / 10 / 04
 *	jskny
*/


#include	<sstream>
#include	<boost/shared_ptr.hpp>
#include	<aen.h>
#include	"uzura.h"
#include	"bullet.h"
#include	"actor.h"

#include	"scripting.h"
#include	"player.h"


using namespace std;
using namespace PSL;


const int BulletParam::TYPE_NONE = 0;
const int BulletParam::TYPE_C = 6422355;
const int BulletParam::TYPE_SCRIPT = 1236341;


const unsigned int BulletPattern::MOVE_NONE = GetHash("none");
const unsigned int BulletPattern::MOVE_NORMAL = GetHash("normal");
const unsigned int BulletPattern::MOVE_HOMING = GetHash("homing");


const unsigned int ACTOR_TYPE_ENEMY = GetHash("ACTOR_TYPE_ENEMY:12524");


std::list<unsigned int> g_idListPlayerBullet;
std::list<unsigned int> g_idListBulletEnemy;


// 特定の要素を消す。
bool EraseKeyListAndActorMap(unsigned int k, std::list<unsigned int>* pList)
{
	auto itr = pList->begin();
	while (itr != pList->end()) {
		unsigned int key = *itr;
		auto tmp = g_mapActors.find(*itr);


		if (tmp == g_mapActors.end()) {
			// 存在しないし。
			itr = pList->erase(itr);
			return (false);
		}


		// 要素を消す。
		if (key == k) {
			pList->erase(itr);
			// 要素を消す。
			g_mapActors.erase(key);
			break;
		}
		++itr;
	}


	return (true);	
}


// すべての要素を消す。
void EraseAllListAndActorMap(std::list<unsigned int>* pList)
{
	auto itr = pList->begin();
	while (itr != pList->end()) {
		unsigned int key = *itr;
		auto tmp = g_mapActors.find(*itr);


		if (tmp == g_mapActors.end()) {
			itr = pList->erase(itr);
			continue;
		}


		// 要素を消す。
		g_mapActors.erase(*itr);
		++itr;
	}


	pList->clear();
	return;
}


bool InitBullet(void)
{
	EraseAllListAndActorMap(&g_idListPlayerBullet);
	EraseAllListAndActorMap(&g_idListBulletEnemy);


	g_idListPlayerBullet.clear();
	return (true);
}


// アクターを消すため少々改造。
void ProcessBullet::kill(void)
{
	if (this->m_actorId != 0) {
		ActorDel(this->m_actorId);

		if (this->getType() == ACTOR_TYPE_ENEMY) {
			EraseKeyListAndActorMap(this->m_actorId, &g_idListBulletEnemy);
		}
		else {
			// 自弾
			EraseKeyListAndActorMap(this->m_actorId, &g_idListPlayerBullet);
		}


cout << "Bullet killd. [" << this->m_actorId << "]" << endl;
	}


	Process::kill();
}


aen::gl::Model GetModelBullet(const char* const id)
{
	std::ostringstream oss;
	oss << "./data/models/bullets/" << id << "/data.mqo";
	return (GetModel(oss.str()));
}


// PSL 設定ファイルからパラメーターを生成。
BulletParam BulletParam::CreateFromPsl(const char* const id)
{
static	hash_map<std::string, BulletParam> g_tmpMap;
	BulletParam ret;
	gscripter::Scripter scripter;


	std::ostringstream oss;
	oss.str("");
	oss << "./data/conf/bullets/" << id << "/conf.txt";
	auto itr = g_tmpMap.find(oss.str());
	if (itr != g_tmpMap.end()) {
		return (itr->second);
	}


	// 基底情報の読み込み。
	scripter = ActorParam::CreateFromScript(oss.str(), &ret);


	ret.setTimer(scripter.getValue("timer"));
	ret.m_patternType = GetHash(scripter.getString("pattern").c_str());
	if (ret.m_patternType == 0) {
		ret.m_patternType = BulletParam::TYPE_NONE;
	}


cout << "Bullet Serialize:" << endl;
ret.serialize(cout);
	g_tmpMap[oss.str()] = ret;
	return (ret);
}


// 読み込みテスト、
void TestBulletConfFile(const char* const id)
{
	std::ostringstream oss;
	oss.str("");
	oss << "./data/conf/bullets/" << id << "/conf.psl";
	// 設定情報を読み込む。
	PSLVM pslVm;
	// 関連付け。
	PSL_SetUzuraFunc(pslVm);


	PSLVM::error e = pslVm.loadScript(oss.str().c_str());
	if (e == PSLVM::FOPEN_ERROR) {
		cerr << "PSL Script compile error : " << oss.str() << endl;
		return;
	}


	// スクリプト実行。
	PSL::variable a = pslVm.run();
	if (static_cast<int>(a) == 0) {
		// 実行失敗。
		cerr << "PSL Script running error : " << oss.str() << endl;
		return;
	}


	// 設定値の取得。
	a = pslVm.get("flagScripting");
cout << "flagScripting : " << static_cast<int>(a) << endl;
	a = pslVm.get("addRotateX");
cout << "addRotateX : " << static_cast<double>(a) << endl;
	a = pslVm.get("addRotateY");
cout << "addRotateY : " << static_cast<double>(a) << endl;
	a = pslVm.get("addRotateZ");
cout << "addRotateZ : " << static_cast<double>(a) << endl;
	a = pslVm.get("timer");
cout << "timer : " << static_cast<double>(a) << endl;


	a = pslVm.get("pattern");
cout << "pattern : " << a.c_str() << endl;


/*
	int v = a;
cout << "FLAG : " << v << endl;
cout << "LENGTH : " << a.length() << endl;
cout << "Exist f = " << a.exist("f") << " @ " << a.exist("GG") << endl;
cout << a.c_str() << endl;
cout << a.type() << endl;
cout << (bool)a << "," << (int)a << "," << (double)a << endl;*/


	return;
}


// スクリプト駆動。
boost::shared_ptr<BulletParam> BulletPatternScripting(unsigned int actorId)
{
	auto itr = g_mapActors.find(actorId);
	if (itr == g_mapActors.end()) {
		AEN_THROW_EXCEPTION("Actor id is unjust.");
	}


	auto param = boost::dynamic_pointer_cast<BulletParam>(itr->second);
	if (!param->getScripting()) {
		AEN_THROW_EXCEPTION("Script is not set.");
	}


	MATRIX m = param->getMatrix();
	std::ostringstream oss;
	variable argv;


	// 行列を放り込む。
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			oss.str("");
			oss << "m" << y << x;
			argv.set(oss.str().c_str(), m.m[y][x]);
		}
	}


	variable array = (*param->getScripting())(argv);


	// 返却結果の行列を取り出す。
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			oss.str("");
			oss << "m" << y << x;
			m.m[y][x] = array[oss.str().c_str()];
		}
	}


	param->setMatrix(m);
//	param->updateMatrix2Pos();
//	g_mapActors[actorId] = param;
	return (param);
}


// たまデータを読み込む。
bool ProcessBullet::load(const char* const id)
{
	std::ostringstream oss;
	oss << "./data/models/" << id << "/data.mqo";


	if (!this->m_model.load(oss.str().c_str())) {
AEN_WARN(g_uzura, "can not load model.");
		return (false);
	}


	oss.str("");
	oss << "./data/conf/" << id << "/conf.lua";
	// 設定情報を読み込む。
	PSLVM pslVm;
	PSLVM::error e = pslVm.loadScript(oss.str().c_str());
	if (e == PSLVM::FOPEN_ERROR) {
		cerr << "PSL Script compile error : " << oss.str() << endl;
		return (false);
	}


	// スクリプト実行。
	pslVm.run();
	// 設定値の取得。
	PSL::variable a = pslVm.get("flagScripting");
//	cout << "FLAG = " << (int)a << endl;


	return (true);
}


// 弾を生成
unsigned int AddBullet(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param, std::list<unsigned int>* pList)
{
	std::ostringstream oss;
	// たまデータの取得。
	BulletParam a = BulletParam::CreateFromPsl(id);


	// ハッシュキー
	oss << id << ":" << modelId << "@" << "[" << GetActorIdStr() << "]";
	unsigned int key = GetHash(oss.str().c_str());


{
	auto a = boost::shared_ptr<ProcessBulletUpdate>(new ProcessBulletUpdate());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);
	g_managerProcessUpdate.attach(a);
}


{
	auto a = boost::shared_ptr<ProcessBulletDraw>(new ProcessBulletDraw());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);

	auto model = GetModelBullet(modelId);
	a->setModel(model);
	g_managerProcessDraw.attach(a);
}


	// 生成。
	auto t = boost::shared_ptr<BulletParam>(new BulletParam(a));
	t->setId(key);
	t->setPatternType(BulletParam::TYPE_NONE);
	t->setPos(cPos);


	// 弾は全て Matrix 管理。
	MATRIX m = MGetIdent();
	m = MMult(m, MGetTranslate(cPos));
	t->setMatrix(m);


	if (t->getPatternType() == BulletPattern::MOVE_NONE) {
		// 何もしない。
static	const auto a = boost::shared_ptr<BulletPattern>(new BulletPattern());
		t->setPattern(a);
	}
	// 指定したベクトルを加算し続ける。
	else if (t->getPatternType() == BulletPattern::MOVE_NORMAL) {
		auto a = boost::shared_ptr<BulletPatternAddVec>(new BulletPatternAddVec(param));
		t->setPattern(a);
	}


	// 動けや。
	if (t->getTimer() <= 0) {
		t->setTimer(200);
	}

	g_mapActors[key] = t;
	pList->push_back(key);
	return (key);
}


// たまのパラメーター, モデル, 発射地点の座標。, 補助引数。
unsigned int AddPlayerBullet(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param)
{
	return (AddBullet(id, modelId, cPos, param, &g_idListPlayerBullet));
}


// 弾を撃つ。
// たまのパラメーター, モデル, パターンデータ, 発射地点の座標。
unsigned int AddPlayerBullet(const char* const id, const char* const modelId, const boost::shared_ptr<BulletPattern>& pattern, const VECTOR& cPos, const VECTOR& param)
{
	unsigned int k = AddPlayerBullet(id, modelId, cPos, param);
	auto a = g_mapActors[k];


	auto p = boost::dynamic_pointer_cast<BulletParam>(a);
	p->setPattern(pattern);
	if (p->getTimer() <= 0) {
		p->setTimer(200);
	}

	g_mapActors[k] = p;
	return (k);
}


// たまのパラメーター, モデル, 発射地点の座標。, 補助引数。
unsigned int AddBulletEnemy(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param)
{
	return (AddBullet(id, modelId, cPos, param, &g_idListBulletEnemy));
}


// 弾を撃つ。
// たまのパラメーター, モデル, パターンデータ, 発射地点の座標。
unsigned int AddBulletEnemy(const char* const id, const char* const modelId, const boost::shared_ptr<BulletPattern>& pattern, const VECTOR& cPos, const VECTOR& param)
{
	unsigned int k = AddBulletEnemy(id, modelId, cPos, param);
	auto a = g_mapActors[k];


	auto p = boost::dynamic_pointer_cast<BulletParam>(a);
	p->setPattern(pattern);
	g_mapActors[k] = p;
	return (k);
}


// 描画処理
void ProcessBullet::draw(void)
{
	if (g_mapActors.find(this->m_actorId) == g_mapActors.end()) {
		// 死んでいるもの。
		this->kill();
		return;
	}


	auto a = boost::dynamic_pointer_cast<BulletParam>(g_mapActors[this->m_actorId]);
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
//	a->updateMatrix2Pos();
	MATRIX tmp = a->getMatrix();
	VECTOR tmp2 = VGet(tmp.m[3][0], tmp.m[3][1], tmp.m[3][2]);
	a->updateCollision(tmp2);


	// コリジョン領域を出力
	if (g_uzura->isDebug()) {
		auto d = a->getCbox();
//		d.setPos(tmp2);
		d.draw();
	}

	MV1SetMatrix(this->m_model.getHandle(), a->getMatrix());
	// 出力
	MV1DrawModel(this->m_model.getHandle());
	g_mapActors[this->m_actorId] = a;
	return;
}


// 座標移動処理。
void ProcessBullet::move(void)
{
	if (g_mapActors.find(this->m_actorId) == g_mapActors.end()) {
		// 死んでいるもの。
		this->m_actorId = NULL;
		this->kill();
		return;
	}


	auto a = boost::dynamic_pointer_cast<BulletParam>(g_mapActors[this->m_actorId]);
	if (a->getTimer() <= 0) {
		// 破棄処理は kill 内で行われている。
		// g_mapActors.erase(this->m_actorId);
		this->kill();
		return;
	}


// a->serialize(cout);
	if (a->getType() == a->TYPE_SCRIPT) {
		// スクリプト駆動。
		auto p = BulletPatternScripting(a->getId());
		a->setMatrix(MMult(a->getMatrix(), p->getMatrix()));
	}
	else {
/*		MATRIX tmp = a->getMatrix();
		VECTOR tmp2 = VGet(tmp.m[3][0], tmp.m[3][1], tmp.m[3][2]);
		VECTOR tmp3 = a->getPattern()->update(tmp2);
		tmp.m[3][0] = tmp3.x, tmp.m[3][1] = tmp3.y, tmp.m[3][2] = tmp3.z;
		// 平行移動だけをする。
		a->setMatrix(tmp);*/
		(*a) = a->getPattern()->update((*a));
	}


	if (a->getHp() <= 0) {
		this->kill();
		return;
	}


	// コリジョン更新関連
	a->updateCollision(a->getPos());
	a->setTimer(a->getTimer() - 1);

	g_mapActors[this->m_actorId] = a;
	return;
}


void ProcessBulletUpdate::update(void)
{
	this->move();
	if (this->isKill()) {
		return;
	}


}


void ProcessBulletUpdate::init(void)
{
	if (this->m_actorId == 0) {
		this->m_actorId = BulletParam::TYPE_NONE;
	}


	return;
}


void ProcessBulletDraw::update(void)
{
	this->draw();
	if (this->isKill()) {
		return;
	}


}


void ProcessBulletDraw::init(void)
{
	if (this->m_actorId == 0) {
		this->m_actorId = BulletParam::TYPE_NONE;
	}


}


//---------------------------------------------------------
// 自キャラ追尾弾
//---------------------------------------------------------


BulletParam BulletPatternHomingPlayer::update(BulletParam a)
{
	if (this->m_counter <= 0) {
		a.setHp(0);
		return (a);
	}


	VECTOR mPos = a.getPos();
	// 自キャラを追尾せよ。
	if (this->m_t % this->m_updateTimer == 0) {
		auto player = g_mapActors[HASH_KEY_PLAYER];
		VECTOR pPos = player->getPos();

		// ベクトルの正規化
		VECTOR rad = VNorm(VGet(
			(pPos.x) - (mPos.x),
			(pPos.y) - (mPos.y),
			(pPos.z) - (mPos.z)
		));


		this->m_addX = rad.x * this->m_speed;
		this->m_addY = rad.y * this->m_speed;
		this->m_addZ = rad.z * this->m_speed;
	}


	--this->m_counter;
	++this->m_t;


	MATRIX m = a.getMatrix();
	m.m[3][0] = mPos.x + this->m_addX;
	m.m[3][1] = mPos.y + this->m_addY;
	m.m[3][2] = mPos.z + this->m_addZ;


	a.setMatrix(m);
	a = BulletPattern::update(a);
	return (a);
}

