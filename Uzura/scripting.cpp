/*
 *	Uzura
 *	スクリプティング統括
 *	2012 / 08 / 27
 *	jskny
*/


#include	<iostream>
#include	<sstream>
#include	<aen.h>
#include	"scripting.h"
#include	"stage.h"
#include	"uzura.h"
#include	"input.h"
#include	"actor.h"
#include	"player.h"
#include	"enemy.h"
#include	"effect.h"


using namespace std;
using namespace LuaPlus;


// HalCoco 全体で使用する仮想マシン
LuaPlus::LuaStateOwner g_luaState(true);
// Lua のイベントリスナーが登録される表
std::hash_map<unsigned int, vector<LuaObject>> g_mapLuaScriptListener;
// Lua 関数へのイベントを片っ端から処理するイベントリスナー
boost::shared_ptr<aen::system::event::Listener> g_eventListener;


// Lua のイベント処理を担当するリスナー
class EventListenerLua :public aen::system::event::Listener
{
public:
	bool		handleEvent(aen::system::event::Event* event);


};


// Lua 用のイベント型
class EventLua :public aen::system::event::Event
{
public:
	enum {
		MN_EVENT_LUA = 27923
	};


	EventLua()
	{
		// 通常の Event 型と一緒にされないように、
		this->setPtr((void*)MN_EVENT_LUA);
	}


	EventLua(LuaObject o) :
		m_luaObj(o)
	{
		// 通常の Event 型と一緒にされないように、
		this->setPtr((void*)MN_EVENT_LUA);
	}


	LuaObject	getLuaObject(void) const { return (this->m_luaObj); };
	void		setLuaObject(const LuaObject& obj) { this->m_luaObj = obj; };


protected:
	LuaObject	m_luaObj;


};


// Event 構造体・・・というかメタテーブル。。。
void __CreateMetaTableEvent(void)
{
	// マウスを使えるように
	LuaPlus::LuaObject metaTableObj = g_luaState->GetGlobals().CreateTable("Event");
	metaTableObj.SetObject("__index", metaTableObj);


	metaTableObj.SetInteger("type", 0);
	metaTableObj.SetNumber("val", 0);
	metaTableObj.SetString("text", "");


	return;
}


// Matrix 構造体
LuaObject GetMatrix(void)
{
	LuaPlus::LuaObject metaTableObj = g_luaState->GetGlobal("Matrix");
	if (!metaTableObj.IsNil()) {
		// すでにあるし・・・
		metaTableObj.SetObject("__index", metaTableObj);
		return (metaTableObj);
	}


	// マトリックス
	metaTableObj = g_luaState->GetGlobals().CreateTable("Matrix");
	metaTableObj.SetObject("__index", metaTableObj);


	std::ostringstream oss;
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			oss.str("");
			oss << "_" << (y+1) << (x+1);
			metaTableObj.SetNumber(oss.str().c_str(), 0.0);
		}
	}


	return (metaTableObj);
}


// Matrix Ident 構造体
LuaObject GetMatrixIdent(void)
{
	LuaPlus::LuaObject a = g_luaState->GetGlobal("Matrix");
	if (!a.IsNil()) {
		// すでにあるし・・・
		a.SetObject("__index", a);
	}
	else {
		// マトリックス
		a = g_luaState->GetGlobals().CreateTable("Matrix");
		a.SetObject("__index", a);
	}


// m[0][0]=1.0f m[0][1]=0.0f m[0][2]=0.0f m[0][3]=0.0f
// m[1][0]=0.0f m[1][1]=1.0f m[1][2]=0.0f m[1][3]=0.0f
// m[2][0]=0.0f m[2][1]=0.0f m[2][2]=1.0f m[2][3]=0.0f
// m[3][0]=0.0f m[3][1]=0.0f m[3][2]=0.0f m[3][3]=1.0f

	a.SetNumber("_11", 1.0), a.SetNumber("_12", 0.0), a.SetNumber("_13", 0.0), a.SetNumber("_14", 0.0);
	a.SetNumber("_21", 0.0), a.SetNumber("_22", 1.0), a.SetNumber("_23", 0.0), a.SetNumber("_24", 0.0);
	a.SetNumber("_31", 0.0), a.SetNumber("_32", 0.0), a.SetNumber("_33", 1.0), a.SetNumber("_34", 0.0);
	a.SetNumber("_41", 0.0), a.SetNumber("_42", 0.0), a.SetNumber("_43", 0.0), a.SetNumber("_44", 1.0);
	return (a);
}


// 自キャラのポジションを取得
LuaObject GetVectorPlayer(void)
{
	auto c = g_mapActors[HASH_KEY_PLAYER];
	LuaPlus::LuaObject a = g_luaState->GetGlobals().CreateTable("__GetVectorPlayer");
	a.SetObject("__index", a);


	if (c.get() == 0) {
AEN_STREAM_LOG_ERROR() << "HASH_KEY_PLAYER に対応した Actor は存在しません。" << endl;
		return (a);
	}


	a.SetNumber("x", c->getPos().x);
	a.SetNumber("y", c->getPos().y);
	a.SetNumber("z", c->getPos().z);
	return (a);
}


// カメラのポジションを取得
LuaObject GetVectorCamera(void)
{
	MATRIX m = g_uzura->getCameraMatrix();
	LuaPlus::LuaObject a = g_luaState->GetGlobals().CreateTable("__GetVectorCamera");
	a.SetObject("__index", a);

	a.SetNumber("x", m.m[3][0]);
	a.SetNumber("y", m.m[3][1]);
	a.SetNumber("z", m.m[3][2]);
// aen::SerializeVector(cout, VGet(m.m[3][0], m.m[3][1], m.m[3][2]));
	return (a);
}


// 自キャラのレイを取得
LuaObject Lua_GetPlayerRay(void)
{
	VECTOR tmp = GetPlayerRay();

	MATRIX m = g_uzura->getCameraMatrix();
	LuaPlus::LuaObject a = g_luaState->GetGlobals().CreateTable("__Lua_GetPlayerRay");
	a.SetObject("__index", a);

	a.SetNumber("x", tmp.x);
	a.SetNumber("y", tmp.y);
	a.SetNumber("z", tmp.z);
	return (a);
}


// Uzura の設定を操作する。
extern	void		Config(const char* const key, int value);
// スクリプトを実行する
extern	bool		DoScript(const char* const sid);
//  Lua 宛のイベントを送信。
extern	void		SendEvent(const char* const key, LuaObject obj);
// イベントマネージャーに登録
extern	bool		AddListener(const char* const key, LuaObject obj);
// イベントマネージャーからリスナーを消去
extern	bool		DelListener(const char* const key, LuaObject obj);
// イベントマネージャーから指定したイベントを受信する全てのリスナーを消去
extern	bool		DelListenerKeyAll(const char* const key);
// イベントマネージャーすべての Lua イベントリスナーを抹消。
extern	bool		DelListenerAll(void);


