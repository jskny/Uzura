/*
 *	Uzura
 *	スクリプティング統括
 *	2012 / 08 / 27
 *	jskny
*/


#pragma once

#ifndef __UZURA_SCRIPTING_INCLUDE_GUARD_20120827_37691_35__
#define __UZURA_SCRIPTING_INCLUDE_GUARD_20120827_37691_35__


#include	<aen_import_lua.h>
#include	"PSL/PSL.h"
#include	"GScripter/gscripter.h"


// HalCoco 全体で使用する仮想マシン
extern	LuaPlus::LuaStateOwner	g_luaState;


// 仮想マシンに関数とか登録。
extern	bool		InitScripting(void);
// 初期化スクリプト実行後ステートマシンに追加設定
extern	void		ScriptingAddInfoState(void);
// スクリプトを実行する。
extern	bool		RunScript(const char* const urlFile);
// スクリプトをリロードする。
extern	bool		ReloadAllScript(const char* const param = "");
// スクリプトを実行する
extern	bool		DoScript(const char* const sid);


// ハッシュ値を取得 ・・・ すべて小文字化する。
extern	unsigned int	GetHash(const char* const str);
// イベント作成後送信ただし毎回ハッシュ値を計算するので重たい。
extern	void		SendEventName(const char* const name, double val, const char* const val2);
// イベント作成後送信ハッシュ値は事前に計算されたし
extern	void		SendEventN(unsigned int hash, double val, const char* const val2);
//  Lua 宛のイベントを送信。
extern	void		SendEvent(const char* const key, LuaPlus::LuaObject obj);
//  Lua 宛のイベントを送信。
extern	void		SendEventC(const char* const key);
//  Lua 宛のイベントを送信。
extern	void		SendEventCTrigger(const char* const key);


// PSL
// Uzura の関数を関連付ける。
extern	void		PSL_SetUzuraFunc(PSL::PSLVM& vm);


#endif // __UZURA_SCRIPTING_INCLUDE_GUARD_20120827_37691_35__

