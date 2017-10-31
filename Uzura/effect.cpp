/*
 *	Uzura
 *	エフェクト
 *	2012 / 10 / 30
 *	jskny
*/


#include	<sstream>
#include	<boost/shared_ptr.hpp>
#include	<aen.h>
#include	"uzura.h"
#include	"effect.h"
#include	"actor.h"
#include	"player.h"

#include	"scripting.h"


using namespace std;


std::list<unsigned int> g_idListEffect;


bool InitEffect(void)
{
	g_managerProcessDraw.clear();
	g_managerProcessUpdate.clear();


	auto itr = g_idListEffect.begin();
	while (itr != g_idListEffect.end()) {
		unsigned int key = *itr;
		auto tmp = g_mapActors.find(*itr);


		if (tmp == g_mapActors.end()) {
			itr = g_idListEffect.erase(itr);
			continue;
		}


		// 要素を消す。
		g_mapActors.erase(*itr);
		++itr;
	}


	g_idListEffect.clear();
	return (true);
}


// アクターを消すため少々改造。
void ProcessEffect::kill(void)
{
	if (this->m_actorId != 0) {
		ActorDel(this->m_actorId);

		for (auto itr = g_idListEffect.begin(); itr != g_idListEffect.end(); ++itr) {
			if (*itr == this->m_actorId) {
				g_idListEffect.erase(itr);
				break;
			}
		}
	}


	Process::kill();
}


// 設定ファイルからパラメーターを生成。
EffectParam EffectParam::CreateFromScript(const char* const id)
{
static	hash_map<std::string, EffectParam> g_tmpMap;
	EffectParam ret;
	gscripter::Scripter scripter;


	std::ostringstream oss;
	oss.str("");
	oss << "./data/conf/effects/" << id << "/conf.txt";
	auto itr = g_tmpMap.find(oss.str());
	if (itr != g_tmpMap.end()) {
		return (itr->second);
	}


	// 基底情報の読み込み。
	scripter = ActorParam::CreateFromScript(oss.str(), &ret);


	ret.setTimer(scripter.getValue("timer"));
	ret.setType(scripter.getValue("type"));
	ret.setSpeed(scripter.getValue("speed"));
	ret.setSize(scripter.getValue("size"));
cout << "Effect Serialize:" << endl;
ret.serialize(cout);
	g_tmpMap[oss.str()] = ret;
	return (ret);
}


bool ProcessEffect::loadTexture(const char* const id)
{
	if (!id) {
AEN_STREAM_LOG_ERROR() << "null pointer." << endl;
return (false);
	}


	std::ostringstream oss;
	string url;
	oss << "./data/graphics/" << id << "/data.png";
	url = oss.str();

	oss.str("");
	oss << "./data/graphics/" << id << "/conf.txt";


	this->m_texture = GetTexture(url, oss.str());
	this->setType(this->MODE_TEXTURE);
	return (true);
}


bool ProcessEffect::loadSound(const char* const id)
{
	if (!id) {
AEN_STREAM_LOG_ERROR() << "null pointer." << endl;
return (false);
	}


	std::ostringstream oss, oss2;
	oss << "./data/sounds/" << id << "/data";
	oss2 << "./data/sounds/" << id << "/conf.txt";
	this->m_player.stop();
	this->m_player = GetSound(oss.str(), oss2.str());
	return (true);
}


// エフェクト生成
// テクスチャみたいなオブジェクト
// efectId, motionId(画像), 座標, param
unsigned int AddEffect(const char* const id, const char* const modelId, const VECTOR& cPos, const VECTOR& param)
{
	std::ostringstream oss;
	// たまデータの取得。
	EffectParam tmp = EffectParam::CreateFromScript(id);


	// ハッシュキー
	oss << "EFFECT*" << id << ":" << modelId << "@" << "[" << GetActorIdStr() << "]";
	unsigned int key = GetHash(oss.str().c_str());


{
	auto a = boost::shared_ptr<ProcessEffectUpdate>(new ProcessEffectUpdate());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);
	g_managerProcessUpdate.attach(a);
}


{
	auto a = boost::shared_ptr<ProcessEffectDraw>(new ProcessEffectDraw());
	AEN_NULL_TO_THROW_EXCEPTION(a);


	a->setId(key);


	if (a->MODE_TEXTURE == tmp.getType()) {
		a->loadTexture(modelId);
	}

	g_managerProcessDraw.attach(a);
}


	// 生成。
	auto t = boost::shared_ptr<EffectParam>(new EffectParam(tmp));
	t->setId(key);