// GetColor
inline int Lua_GetColor(int r, int g, int b)
{
	return (GetColor(r, g, b));
}


// キーボード関連の関数を Lua へ関連付ける。
void SetScriptingKeybord(void)
{
	LuaObject a = g_luaState->GetGlobal("Keybord");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Keybord");


	a.RegisterDirect("isOn", Keybord::isOn);
	a.RegisterDirect("isNowOn", Keybord::isNowOn);
	a.RegisterDirect("isInput", Keybord::isOn);
	a.RegisterDirect("isInputNow", Keybord::isNowOn);
	a.RegisterDirect("on", Keybord::On);
	a.RegisterDirect("off", Keybord::On);


	a.SetInteger("1", KEY_INPUT_1);
	a.SetInteger("2", KEY_INPUT_2);
	a.SetInteger("3", KEY_INPUT_3);
	a.SetInteger("4", KEY_INPUT_4);
	a.SetInteger("5", KEY_INPUT_5);
	a.SetInteger("6", KEY_INPUT_6);
	a.SetInteger("7", KEY_INPUT_7);
	a.SetInteger("8", KEY_INPUT_8);
	a.SetInteger("9", KEY_INPUT_9);
	a.SetInteger("0", KEY_INPUT_0);


	a.SetInteger("A", KEY_INPUT_A);
	a.SetInteger("B", KEY_INPUT_B);
	a.SetInteger("C", KEY_INPUT_C);
	a.SetInteger("D", KEY_INPUT_D);
	a.SetInteger("E", KEY_INPUT_E);
	a.SetInteger("F", KEY_INPUT_F);
	a.SetInteger("G", KEY_INPUT_G);
	a.SetInteger("H", KEY_INPUT_H);
	a.SetInteger("I", KEY_INPUT_I);
	a.SetInteger("J", KEY_INPUT_J);
	a.SetInteger("K", KEY_INPUT_K);
	a.SetInteger("L", KEY_INPUT_L);
	a.SetInteger("M", KEY_INPUT_M);
	a.SetInteger("N", KEY_INPUT_N);
	a.SetInteger("O", KEY_INPUT_O);
	a.SetInteger("P", KEY_INPUT_P);
	a.SetInteger("Q", KEY_INPUT_Q);
	a.SetInteger("R", KEY_INPUT_R);
	a.SetInteger("S", KEY_INPUT_S);
	a.SetInteger("T", KEY_INPUT_T);
	a.SetInteger("U", KEY_INPUT_U);
	a.SetInteger("V", KEY_INPUT_V);
	a.SetInteger("W", KEY_INPUT_W);
	a.SetInteger("X", KEY_INPUT_X);
	a.SetInteger("Y", KEY_INPUT_Y);
	a.SetInteger("Z", KEY_INPUT_Z);

	a.SetInteger("UP", KEY_INPUT_UP);
	a.SetInteger("DOWN", KEY_INPUT_DOWN);
	a.SetInteger("LEFT", KEY_INPUT_LEFT);
	a.SetInteger("RIGHT", KEY_INPUT_RIGHT);

	a.SetInteger("LSHIFT", KEY_INPUT_LSHIFT);
	a.SetInteger("RSHIFT", KEY_INPUT_RSHIFT);

	a.SetInteger("RETURN", KEY_INPUT_RETURN);
	a.SetInteger("SPACE", KEY_INPUT_SPACE);
	a.SetInteger("ESCAPE", KEY_INPUT_ESCAPE);


	a.SetInteger("LCONTROL", KEY_INPUT_LCONTROL);
	a.SetInteger("RCONTROL", KEY_INPUT_RCONTROL);


/*	a.SetInteger("KEY_INPUT_1", KEY_INPUT_1);
	a.SetInteger("KEY_INPUT_2", KEY_INPUT_2);
	a.SetInteger("KEY_INPUT_3", KEY_INPUT_3);
	a.SetInteger("KEY_INPUT_4", KEY_INPUT_4);
	a.SetInteger("KEY_INPUT_5", KEY_INPUT_5);
	a.SetInteger("KEY_INPUT_6", KEY_INPUT_6);
	a.SetInteger("KEY_INPUT_7", KEY_INPUT_7);
	a.SetInteger("KEY_INPUT_8", KEY_INPUT_8);
	a.SetInteger("KEY_INPUT_9", KEY_INPUT_9);
	a.SetInteger("KEY_INPUT_0", KEY_INPUT_0);


	a.SetInteger("KEY_INPUT_A", KEY_INPUT_A);
	a.SetInteger("KEY_INPUT_B", KEY_INPUT_B);
	a.SetInteger("KEY_INPUT_C", KEY_INPUT_C);
	a.SetInteger("KEY_INPUT_D", KEY_INPUT_D);
	a.SetInteger("KEY_INPUT_E", KEY_INPUT_E);
	a.SetInteger("KEY_INPUT_F", KEY_INPUT_F);
	a.SetInteger("KEY_INPUT_G", KEY_INPUT_G);
	a.SetInteger("KEY_INPUT_H", KEY_INPUT_H);
	a.SetInteger("KEY_INPUT_I", KEY_INPUT_I);
	a.SetInteger("KEY_INPUT_J", KEY_INPUT_J);
	a.SetInteger("KEY_INPUT_K", KEY_INPUT_K);
	a.SetInteger("KEY_INPUT_L", KEY_INPUT_L);
	a.SetInteger("KEY_INPUT_M", KEY_INPUT_M);
	a.SetInteger("KEY_INPUT_N", KEY_INPUT_N);
	a.SetInteger("KEY_INPUT_O", KEY_INPUT_O);
	a.SetInteger("KEY_INPUT_P", KEY_INPUT_P);
	a.SetInteger("KEY_INPUT_Q", KEY_INPUT_Q);
	a.SetInteger("KEY_INPUT_R", KEY_INPUT_R);
	a.SetInteger("KEY_INPUT_S", KEY_INPUT_S);
	a.SetInteger("KEY_INPUT_T", KEY_INPUT_T);
	a.SetInteger("KEY_INPUT_U", KEY_INPUT_U);
	a.SetInteger("KEY_INPUT_V", KEY_INPUT_V);
	a.SetInteger("KEY_INPUT_W", KEY_INPUT_W);
	a.SetInteger("KEY_INPUT_X", KEY_INPUT_X);
	a.SetInteger("KEY_INPUT_Y", KEY_INPUT_Y);
	a.SetInteger("KEY_INPUT_Z", KEY_INPUT_Z);*/


}


