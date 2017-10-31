/*
 *	Uzura
 *	当たり判定
 *	2012 / 10 / 25
 *	jskny
*/


#include	"collision.h"
#include	"actor.h"
#include	"effect.h"
#include	"enemy.h"


#include	"uzura.h"


using namespace std;


// リストと指定した id で当たり判定。
int CollisionListById(std::list<unsigned int>& ids, unsigned int const key, bool flagDecHp, int maxCount)
{
	if (maxCount <= 0) {
		return (0);
	}


	auto tmp = g_mapActors.find(key);
	if (tmp == g_mapActors.end()) {
AEN_STREAM_LOG_ERROR() << "Can not match key : " << key << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not match key : " << key << endl;
		cerr << "Can not match key : " << key << endl;
		return (0);
	}


	AEN_NULL_TO_THROW_EXCEPTION(tmp->second.get());
	aen::CollisionBox3d tmpC = tmp->second->getCbox();


	auto itr = ids.begin();
	int count = 0;
	while (itr != ids.end()) {
		auto c = g_mapActors.find(*itr);
		if (c == g_mapActors.end()) {
			itr = ids.erase(itr);
			continue;
		}
		else if (c->second.get() == NULL) {
			itr = ids.erase(itr);
			continue;
		}


		// 当たり判定
		if (tmpC.isHit(c->second->getCbox())) {
cout << "Collision HIT : " << key << "@" << *itr << endl;
			if (flagDecHp) {
				tmp->second->setHp(tmp->second->getHp() - 1);
				g_mapActors[key] = tmp->second;


				c->second->setHp(c->second->getHp() - 1);
				g_mapActors[*itr] = c->second;
			}


			++count;
			if (count >= maxCount) {
				return (count);
			}
		}


		++itr;
	}


	return (count);
}


int CollisionListByListAndLifeDec(std::list<unsigned int>& ids, std::list<unsigned int>& ids2)
{
	int count = 0;
	auto itr = ids.begin();
	while (itr != ids.end()) {
		auto c = g_mapActors.find(*itr);
		if (c == g_mapActors.end()) {
			itr = ids.erase(itr);
			continue;
		}
		else if (c->second.get() == NULL) {
			itr = ids.erase(itr);
			continue;
		}


		if (CollisionListById(ids2, *itr) != 0) {
			++count;
			c->second->setHp(c->second->getHp() - 1);
AddEffect("test", "test", c->second->getPos(), VGet(0, 0, 0));
PlaySE("bom11");

			if (c->second->getHp() <= 0) {
				if (c->second->getType() == EnemyParam::TYPE_ENEMY) {
					// アップキャスト。
					auto o = boost::dynamic_pointer_cast<EnemyParam>(c->second);
					SetScore(GetScore() + o->getScore());
				}
			}


			g_mapActors[*itr] = c->second;
		}


		++itr;
	}


	return (count);
}

