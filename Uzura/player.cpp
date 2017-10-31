/*
 *	Uzura
 *	Player
 *	2012 / 09 / 19
 *	jskny
*/


#include	<boost/shared_ptr.hpp>
#include	<aen.h>
#include	"player.h"
#include	"actor.h"


#include	"uzura.h"
#include	"bullet.h"
#include	"enemy.h"
#include	"effect.h"


#define		GetRad		aen::util::GetRad


using namespace std;


aen::gl::Model	g_handlePlayer;


// TODO: 飛行機のパラメーターの外部ファイル化。
// 今は InitActors でセットしている。


// プレーヤーの見ている方向へ向かうベクトルを取得。
VECTOR GetPlayerRay(void)
{
	VECTOR ret = VGet(0, 0, 1);
	auto a = g_mapActors[HASH_KEY_PLAYER];
	MATRIX m = a->getMatrix();


	// 並行移動を消す。
	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	// 正規化
	ret = VTransform(ret, m);
// aen::SerializeVector(cout, ret);


//	DrawLine3D(a->getPos(), ret, GetColor(0xFF, 0x00, 0x00));	
	return (ret);
}


// プレーヤーの見ている方向へ向かう行列を取得。
MATRIX GetPlayerRayMatrix(void)
{
	auto a = g_mapActors[HASH_KEY_PLAYER];
	MATRIX m = a->getMatrix();


	// 並行移動を消す。
	// POINT: 危険。Y 回転を逆にする。
	for (int i = 0; i < 3; ++i) {
		m.m[1][i] = -m.m[1][i];
	}


	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	return (m);
}


// 自キャラのポジションを取得
inline VECTOR GetPlayerPos(void)
{
	auto c = g_mapActors[HASH_KEY_PLAYER];
	return (c->getPos());
}


bool InitPlayer(void)
{
	g_handlePlayer = GetModel("./data/models/nc_plane/data.mqo");
	MV1SetMatrix(g_handlePlayer.getHandle(), MGetIdent());
	auto c = g_mapActors[HASH_KEY_PLAYER];
	aen::CollisionBox3d box;
	box.ix = 100;
	box.iy = 100;
	box.iz = 100;

	box.bx = 50;
	box.by = 50;
	box.bz = 50;
	c->setCbox(box);


	c->setMatrix(MGetIdent());
	c->setPos(VGet(0, 0, 0));
	g_mapActors[HASH_KEY_PLAYER] = c;


	// カメラ更新 50 個のバッファを使っているから。
	for (int i = 0; i < 50; ++i) {
		g_uzura->updateCameraMatrix();
	}


	auto a = boost::shared_ptr<aen::system::task::Process>(new ProcessPlayerUpdate());
	g_managerProcessUpdate.attach(a);
	a = boost::shared_ptr<aen::system::task::Process>(new ProcessPlayerDraw());
	g_managerProcessDraw.attach(a);
	return (true);
}


void ProcessPlayerUpdate::init(void)
{
}


void ProcessPlayerUpdate::update(void)
{
using namespace aen::gl::input;
// cout << "UPDATE" << endl;
	auto c = g_mapActors[HASH_KEY_PLAYER];


	float inputx = 0.0f, inputy = 0.0f, inputz = 0.0f;
	bool isUseJoystick = Joystick::getNum() > 0;
	if (isUseJoystick) {
		// X 移動。
		inputz = -GetRad(3) * (Joystick::analogX(Joystick::PAD_1) / 1000);
		// Y 移動。
		inputx = -GetRad(3) * (Joystick::analogY(Joystick::PAD_1) / 1000);
	}


DINPUT_JOYSTATE input;
GetJoypadDirectInputState(DX_INPUT_PAD1, &input);


	// POV で操作。
	if (isUseJoystick && input.POV[0] != -1) {
		// 入力がある。
		int a = input.POV[0];
		int v = 3000;
		if (a < 9000*0+v || a > 9000*4-v) {
			// 上
			inputx = +GetRad(3);
		}
		else if (a < 9000*2+v && a > 9000*2-v) {
			// 下
			inputx = -GetRad(3);
		}


		if (a < 9000*3+v && a > 9000*3-v) {
			// 左
			// inputz = + GetRad(3);
			inputy = - GetRad(1);
		}
		else if (a < 9000*1+v && a > 9000*1-v) {
			// 右
			// inputz = - GetRad(3);
			inputy = + GetRad(1);
		}
	}


	if(CheckHitKey(KEY_INPUT_LEFT) || Joystick::isInput(Joystick::PAD_1, Joystick::LEFT)) {
		inputz = + GetRad(3);
	}
	else if(CheckHitKey(KEY_INPUT_RIGHT) || Joystick::isInput(Joystick::PAD_1, Joystick::RIGHT)) {
		inputz = - GetRad(3);
	}


	if(CheckHitKey(KEY_INPUT_UP) || Joystick::isInput(Joystick::PAD_1, Joystick::UP)) {
		inputx = + GetRad(3);
	}
	else if(CheckHitKey(KEY_INPUT_DOWN) || Joystick::isInput(Joystick::PAD_1, Joystick::DOWN)) {
		inputx = - GetRad(3);
	}


	if(CheckHitKey(KEY_INPUT_Z) || Joystick::isInput(Joystick::PAD_1, Joystick::L)) {
		inputy = - GetRad(1);
	}
	else if(CheckHitKey(KEY_INPUT_X) || Joystick::isInput(Joystick::PAD_1, Joystick::R)) {
		inputy = + GetRad(1);
	}


	// プレイヤー座標更新
	float speed = c->getSpeed();
static	bool flagSpeedUp = false;
static	bool flagSpeedDown = false;
	// 減速
	if (CheckHitKey(KEY_INPUT_A) || Joystick::isInput(Joystick::PAD_1, Joystick::L2) || (isUseJoystick && input.Z > 500)) {
		speed *= 1.0f/4.0f; // 1/4
		inputx *= 0.5;
		inputy *= 0.5;
		inputz *= 0.5;

		if (!flagSpeedDown) {
			PlaySE("gas02");
			flagSpeedDown = true;
		}


using namespace aen::gl;
Blend::push();
Blend::set(Blend::BT_ADD, 0xFF/2);
DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0x10, 0x00, 0x00), TRUE);
Blend::pop();
	}
	else {
		flagSpeedDown = false;
	}


	if (CheckHitKey(KEY_INPUT_O) || Joystick::isInput(Joystick::PAD_1, Joystick::R2) || (isUseJoystick && input.Z < -500)) {
		// 加速
		speed *= 2.0f; // 1.5 倍
		inputx *= 0.5;
		inputy *= 0.5;
		inputz *= 0.5;


		if (!flagSpeedUp) {
			PlaySE("eco00");
			flagSpeedUp = true;
		}


using namespace aen::gl;
Blend::push();
Blend::set(Blend::BT_ADD, 0xFF/2);
DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0x00, 0x00, 0x20), TRUE);
Blend::pop();
	}
	else {
		flagSpeedUp = false;
	}


	if (CheckHitKey(KEY_INPUT_S)) {
		speed = 0;
	}


	VECTOR v;
	MATRIX m = MV1GetMatrix(g_handlePlayer.getHandle());
	MATRIX temp = m;
	//モデルから引っ張ってきた行列から平行移動要素を
	//キャンセルして姿勢行列に変換
	temp.m[3][0] = temp.m[3][1] = temp.m[3][2] = 0.0f;
	//ここでクォータニオン登場！
	//モデルの姿勢行列中から、X/Y/Z軸のベクトルを抜き出して
	//入力に応じて回転させる
	v = VGet(m.m[0][0], m.m[0][1], m.m[0][2]);	//x軸の向き
	temp = MMult(temp, MGetRotAxis(v, inputx));
	v = VGet(m.m[1][0], m.m[1][1], m.m[1][2]);	//y軸の向き
	temp = MMult(temp, MGetRotAxis(v, inputy));
	v = VGet(m.m[2][0], m.m[2][1], m.m[2][2]);	//z軸の向き
	temp = MMult(temp, MGetRotAxis(v, inputz));
	//無くしちゃった平行移動要素を加えてモデル行列に書き戻し
	temp = MMult(temp, MGetTranslate(VGet(m.m[3][0], m.m[3][1], m.m[3][2])));


	// 座標更新。