// マウス関連の関数を Lua へ関連付ける。
void SetScriptingMouse(void)
{
	LuaObject a = g_luaState->GetGlobal("Mouse");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Mouse");


	a.RegisterDirect("isOn", Mouse::isOn);
	a.RegisterDirect("isNowOn", Mouse::isNowOn);

	a.RegisterDirect("x", Mouse::x);
	a.RegisterDirect("y", Mouse::y);
	a.RegisterDirect("vx", Mouse::vx);
	a.RegisterDirect("vy", Mouse::vy);

	a.RegisterDirect("wheel", Mouse::wheel);


	a.SetInteger("L", Mouse::KEY_L);
	a.SetInteger("M", Mouse::KEY_M);
	a.SetInteger("R", Mouse::KEY_R);


}


inline bool I_JoyInputPad1(unsigned int key)
{
	using namespace aen::gl::input;
	return (Joystick::isInput(Joystick::PAD_1,key));
}


inline bool I_JoyInputNowPad1(unsigned int key)
{
	using namespace aen::gl::input;
	return (Joystick::isNowInput(Joystick::PAD_1, key));
}


// ジョイスティック Lua へ関連付ける。
void SetScriptingJoypad(void)
{
using namespace aen::gl::input;
	LuaObject a = g_luaState->GetGlobal("Joystick");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Joystick");


	a.RegisterDirect("isOn", I_JoyInputPad1);
	a.RegisterDirect("isNowOn", I_JoyInputNowPad1);
	a.RegisterDirect("isInput", I_JoyInputPad1);
	a.RegisterDirect("isInputNow", I_JoyInputNowPad1);


	a.SetInteger("_1", PAD_INPUT_1);
	a.SetInteger("_2", PAD_INPUT_2);
	a.SetInteger("_3", PAD_INPUT_3);
	a.SetInteger("_4", PAD_INPUT_4);
	a.SetInteger("_5", PAD_INPUT_5);
	a.SetInteger("_6", PAD_INPUT_6);
	a.SetInteger("_7", PAD_INPUT_7);
	a.SetInteger("_8", PAD_INPUT_8);
	a.SetInteger("_9", PAD_INPUT_9);
	a.SetInteger("_10", PAD_INPUT_10);
	a.SetInteger("_11", PAD_INPUT_11);
	a.SetInteger("_12", PAD_INPUT_12);
	a.SetInteger("_13", PAD_INPUT_13);
	a.SetInteger("_14", PAD_INPUT_14);
	a.SetInteger("_15", PAD_INPUT_15);
	a.SetInteger("_16", PAD_INPUT_16);


	a.SetInteger("A", Joystick::A);
	a.SetInteger("B", Joystick::B);
	a.SetInteger("X", Joystick::X);
	a.SetInteger("Y", Joystick::Y);

	a.SetInteger("L", Joystick::L);
	a.SetInteger("R", Joystick::R);
	a.SetInteger("L2", Joystick::L2);
	a.SetInteger("R2", Joystick::R2);
	a.SetInteger("L3", Joystick::L3);
	a.SetInteger("R3", Joystick::R3);

	a.SetInteger("START", Joystick::START);
	a.SetInteger("SELECT", Joystick::SELECT);

	a.SetInteger("UP", Joystick::UP);
	a.SetInteger("DOWN", Joystick::DOWN);
	a.SetInteger("LEFT", Joystick::LEFT);
	a.SetInteger("RIGHT", Joystick::RIGHT);


}


// Blend::set を呼ぶだけの関数。
// enum 型が、LuaPlus のテンプレートで処理できない模様。
inline void __BlendSet(int type, int param)
{
//	aen::gl::Blend::push();
	aen::gl::Blend::set(static_cast<aen::gl::Blend::BLEND_TYPE>(type), param);
}


// lua 用の getNowCount
inline long Lua_GetNowCount(void) { return (GetNowCount()); };
// Window counter
inline unsigned int Lua_GetWindowCount(void) { return (g_uzura->getWindow()->getCount()); };
inline unsigned int Lua_GetWindowFps(void) { return (g_uzura->getWindow()->getFps()); };
inline unsigned int Lua_GetWindowFrameTime(void) { return (g_uzura->getWindow()->getFtime()); };


inline int Lua_DeleteGraph(int handle) { return (DeleteGraph(handle)); };
// ガウスフィルタ
inline int Lua_GraphFilterGF(int handle, int pixelWidth, int param) { return (GraphFilter(handle, DX_GRAPH_FILTER_GAUSS, pixelWidth , param)); };
// 色調反転
inline int Lua_GraphFilterInvert(int handle) { return (GraphFilter(handle, DX_GRAPH_FILTER_INVERT)); };

// システム has crash
inline void Lua_Crash(const char* const mes) { g_uzura->crash(mes); };


// ブレンド関連を Lua へ関連付ける。
void SetScriptingBlend(void)
{
	LuaObject a = g_luaState->GetGlobal("Blend");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Blend");


using namespace aen::gl;
	a.RegisterDirect("set", __BlendSet);
	a.RegisterDirect("reset", Blend::reset);

	a.RegisterDirect("push", Blend::push);
	a.RegisterDirect("pop", Blend::pop);


	a.RegisterDirect("stackEmpty", Blend::stackEmpty);
	a.RegisterDirect("stackSize", Blend::stackSize);


	a.SetInteger("ADD", Blend::BT_ADD);
	a.SetInteger("SUB", Blend::BT_SUB);
	a.SetInteger("ALPHA", Blend::BT_ALPHA);
	a.SetInteger("INVSRC", Blend::BT_INVSRC);
	a.SetInteger("MULA", Blend::BT_MULA);
	a.SetInteger("NORMAL", Blend::BT_NORMAL);


	return;
}


// 画像読み込み。
int Lua_LoadGraph(const char* const urlFileD, const char* const urlFileF)
{
	std::ostringstream oss;
	oss << "./data/graphics/" << urlFileD << "/" << urlFileF;
	int buf = LoadGraph(oss.str().c_str());
	if (buf == -1) {
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_HANDLE) << "Lua_LoadGraph -> ERROR >> NotFound File '" << oss.str() << "'" << endl;
AEN_STREAM_LOG_ERROR() << "Lua_LoadGraph -> ERROR >> NotFound File '" << oss.str() << "'" << endl;
		AEN_ERROR("can't find graphic file.");
		return (-1);
	}


	return (buf);
}


