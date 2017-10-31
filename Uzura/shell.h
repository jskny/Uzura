/*
 *	Uzura
 *	カラを破ってもう一度
 *	2012 / 11 / 13
 *	jskny
*/


#pragma once

#ifndef __UZURA_SHELL_INCLUDE_GUARD_39813_0_137629183_1__
#define __UZURA_SHELL_INCLUDE_GUARD_39813_0_137629183_1__


#include	<string>
#include	<vector>


// 初期化。
extern	void		ShellReset(void);
// シェルが動いていれば true を返す。というか、exit 入力時に false になるだけ。
extern	bool		IsRunningShell(void);
// シェル稼働フラグ・・・
extern	void		SetFlagRunningShell(bool flag);

// 文字列を追加。
extern	void		ShellAddLine(const char* const line);


// 出力
extern	void		ShellDraw(void);
// 入力
extern	void		ShellInput(void);


// 関数の登録。
// n は参照していた時の i, args は引数。返却値は、内部で処理した引数数。エラーは -1 とかで、
extern	void		ShellAttachFunction(std::string name, int (*pFunc) (int n, const std::vector<std::string>& args));


namespace shell
{
inline	void		reset(void) { ShellReset(); };
inline	bool		isRunnning(void) { IsRunningShell(); };
inline	void		setFlagRunnning(bool f) { SetFlagRunningShell(f); };

inline	void		add(const char* const line) { ShellAddLine(line); };
inline	void		draw(void) { ShellDraw(); };
inline	void		input(void) { ShellInput(); };
inline	void		attach(std::string name, int (*pFunc) (int n, const std::vector<std::string>& args)) { ShellAttachFunction(name, pFunc); };

} // namespace shell


#endif // __UZURA_SHELL_INCLUDE_GUARD_39813_0_137629183_1__