//	t->setPatternType(EnemyPattern::TYPE_NONE);


	// 弾は全て Matrix 管理。
	MATRIX m = MGetIdent();
	m = MMult(m, MGetTranslate(cPos));
	t->setMatrix(m);


	// 動けや。
	if (t->getTimer() <= 0) {
		t->setTimer(200);
	}


	g_mapActors[key] = t;
	g_idListEffect.push_back(key);
	return (key);
}


// efectId, soundId
static aen::gl::sound::Player g_tmpSound;
void PlaySE(const char* const soundId)
{
	std::ostringstream oss, oss2;
	oss << "./data/sounds/" << soundId << "/data";
	oss2 << "./data/sounds/" << soundId << "/conf.txt";
	g_tmpSound.stop();
	g_tmpSound = GetSound(oss.str(), oss2.str());
	g_tmpSound.stop();
	g_tmpSound.playOne();
//	o.playOne();
//cout << "Play : " << soundId << ":" << o.isLoad() << "," << o.isPlay() << "@" << oss.str() << "," << oss2.str() << endl;
	return;
}


//---------------------------------------------------------


// 描画処理
void ProcessEffect::draw(void)
{
	if (g_mapActors.find(this->m_actorId) == g_mapActors.end()) {
		// 死んでいるもの。
		this->kill();
		return;
	}


	auto a = boost::dynamic_pointer_cast<EffectParam>(g_mapActors[this->m_actorId]);
	if (this->getType() == this->MODE_SOUND) {
		// 音声を再生。
		this->m_player.playOne();
		this->setType(-1);
	}
	else if (this->getType() == this->MODE_TEXTURE) {
		if (this->getTexture().getSize() != 0) {
			// テクスチャ表示
			int index = (a->getCounter() / a->getSpeed()) % this->getTexture().getSize();
			a->setCounter(a->getCounter() + 1);
			DrawBillboard3D(a->getPos(), 0.5f, 0.5f, a->getSize(), 0.0f, this->getTexture().getHandle(index), TRUE);
// a->serialize(cout);
		}
	}


	g_mapActors[this->m_actorId] = a;


/*	// モデルが、スクリーン座標系のどこに出力されているのか取得する。
	VECTOR eye = ConvWorldPosToScreenPos(a->getPos());
	if (eye.z > 0.0f && eye.z < 1.0f) {
		// 徐々に色が変わるように。
		aen::gl::DrawPolygon2d(eye.x, eye.y, 6, 0, 30, GetColor(0xFF, 0xFF, 0x00), 1, 2);
	}*/
	return;
}


// 座標移動処理。
void ProcessEffect::move(void)
{
	if (g_mapActors.find(this->m_actorId) == g_mapActors.end()) {
		// 死んでいるもの。
		this->kill();
		return;
	}


	auto a = boost::dynamic_pointer_cast<EffectParam>(g_mapActors[this->m_actorId]);
	if (a->getTimer() <= 0) {
		// 破棄処理は kill 内で行われている。
		// g_mapActors.erase(this->m_actorId);
		this->kill();
cout << "Effect killd. [" << this->m_actorId << "]" << endl;
		return;
	}


	// コリジョン更新関連
	a->updateMatrix2Pos();
	a->updateCollision(a->getPos());
// aen::SerializeVector(cout, a->getCbox().getPos());

	a->setTimer(a->getTimer() - 1);


// cout << "A ; "; aen::SerializeVector(cout, a->getPos());
// cout << "B ; "; aen::SerializeVector(cout, ((BulletPatternAddVec*)(a->getPattern().get()))->getVec());
	g_mapActors[this->m_actorId] = a;
	return;
}


//---------------------------------------------------------
// 基底クラスの可愛いい傀儡たち。
//---------------------------------------------------------


void ProcessEffectUpdate::init(void)
{
}


void ProcessEffectUpdate::update(void)
{
	this->move();
	if (this->isKill()) {
		return;
	}

}


void ProcessEffectDraw::init(void)
{
}


void ProcessEffectDraw::update(void)
{
	this->draw();


	if (this->isKill()) {
		return;
	}


	if (g_uzura->isDebug()) {
		auto c = g_mapActors[HASH_KEY_PLAYER];
		auto me = g_mapActors[this->m_actorId];
		me->updateMatrix2Pos();

		DrawLine3D(c->getPos(), me->getPos(), GetColor(0x00, 0xFF, 0x00));
	}


	return;
}