// 音声読み込み。
int Lua_LoadSound(const char* const soundId)
{
	if (aen::gl::sound::IsNonUseSound()) {
		return (-1);
	}
	

	std::ostringstream oss;
	oss << "./data/sounds/" << soundId;
	auto o = GetSound(oss.str(), "");


	if (o.isLoad()) {
		return (o.getHandle());
	}


cerr << "Lua_LoadSound -> ERROR >> NotFound File '" << oss.str() << "'" << endl;
AEN_STREAM_LOG_ERROR() << "Lua_LoadSound -> ERROR >> NotFound File '" << oss.str() << "'" << endl;
	return (-1);
}


inline int Lua_DeleteSound(int handle) { return (DeleteSoundMem(handle)); };
inline int Lua_PlaySound(int handle, int mode)
{
	if (aen::gl::sound::IsNonUseSound()) {
		return (-1);
	}

	return (PlaySoundMem(handle, mode));
}


inline void Lua_ProfilePush(const char* const name) { g_uzura->getManagerProfile()->push(name, 0, GetColor(0x50, 0xFF, 0xFF)); }
inline void Lua_ProfilePop(void) { g_uzura->getManagerProfile()->pop(); }


// プロファイラを Lua へ関連付ける。
void SetScriptingProfile(void)
{
	LuaObject a = g_luaState->GetGlobal("Profile");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Profile");
	a.RegisterDirect("push", Lua_ProfilePush);
	a.RegisterDirect("pop", Lua_ProfilePop);
	return;
}


// 基準線
inline void Lua_DrawStandardLine3d(float x, float y, float z)
{
	DrawStandardLine3d(VGet(x, y, z));
}


inline float Lua_GetScale(float n) { return (GetScale(n)); }
inline float Lua_GetScaleBpm(float bpm, float n) { return (GetScale(bpm, n)); }


inline void Lua_UpdateBpmSystem(void) { return (UpdateBpmSystem(g_uzura->getBpm())); }


inline int Lua_GetBpm(void) { return (g_uzura->getBpm()); }


inline void Lua_Sleep(unsigned int count) { Sleep(count); };


// 音声関連処理を Lua へ組み込む。
void SetScriptingSound(void)
{
	LuaObject a = g_luaState->GetGlobal("Sound");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Sound");


using namespace aen::gl;
	a.RegisterDirect("load", Lua_LoadSound);
//	a.RegisterDirect("delete", Lua_DeleteSound);

	a.RegisterDirect("play", Lua_PlaySound);
	a.RegisterDirect("stop", StopSoundMem);


	a.RegisterDirect("stackEmpty", Blend::stackEmpty);
	a.RegisterDirect("stackSize", Blend::stackSize);


	a.SetInteger("NORMAL", DX_PLAYTYPE_NORMAL);
	a.SetInteger("BACK", DX_PLAYTYPE_BACK);
	a.SetInteger("BGM", DX_PLAYTYPE_LOOP);
	a.SetInteger("LOOP", DX_PLAYTYPE_LOOP);


	return;
}


inline void Lua_DrawGrid(int x, int y, int z, int segment, float interval, int color, int brightness)
{
	DrawGrid(VGet(x, y, x), segment, interval, color, brightness);
}


// オフセット計算後グリッド出力。
inline void Lua_DrawGridCamera(int segment, float interval, int color, int brightness)
{
	MATRIX m = g_uzura->getCameraMatrix();
//	auto c = g_mapActors[HASH_KEY_PLAYER];
//	m = c->getMatrix();
	VECTOR vec = VGet(
		static_cast<int>(m.m[3][0] / interval) * interval,
		static_cast<int>(m.m[3][1] / interval) * interval,
		static_cast<int>(m.m[3][2] / interval) * interval);
	DrawGrid(vec, segment, interval, color, brightness);
}


// 手抜きカメラ初期化
inline void Lua_InitCamera(void){ aen::gl::InitCamera(); };


// 手抜きカメラ実行。
inline void Lua_UpdateCamera(float x, float y, float z)
{
	auto v = VGet(x, y, z);
// cout << "UPDATE CAMERA : ";
aen::SerializeVector(cout, v);
	aen::gl::UpdateCamera(v);
}


inline void Lua_InitCameraGame(void)
{
	g_uzura->setCameraMatrix(MGetIdent());
	SetCameraViewMatrix(MGetIdent());
}


// ゲーム用のカメラの更新。
inline void Lua_UpdateCameraGame(float x, float y, float z)
{
	g_uzura->updateCameraMatrix(VGet(x, y, z));
// update ないで
//	SetCameraViewMatrix(MInverse(g_uzura->getCameraMatrix()));
}


// タスクシステム起動せよ。
inline void Lua_TaskUpdate(void)
{
g_uzura->getManagerProfile()->push("C++ Update", GetColor(255, 160, 0));
	g_managerProcessUpdate.update();
g_uzura->getManagerProfile()->pop(); // ("Update");
}


inline void Lua_TaskDraw(void)
{
g_uzura->getManagerProfile()->push("C++ Draw", GetColor(255, 160, 0));
	g_managerProcessDraw.update();
g_uzura->getManagerProfile()->pop(); // ("Draw");
}


// SetDrawScreen を呼ぶと、3D カメラの設定がパーになるため、再設定機能を追加
inline void Lua_SetDrawScreen(int h)
{
	SetDrawScreen(h);
	aen::gl::UpdateCamera__();
}


inline void Lua_PlaySE(const char* const soundId)
{
	PlaySE(soundId);
}


// 3D モデルを使用可能にする。
inline int Lua_GetModel(const char* const modelId)
{
	std::ostringstream oss;
	oss << "./data/models/" << modelId;
	auto o = GetModel(oss.str());


	if (o.isLoad()) {
		return (o.getHandle());
	}


cerr << "Lua_GetModelHandle -> ERROR >> NotFound File '" << oss.str() << "'" << endl;
AEN_STREAM_LOG_ERROR() << "Lua_GetModelHandle -> ERROR >> NotFound File '" << oss.str() << "'" << endl;
	return (-1);
}


// モデルをセット。
inline void Lua_SetPosition(int handle, float x, float y, float z)
{
	MV1SetPosition(handle, VGet(x, y, z));
}


// モデル出力
inline void Lua_DrawModel(int handle)
{
	MV1DrawModel(handle);
}


// モデル関連を Lua へ組み込む。
void SetScriptingModel(void)
{
	LuaObject a = g_luaState->GetGlobal("Model");
	if (!a.IsNil()) {
		// すでにあるし・・・
		return;
	}


	a = g_luaState->GetGlobals().CreateTable("Model");


	a.RegisterDirect("load", Lua_GetModel);
	a.RegisterDirect("setPos", Lua_SetPosition);

	a.RegisterDirect("draw", Lua_DrawModel);
	return;
}


