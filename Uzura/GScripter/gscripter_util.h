/*
 *	ガーネット・スクリプター
 *	util
 *	2011 / 06 / 06
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_UTIL_H_INCLUDE_GUARD_5734924_2435_33093_09873_352__
#define __GSCRIPTER_GSCRIPTER_UTIL_H_INCLUDE_GUARD_5734924_2435_33093_09873_352__

#include	<string>

namespace gscripter
{

// 大文字小文字を区別しないで文字列を比較する。
// 同じであればtrueを、そうではなければfalseを返す。
extern	bool		compare_string(const std::string& s1, const std::string& s2);
// 大文字/小文字を無視して部分文字列を検索する
// str:検索する文字列。substr:探す部分文字列
// 戻り値：一致した場合は先頭の反復子、一致しない場合はstr.end()
extern	std::string::iterator	search_string(std::string& str, const std::string& substr);
//--------------------------------------
// 文字列の前後の空白を除き取り除いた文字列を返す。
extern	std::string	trim(const std::string& s);
// 文字列の前の空白を除き取り除いた文字列を返す。
extern	std::string	trim_start(const std::string& s);
// 文字列の最後の改行を削除する
// OSによって改行文字が異なり、C++ のストリームI/Oの
// 改行文字がOSによって異なることがあるため
extern	std::string	remove_last_CRLF(const char* buf);
extern	std::string	remove_last_CRLF(const std::string& str);
// コメントを削除して削除した文字列を返す。
extern	std::string	remove_comment(const char* str);
extern	std::string	remove_comment(const std::string& str);

}


#endif // __GSCRIPTER_GSCRIPTER_UTIL_H_INCLUDE_GUARD_5734924_2435_33093_09873_352__

