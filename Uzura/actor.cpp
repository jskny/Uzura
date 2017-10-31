/*
 *	Uzura
 *	Actor
 *	2012 / 09 / 13
 *	jskny
*/


#include	<iostream>
#include	<hash_map>
#include	"actor.h"


#include	"GScripter/gscripter.h"


using namespace std;
using namespace LuaPlus;


// アクター一覧配置。
std::hash_map<unsigned int, boost::shared_ptr<ActorParam>>	g_mapActors;
// タスクシステム。マネージャー
aen::system::task::Manager	g_managerProcessUpdate;
aen::system::task::Manager	g_managerProcessDraw;


const	int		HASH_KEY_PLAYER = aen::hash::AdlerStringLower("player-20121002");
const	int		ACTOR_TYPE_PLAYER = aen::hash::AdlerStringLower("type-player-20121002");


// アクターを破棄する。
bool ActorDel(unsigned int id)
{
	auto itr = g_mapActors.find(id);
	if (itr == g_mapActors.end()) {
// AEN_STREAM_LOG_ERROR() << "Actor id is unjust. : " << id << endl;
// AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Actor id is unjust. : " << id << endl;
// cerr << "Actor id is unjust. : " << id << endl;
		return (false);
	}


	g_mapActors.erase(itr);
// AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_LOW) << "Actor delete is Succeed : " << id << endl;
	return (true);
}


// 登録されている全てのアクターを破棄。
void ActorReset(void)
{
	g_mapActors.clear();
}


// アクターId 生成用の文字列を返す。
std::string GetActorIdStr(void)
{
	std::ostringstream oss;
	oss << GetRand(100) << ":" << GetRand(152) << "@" << GetRand(10000) << ";";
	return (oss.str());
}


// マトリックスをベクトルへ変換
void ActorParam::updateMatrix2Pos(void)
{
	// 行列から平行移動の x, y, z を取り出す、
	this->setPos(
		VGet(this->m_matrix.m[3][0], this->m_matrix.m[3][1], this->m_matrix.m[3][2])
	);


// cout << "SHOW" << endl;
//	aen::SerializeVector(cout, this->getPos());
// aen::SerializeMatrix(cout, this->getMatrix());
}


bool InitActors(void)
{
	g_mapActors.clear();
	g_managerProcessUpdate.clear();
	g_managerProcessDraw.clear();


	// 自キャラは例外。
	auto a = boost::shared_ptr<ActorParam>(new ActorParam());
	a->setId(HASH_KEY_PLAYER);
	a->setPos(VGet(0, 0, 0));
	a->setMatrix(MGetIdent());
	a->setType(ACTOR_TYPE_PLAYER);
	a->setSpeed(50);

	g_mapActors[HASH_KEY_PLAYER] = a;
	return (true);
}


// ActorParam を GScripter から生成。
gscripter::Scripter ActorParam::CreateFromScript(const std::string& urlFile, ActorParam* pParam)
{
	AEN_NULL_TO_THROW_EXCEPTION_PARAM(pParam);
static	hash_map<std::string, ActorParam> g_tmpMap;
static	hash_map<std::string, gscripter::Scripter> g_tmpMapScripter;
	ActorParam ret;
	gscripter::Scripter scripter;


	auto itr =g_tmpMap.find(urlFile);
	if (itr != g_tmpMap.end()) {
		// 既にあるのを返す。
		*pParam = itr->second;
		// すでにあるならば、確実にこちらもあるので、
		scripter = g_tmpMapScripter.find(urlFile)->second;
		return (scripter);
	}


	// スクリプト読み込み
	if (!scripter.load(urlFile)) {
AEN_STREAM_LOG_ERROR() << "GScripter Script load error : " << urlFile << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script load error : " << urlFile << endl;
		cerr << "GScripter Script load error : " << urlFile << endl;
		return (scripter);
	}
	else if (!scripter.lex()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script compile error : " << urlFile << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script compile error : " << urlFile << endl;
		cerr << "GScripter Script compile error : " << urlFile << endl;
		return (scripter);
	}
	else if (!scripter.compile()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script compile error : " << urlFile << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script compile error : " << urlFile << endl;
		cerr << "GScripter Script compile error : " << urlFile << endl;
		return (scripter);
	}


	while (!scripter.isScriptEnd()) {
		if (!scripter.run()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script running error : " << urlFile << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script running error : " << urlFile << endl;
cerr << "GScripter Script running error : " << urlFile << endl;
			break;
		}
	}


	ret.setHp(scripter.getValue("hp"));


	// 当たり判定領域構築
	VECTOR tmp = VGet(scripter.getValue("ix"), scripter.getValue("iy"), scripter.getValue("iz"));
	ret.setCboxI(tmp);
	tmp = VGet(scripter.getValue("bx"), scripter.getValue("by"), scripter.getValue("bz"));
	ret.setCboxB(tmp);


	g_tmpMap[urlFile] = ret;
	g_tmpMapScripter[urlFile] = scripter;
	*pParam = ret;
	return (scripter);
}