// 敵キャラ関連
void Lua_AddEnemy(const char* const id, const char* const modelId, int x, int y , int z)
{
	AddEnemy(id, modelId, VGet(x, y, z), VGet(0, 0, 0));
}


// プレイヤー座標の更新。
void Lua_SetPlayerPos(int x, int y, int z)
{
	auto c = g_mapActors[HASH_KEY_PLAYER];
	MATRIX a = c->getMatrix();
	a.m[3][0] = x;
	a.m[3][1] = y;
	a.m[3][2] = z;


	VECTOR h = VGet(x, y, z);
	c->setPos(h);
	c->setMatrix(a);
	c->updateCollision(h);
	g_mapActors[HASH_KEY_PLAYER] = c;
}


// 自キャラのライフ取得
int Lua_GetPlayerHp(void)
{
	auto c = g_mapActors[HASH_KEY_PLAYER];
	return (c->getHp());
}


// 自キャラのライフ取得
void Lua_SetPlayerHp(int hp)
{
	auto c = g_mapActors[HASH_KEY_PLAYER];
	c->setHp(hp);
	g_mapActors[HASH_KEY_PLAYER] = c;
	return;
}


// ムービーを再生状態にする。
void Lua_PlayMovieToGraph(int handle)
{
	PlayMovieToGraph(handle);
}


// スコアの出力
void Lua_DrawScore(int x, int y)
{
	DrawString(x, y, "Score      : ", GetColor(0xFF, 0xFF, 0xFF));
	DrawString(x, y + 16, "High Score : ", GetColor(0xFF, 0xFF, 0xFF));

	DrawFormatString(x + 120, y, GetColor(0xFF, 0xFF, 0xFF), "%08d", GetScore());
	DrawFormatString(x + 120, y + 16, GetColor(0xFF, 0xFF, 0xFF), "%08d", GetScoreHigh());
}


// エフェクト作成。
void Lua_AddEffect(const char* const id, const char* const modelId, float x, float y, float z)
{
	AddEffect(id, modelId, VGet(x, y, z), VGet(0, 0, 0));
}


// サブシステムからデータがあるかな？
int Lua_GetSubsystemDataLength(void)
{
	return (GetNetWorkDataLength(g_uzura->getHandleSubsystem()));
}


// データを取得。
// データがあれば -1 以外。
char* GetSubsystemData(void)
{
static	char buf[255];
	NetWorkRecv(g_uzura->getHandleSubsystem(), buf, Lua_GetSubsystemDataLength());
	return (buf);
}


// Tweet を生成。
void Lua_AddTweet(const char* const id, float x, float y, float z)
{
	std::ostringstream oss;
	oss << "./data/subsystem/" << id << ".png";
	auto t = GetTexture(oss.str().c_str(), "");
	AddTweet("tweet", VGet(x, y, z), t);
}