//	c.setMatrix(temp);
	MV1SetMatrix(g_handlePlayer.getHandle(), temp);

	if (CheckHitKey(KEY_INPUT_R)) {
		MV1SetMatrix(g_handlePlayer.getHandle(), MGetIdent());
	}


	// プレイヤー座標更新
//	float speed = c->getSpeed();


	m = MV1GetMatrix(g_handlePlayer.getHandle());
	//モデル行列からZ軸の向きを取り出して、正規化して、速度を掛け算
	v = VGet(m.m[2][0], m.m[2][1], m.m[2][2]);
	v = VNorm(v);
//cout << "N : ";
//aen::SerializeVector(cout, v);
	v = VScale(v, speed);
//cout << "S : ";
//aen::SerializeVector(cout, v);
	//移動ベクトルを姿勢行列に足し込み
	m = MMult(m, MGetTranslate(v));
//	c.setMatrix(m);
	MV1SetMatrix(g_handlePlayer.getHandle(), m);


	c->setMatrix(MV1GetMatrix(g_handlePlayer.getHandle()));
	c->updateMatrix2Pos();
	MV1SetMatrix(g_handlePlayer.getHandle(), c->getMatrix());
	g_mapActors[HASH_KEY_PLAYER] = c;

}


void ProcessPlayerDraw::init(void)
{
}


void ProcessPlayerDraw::update(void)
{
using namespace aen::gl::input;
	auto c = g_mapActors[HASH_KEY_PLAYER];


//	SetUseLighting(TRUE);
	MV1SetMatrix(g_handlePlayer.getHandle(), c->getMatrix());
	MV1DrawModel(g_handlePlayer.getHandle());
//	SetUseLighting(FALSE);


	// 垂直線
	VECTOR tmp = VScale(GetPlayerRay(), 10000);
	tmp = VAdd(tmp, c->getPos());
using namespace aen::gl;
Blend::push();
Blend::set(Blend::BT_ADD, 128);
	DrawLine3D(c->getPos(), tmp, GetColor(0xFF, 0xFF, 0xFF));
Blend::pop();


	if (CheckHitKey(KEY_INPUT_SPACE)
	 || Joystick::isInput(Joystick::PAD_1, Joystick::A)
	 || Joystick::isInput(Joystick::PAD_1, Joystick::B)) {
		// 予備引数は無視。
		AddPlayerBullet("test", "test", boost::shared_ptr<BulletPatternAddVec>(
			new BulletPatternAddVec(VScale(GetPlayerRay(), 200))
		), c->getPos(), VGet(0, 0, 0));


		PlaySE("don07");
	}
	else if (CheckHitKey(KEY_INPUT_E)) {
		// 予備引数は無視。
		AddEnemy("test", "test", c->getPos(), VGet(0, 0, 0));
	}
	else if (CheckHitKey(KEY_INPUT_D)) {
		// 予備引数は無視。
		AddEffect("test", "test", c->getPos(), VGet(0, 0, 0));
	}


}


#undef GetRad