// 仮想マシンに関数とか登録。
bool InitScripting(void)
{
	g_mapLuaScriptListener.clear();
	g_eventListener.reset();


	// run はパス指定
	g_luaState->GetGlobals().RegisterDirect("RunScript", RunScript);
	// do はファイル名指定
	g_luaState->GetGlobals().RegisterDirect("DoScript", DoScript);


	g_luaState->GetGlobals().RegisterDirect("Crash", Lua_Crash);


	// 基準線
	g_luaState->GetGlobals().RegisterDirect("DrawStandardLine3d", Lua_DrawStandardLine3d);
	// Now loading
	g_luaState->GetGlobals().RegisterDirect("DrawLoadingScene", DrawLoadingScene);


	// InitActor
	g_luaState->GetGlobals().RegisterDirect("InitActors", InitActors);
	// InitEnemy
	g_luaState->GetGlobals().RegisterDirect("InitEnemys", InitEnemys);
	// InitPlayer
	g_luaState->GetGlobals().RegisterDirect("InitPlayer", InitPlayer);
	g_luaState->GetGlobals().RegisterDirect("InitEffect", InitEffect);

	// タスクシステム。
	g_luaState->GetGlobals().RegisterDirect("TaskUpdate", Lua_TaskUpdate);
	g_luaState->GetGlobals().RegisterDirect("TaskDraw", Lua_TaskDraw);


	// 手抜きカメラ
	g_luaState->GetGlobals().RegisterDirect("UpdateCamera", Lua_UpdateCamera);
	g_luaState->GetGlobals().RegisterDirect("InitCamera", Lua_InitCamera);
	// ゲーム用のカメラ
	g_luaState->GetGlobals().RegisterDirect("UpdateCameraGame", Lua_UpdateCameraGame);
	g_luaState->GetGlobals().RegisterDirect("InitCameraGame", Lua_InitCameraGame);
	g_luaState->GetGlobals().RegisterDirect("GetVectorPlayer", GetVectorPlayer);
	g_luaState->GetGlobals().RegisterDirect("GetVectorCamera", GetVectorCamera);
	g_luaState->GetGlobals().RegisterDirect("GetPlayerRay", Lua_GetPlayerRay);


	g_luaState->GetGlobals().RegisterDirect("GetPlayerHp", Lua_GetPlayerHp);
	g_luaState->GetGlobals().RegisterDirect("SetPlayerHp", Lua_SetPlayerHp);


	// Effect
	g_luaState->GetGlobals().RegisterDirect("PlaySE", Lua_PlaySE);
	g_luaState->GetGlobals().RegisterDirect("AddEffect", Lua_AddEffect);


	// Subsystem のデータ関連処理。
	g_luaState->GetGlobals().RegisterDirect("GetSubsystemDataLength", Lua_GetSubsystemDataLength);
	g_luaState->GetGlobals().RegisterDirect("GetSubsystemData", GetSubsystemData);
	g_luaState->GetGlobals().RegisterDirect("AddTweet", Lua_AddTweet);


	// Matrix
	g_luaState->GetGlobals().RegisterDirect("GetMatrix", GetMatrix);
	g_luaState->GetGlobals().RegisterDirect("GetMatrixIdent", GetMatrixIdent);


	g_luaState->GetGlobals().RegisterDirect("GetRad", aen::util::GetRad);
	g_luaState->GetGlobals().RegisterDirect("GetDo", aen::util::GetDo);
	g_luaState->GetGlobals().RegisterDirect("GetRand", DxLib::GetRand);
	g_luaState->GetGlobals().RegisterDirect("Sleep", Lua_Sleep);
	g_luaState->GetGlobals().RegisterDirect("WaitTimer", DxLib::WaitTimer);

	g_luaState->GetGlobals().RegisterDirect("GetHash", GetHash);
	g_luaState->GetGlobals().RegisterDirect("SendEventN", SendEventN);
	g_luaState->GetGlobals().RegisterDirect("SendEventName", SendEventName);

	g_luaState->GetGlobals().RegisterDirect("Config", Config);
	g_luaState->GetGlobals().RegisterDirect("GetWindowCount", Lua_GetWindowCount);
	g_luaState->GetGlobals().RegisterDirect("GetNowCount", Lua_GetNowCount);
	g_luaState->GetGlobals().RegisterDirect("GetWindowFrameTime", Lua_GetWindowFps);
	g_luaState->GetGlobals().RegisterDirect("GetWindowFrameTime", Lua_GetWindowFrameTime);

	g_luaState->GetGlobals().RegisterDirect("DrawPolygon2d", aen::gl::DrawPolygon2d);
	g_luaState->GetGlobals().RegisterDirect("IsHitMouseBox", aen::gl::IsHitMouseBox);
	g_luaState->GetGlobals().RegisterDirect("DrawGrid", Lua_DrawGrid);
	g_luaState->GetGlobals().RegisterDirect("DrawGridCamera", Lua_DrawGridCamera);


	// 背景の円
	g_luaState->GetGlobals().RegisterDirect("GetBpm", Lua_GetBpm);
	g_luaState->GetGlobals().RegisterDirect("UpdateBpmSystem", Lua_UpdateBpmSystem);
	g_luaState->GetGlobals().RegisterDirect("InitBackEn", InitBackEn);
	g_luaState->GetGlobals().RegisterDirect("DrawBackEn", DrawBackEn);
	g_luaState->GetGlobals().RegisterDirect("GetScale", Lua_GetScale);
	g_luaState->GetGlobals().RegisterDirect("GetScaleBpm", Lua_GetScaleBpm);
	g_luaState->GetGlobals().RegisterDirect("DrawBackLine", DrawBackLine);
	g_luaState->GetGlobals().RegisterDirect("DrawBackBox", DrawBackBox);


	// ステージ関連
	g_luaState->GetGlobals().RegisterDirect("LoadStage", LoadStage);
	g_luaState->GetGlobals().RegisterDirect("ChangeStage", LoadStage);


	// Lua イベント関連
	g_luaState->GetGlobals().RegisterDirect("SendEvent", SendEvent);
	g_luaState->GetGlobals().RegisterDirect("AddListener", AddListener);
	g_luaState->GetGlobals().RegisterDirect("DelListener", DelListener);
	g_luaState->GetGlobals().RegisterDirect("DelListenerKeyAll", DelListenerKeyAll);
	g_luaState->GetGlobals().RegisterDirect("DelListenerAll", DelListenerAll);


	// 敵キャラを生成
	g_luaState->GetGlobals().RegisterDirect("AddEnemy", Lua_AddEnemy);
	g_luaState->GetGlobals().RegisterDirect("SetPlayerPos", Lua_SetPlayerPos);


	// DxLib wrapper
	g_luaState->GetGlobals().RegisterDirect("GetNowCount", Lua_GetNowCount);
	g_luaState->GetGlobals().RegisterDirect("GetColor", Lua_GetColor);
	g_luaState->GetGlobals().RegisterDirect("DrawLine", DrawLine);
	g_luaState->GetGlobals().RegisterDirect("DrawBox", DrawBox);
	g_luaState->GetGlobals().RegisterDirect("DrawCircle", DrawCircle);
	g_luaState->GetGlobals().RegisterDirect("DrawOval", DrawOval);
	g_luaState->GetGlobals().RegisterDirect("DrawTriangle", DrawTriangle);
	g_luaState->GetGlobals().RegisterDirect("GetPixel", DxLib::GetPixel);
	g_luaState->GetGlobals().RegisterDirect("DrawPixel", DxLib::DrawPixel);

	g_luaState->GetGlobals().RegisterDirect("DrawString", DrawString);
	g_luaState->GetGlobals().RegisterDirect("SetFontSize", SetFontSize);
	g_luaState->GetGlobals().RegisterDirect("SetFontThickness", SetFontThickness);


	g_luaState->GetGlobals().RegisterDirect("PlayMovieToGraph", Lua_PlayMovieToGraph);
	g_luaState->GetGlobals().RegisterDirect("SeekMovieToGraph", SeekMovieToGraph);
	g_luaState->GetGlobals().RegisterDirect("TellMovieToGraph", TellMovieToGraph);
	g_luaState->GetGlobals().RegisterDirect("GetMovieStateToGraph", GetMovieStateToGraph);


	// Window 動かそう。
	g_luaState->GetGlobals().RegisterDirect("SetWindowPosition", SetWindowPosition);


	// DxLib 画像。
	g_luaState->GetGlobals().RegisterDirect("MakeScreen", MakeScreen);
	g_luaState->GetGlobals().RegisterDirect("ClsDrawScreen", ClsDrawScreen);
	g_luaState->GetGlobals().RegisterDirect("ScreenFlip", ScreenFlip);
	g_luaState->GetGlobals().RegisterDirect("SetDrawScreen", Lua_SetDrawScreen);
	g_luaState->GetGlobals().SetInteger("DX_SCREEN_FRONT", DX_SCREEN_FRONT);
	g_luaState->GetGlobals().SetInteger("DX_SCREEN_BACK", DX_SCREEN_BACK);

	// フィルタ
	// ガウス
	g_luaState->GetGlobals().RegisterDirect("GraphFilterGF", Lua_GraphFilterGF);
	// 反転
	g_luaState->GetGlobals().RegisterDirect("GraphFilterInvert", Lua_GraphFilterInvert);


	g_luaState->GetGlobals().RegisterDirect("LoadGraph", Lua_LoadGraph);
	g_luaState->GetGlobals().RegisterDirect("DeleteGraph", Lua_DeleteGraph);
	g_luaState->GetGlobals().RegisterDirect("DrawGraph", DrawGraph);
	g_luaState->GetGlobals().RegisterDirect("DrawTurnGraph", DrawTurnGraph);
	g_luaState->GetGlobals().RegisterDirect("DrawExtendGraph", DrawExtendGraph);
	g_luaState->GetGlobals().RegisterDirect("DrawRotaGraph", DrawRotaGraph);
	g_luaState->GetGlobals().RegisterDirect("DrawRotaGraph2", DrawRotaGraph2);

	g_luaState->GetGlobals().RegisterDirect("SetDrawMode", SetDrawMode);
	g_luaState->GetGlobals().SetInteger("DX_DRAWMODE_NEAREST", DX_DRAWMODE_NEAREST);
	g_luaState->GetGlobals().SetInteger("DX_DRAWMODE_BILINEAR", DX_DRAWMODE_BILINEAR);
	g_luaState->GetGlobals().RegisterDirect("SetDrawBlendMode", SetDrawBlendMode);
	g_luaState->GetGlobals().SetInteger("DX_BLENDMODE_NOBLEND", DX_BLENDMODE_NOBLEND);
	g_luaState->GetGlobals().SetInteger("DX_BLENDMODE_ALPHA", DX_BLENDMODE_ALPHA);
	g_luaState->GetGlobals().SetInteger("DX_BLENDMODE_ADD", DX_BLENDMODE_ADD);
	g_luaState->GetGlobals().SetInteger("DX_BLENDMODE_SUB", DX_BLENDMODE_SUB);
	g_luaState->GetGlobals().SetInteger("DX_BLENDMODE_MULA", DX_BLENDMODE_MULA);
	g_luaState->GetGlobals().SetInteger("DX_BLENDMODE_INVSRC", DX_BLENDMODE_INVSRC);


	// スコア
	g_luaState->GetGlobals().RegisterDirect("SetScore", SetScore);
	g_luaState->GetGlobals().RegisterDirect("GetScore", GetScore);
	g_luaState->GetGlobals().RegisterDirect("GetScoreHigh", GetScoreHigh);
	g_luaState->GetGlobals().RegisterDirect("DrawScore", Lua_DrawScore);


	// Event 型の作成。
	__CreateMetaTableEvent();
	g_eventListener = boost::shared_ptr<aen::system::event::Listener>(new EventListenerLua);
	return (true);
}


// 初期化スクリプト実行後ステートマシンに追加設定
void ScriptingAddInfoState(void)
{
extern bool g_flagFullScreen;
	g_luaState->GetGlobals().SetInteger("WINDOW_W", WINDOW_W);
	g_luaState->GetGlobals().SetInteger("WINDOW_H", WINDOW_H);
	g_luaState->GetGlobals().SetInteger("WINDOW_MODE", g_flagFullScreen);


	// 各種機能を関連付ける。
	SetScriptingKeybord();
	SetScriptingJoypad();
	SetScriptingMouse();
	SetScriptingBlend();
	SetScriptingProfile();
	SetScriptingSound();
	SetScriptingModel();
}


// スクリプトを実行する。
bool RunScript(const char* const urlFile)
{
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_LOW) << "run lua script : " << urlFile << endl;
	while (true) {
		int top = g_luaState->GetTop();


		int ret = g_luaState->DoFile(urlFile);
		if (ret != 0) {
			char c;
			std::ostringstream oss;
			oss << "script error '" << urlFile << "' >> " << g_luaState->ToString(-1) << endl;
AEN_STREAM_LOG_ERROR() << oss.str();
			cout << oss.str();
			cout << "reload ? (y/n) > ";
			cin >> c;

			if (c == 'n') {
				return (false);
			}
		}
		else {
			break;
		}


		g_luaState->SetTop(top);
	}


	return (true);
}


// スクリプトをリロードする。
bool ReloadAllScript(const char* const param/* = "" */)
{
	if (!RunScript("./data/script/reload.lua")) {
		cerr << "Script Reload error." << endl;
		return (false);
	}


	return (true);
}


// イベント作成後送信ただし毎回ハッシュ値を計算するので重たい。
void SendEventName(const char* const name, double val, const char* const val2)
{
	AEN_ASSERT_KILL(name, "null pointer.");
	AEN_ASSERT_KILL(val2, "null pointer.");


	unsigned int hash = GetHash(name);
	SendEventN(hash, val, val2);
}


// イベント作成後送信ハッシュ値は事前に計算されたし
void SendEventN(unsigned int hash, double val, const char* const val2)
{
	AEN_ASSERT_KILL(val2, "null pointer.");


	aen::system::event::Event event;


	event.setType(hash);
	event.setValue(val);
	event.setStrValue(val2);


	g_uzura->getManagerEvent()->push(event);
}


// Uzura の設定を変更する
void Config(const char* const key, int value)
{
	AEN_ASSERT_KILL(key, "null pointer.");
	string k(key);


	if (k == "bpm") {
		g_uzura->setBpm(value);
	}
	else if (k == "fps") {
		g_uzura->getWindow()->setFps(value);
	}
	else if (k == "draw-profile") {
		g_uzura->setFlagShowProfile((value != 0 ? true : false));
	}
	else if (k == "debug") {
		g_uzura->setFlagDebug((value != 0 ? true : false));
	}
	else if (k == "sound-off") {
		// サウンド禁止 == 1
		aen::gl::sound::SetFlagNonUseSound((value ? true : false));
	}
	else if (k == "WINDOW_W") {
		WINDOW_W = value;
	}
	else if (k == "WINDOW_H") {
		WINDOW_H = value;
	}
	else if (k == "WINDOW_MODE") {
extern bool g_flagFullScreen; // main.cpp にあるよ。
		g_flagFullScreen = (value ? true : false);
	}
	else if (k == "keymap") {
		char buf[2][30] = {
			"jc-u3312sbk",
			"x360"
		};


		if (value > 1) {
			cerr << "key map index is unjust" << endl;
			AEN_STREAM_LOG_ERROR() << "key map index is unjust." << endl;
			return;
		}


cout << "change key map." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_HIGH) << "change key map : " << buf[value] << endl;
		// ゲームパッドのキーマップはデバッグだし・・・
		std::ostringstream oss;
		oss << "./data/keymap/" << buf[value] << ".txt";
		aen::gl::input::Joystick::loadKeyConfig(oss.str().c_str());
	}


}


// スクリプトを実行する
bool DoScript(const char* const sid)
{
	std::ostringstream oss;
	oss << "./data/script/" << sid << ".lua";
	return (RunScript(oss.str().c_str()));
}


//---------------------------------------------------------
// Lua の関数をイベントリスナーにする為の処理とか、
//---------------------------------------------------------


// Lua 宛のイベント処理。 -> Lua から Lua 宛のイベント
bool EventListenerLua::handleEvent(aen::system::event::Event* event)
{
// cout << "EventListenerLua::handleEvent -> OutputEvent listener : ";
// cout << event->getType() << "," << event->getValue() << "," << event->getStrValue() << endl;


	auto itr = g_mapLuaScriptListener.find(event->getType());
	if (itr == g_mapLuaScriptListener.end()) {
		// 表には登録されていなかった。
// cout << "LuaEventListner not find. : normal." << endl;
		return (false);
	}


	if (event->getPtr() != (void*)EventLua::MN_EVENT_LUA) {
		// 通常のイベントへ・・・
		// Lua では処理しない。
// cout << "Not LuaEvent object. : " << event->getType() << endl;
		return (false);
	}


	bool ret = false;
	for (auto itrV = itr->second.begin(); itrV != itr->second.end(); ++itrV) {
		// 登録されている・・・
		if (!itrV->IsFunction()) {
			// 関数じゃないだと・・・
			cerr << "Lua function map reg obj is not function..." << endl;
			AEN_STREAM_LOG_ERROR() << "Lua function map reg obj is not function..." << endl;
			return (false);
		}


		try {
			LuaFunction<bool> function((*itrV));
			EventLua* e = (EventLua*)event;
			if (!e->getLuaObject().IsNil()) {
				ret = function(e->getLuaObject());
			}
			else {
				ret = function();
			}
		}
		catch (LuaPlus::LuaException exp) {
			std::ostringstream oss;
			oss << "LuaEvent error : " << exp.GetErrorMessage() << endl;
			AEN_STREAM_LOG_ERROR() << oss.str();
			cerr << oss.str();
		}


		if (ret) {
			break;
		}
	}


	return (ret);
}


//  Lua 宛のイベントを送信。
void SendEvent(const char* const key, LuaObject obj)
{
	AEN_ASSERT_KILL(key, "null pointer.");
	auto e = boost::shared_ptr<EventLua>(new EventLua(obj));


	e->setType(GetHash(key));


	// 値は Lua の型で決める。
	if (obj.IsBoolean()) {
		// bool
		e->setValue(obj.GetBoolean() ? 1 : 0);
	}
	else if (obj.IsInteger()) {
		e->setValue(obj.GetInteger());
	}
	else if (obj.IsNumber()) {
		e->setValue(obj.GetNumber());
	}
	else if (obj.IsString()) {
		e->setStrValue(obj.GetString());
	}


	g_uzura->getManagerEvent()->push(e);
}


//  Lua 宛のイベントを送信。
void SendEventC(const char* const key)
{
	AEN_ASSERT_KILL(key, "null pointer.");
	auto e = boost::shared_ptr<EventLua>(new EventLua());


	e->setType(GetHash(key));
	e->setText(key);
	e->setValue(EventLua::MN_EVENT_LUA);


	g_uzura->getManagerEvent()->push(e);
}


//  Lua 宛のイベントを送信。
void SendEventCTrigger(const char* const key)
{
	AEN_ASSERT_KILL(key, "null pointer.");
	auto e = boost::shared_ptr<EventLua>(new EventLua());


	e->setType(GetHash(key));
	e->setText(key);
	e->setValue(EventLua::MN_EVENT_LUA);


	// 直に Lua のイベントを処理している回路へ叩きこむ。
	g_eventListener->handleEvent(e.get());
}


// イベントマネージャーに登録
bool AddListener(const char* const key, LuaObject obj)
{
	if (!obj.IsFunction()) {
cerr << "登録しようとした関数イベントリスナー型は、関数型ではありません。" << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_HIGH) << "登録しようとした関数イベントリスナー型は、関数型ではありません。" << endl;
AEN_STREAM_LOG_ERROR() << "登録しようとした関数イベントリスナー型は、関数型ではありません。" << endl;
		return (false);
	}


	unsigned int hash = GetHash(key);
	if (g_mapLuaScriptListener.find(hash) == g_mapLuaScriptListener.end()) {
		// 最初の空配列生成。
		g_mapLuaScriptListener[hash] = vector<LuaObject>();
	}


	// あとは追加ー
	g_mapLuaScriptListener[hash].push_back(obj);


	// マネージャーに登録されていなければ登録する。
	if (!g_uzura->getManagerEvent()->isReg(g_eventListener,  hash)) {
		g_uzura->getManagerEvent()->addListener(g_eventListener, hash);
	}


	return (true);
}


// イベントマネージャーからリスナーを消去
bool DelListener(const char* const key, LuaObject obj)
{
	if (!obj.IsFunction()) {
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_HIGH) << "登録しようとした関数イベントリスナー型は、関数型ではありません。" << endl;
		return (false);
	}


	unsigned int hash = GetHash(key);
	if (g_mapLuaScriptListener.find(hash) == g_mapLuaScriptListener.end()) {
		// そもそもないってことですね。
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_HIGH) << "消去しようとしたイベント '" << key << "' は登録されておりません。" << endl;
		return (false);
	}


	// あったら殺せ。
	bool ret = false;
	auto vec = g_mapLuaScriptListener[hash];
	for (auto itr = vec.begin(); itr != vec.end(); ++itr) {
		if ((*itr) == obj) {
			// 抹消じゃー
			vec.erase(itr);
			ret = true;
			break;
		}

	}


	if (!ret) {
		return (false);
	}


	// コピー戻して完了。
	// ついでにソートしておく・・・深い意味はない。
	std::sort(vec.begin(), vec.end());
	g_mapLuaScriptListener[hash] = vec;


	if (vec.empty()) {
		// マネージャーから切り離す。
		g_uzura->getManagerEvent()->delListener(g_eventListener, hash);
	}


	return (true);
}


// イベントマネージャーから指定したイベントを受信する全てのリスナーを消去
bool DelListenerKeyAll(const char* const key)
{
	unsigned int hash = GetHash(key);
	if (g_mapLuaScriptListener.find(hash) == g_mapLuaScriptListener.end()) {
		// そもそもないってことですね。
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_HIGH) << "消去しようとしたイベント '" << key << "' は登録されておりません。" << endl;
		return (false);
	}


	// 君は要らないリスナーなんだよ。
	g_mapLuaScriptListener[hash].clear();
	g_uzura->getManagerEvent()->delListener(g_eventListener, hash);
	return (true);
}


// イベントマネージャーすべての Lua イベントリスナーを抹消。
bool DelListenerAll(void)
{
	// 君は要らないリスナーなんだよ。
	auto itr = g_mapLuaScriptListener.begin();
	while (itr != g_mapLuaScriptListener.end()) {
		itr->second.clear();
		g_uzura->getManagerEvent()->delListener(g_eventListener, itr->first);
		++itr;
	}


	// 抹消。
	g_mapLuaScriptListener.clear();
	return (true);
}


//---------------------------------------------------------
// PSL
//---------------------------------------------------------


// Lua に関連付けている関数とかを、PSL に持ってくる。
inline void Psl_Crash(const char* const mes) { g_uzura->crash(mes); };
void PSL_SetUzuraFunc(PSL::PSLVM& vm)
{
	vm.addFunction("Crash", Psl_Crash);


	vm.addFunction("GetRad", aen::util::GetRad);
	vm.addFunction("GetDo", aen::util::GetDo);
	vm.addFunction("GetRand", DxLib::GetRand);
	vm.addFunction("WaitTimer", DxLib::WaitTimer);

	vm.addFunction("GetHash", GetHash);
	vm.addFunction("SendEventN", SendEventN);
	vm.addFunction("SendEventName", SendEventName);
}

