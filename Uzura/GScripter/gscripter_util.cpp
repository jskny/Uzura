/*
 *	ガーネット・スクリプター
 *	util
 *	2011 / 06 / 06
 *	jskny
*/

#include	<iostream>
#include	<algorithm>
#include	<string>
#include	<vector>

#include	"gscripter_def.h"
#include	"gscripter_util.h"

using namespace std;

namespace gscripter
{

// 大文字小文字を区別しないで文字を比較する関数
static bool compare_char(const char c1, const char c2)
{
	return ((tolower(c1) == tolower(c2)));
}

// 大文字小文字を区別しないで文字列を比較する。
// 同じであればtrueを、そうではなければfalseを返す。
bool compare_string(const std::string& s1, const std::string& s2)
{
	if (s1.length() != s2.length()) {
		return (false);
	}

	// 大文字小文字を区別しない比較を行う。
	return (std::equal(s1.begin(), s1.end(), s2.begin(), compare_char));
}

// 大文字/小文字を無視して部分文字列を検索する
// str:検索する文字列。substr:探す部分文字列
// 戻り値：一致した場合は先頭の反復子、一致しない場合はstr.end()
std::string::iterator	search_string(std::string& str, const std::string& substr)
{
	return (std::search(str.begin(), str.end(), substr.begin(), substr.end(), compare_char));
}

// 文字列の前後の空白を除き取り除いた文字列を返す。
std::string trim(const std::string& s)
{
	std::string ss;
	int len = 0;
	bool flsg_str = false;

	if (s.empty()) {
		return (s);
	}

	len = s.length();
	for (int i = 0; i < len; i++)
	{
		char c = s[i];

		// 0x08 == BS ( バックスペース )
		if (!flsg_str && (c == 0x08 || c == '\t' || c == ' ')) {
			continue;
		}
		else {
			ss += c;
			flsg_str = true;
		}
	}

	return (ss);
}

// 文字列の前の空白を除き取り除いた文字列を返す。
std::string trim_start(const std::string& s)
{
	std::string ss;
	int len = 0;
	bool flsg_space = false;

	if (s.empty()) {
		return (s);
	}

	len = s.length();
	for (int i = 0; i < len; i++)
	{
		char c = s[i];

		if (!flsg_space && (c == 0x08 || c == '\t' || c == ' ')) {
			continue;
		}
		else {
			ss += c;
			flsg_space = true;
		}
	}

	return (ss);
}

// 文字列の最後の改行を削除する
// OSによって改行文字が異なり、C++ のストリームI/Oの
// 改行文字がOSによって異なることがあるため
std::string remove_last_CRLF(const char* str)
{
	int len = 0;
	std::string buf(str);

	if (str == 0) {
GSCRIPTER_ERROR("文字列が不正です。");
		return (buf);
	}

	len = buf.length();
	if (len > 1) {
		if (buf[len-1] == 0x0D || buf[len-1] == 0x0A) {
			buf[len-1] = '\0';
		}
	}

	return (str);
}

std::string remove_last_CRLF(const std::string& str)
{
	int len = str.length();
	std::string buf(str);

	if (len > 1) {
		if (buf[len-1] == 0x0D || buf[len-1] == 0x0A) {
			buf[len-1] = '\0';
		}
	}

	return (buf);
}

// コメントを削除して削除した文字列を返す。
std::string remove_comment(char* str)
{
	int len;
	char chr = 0;
	std::string strbuf;
	bool flag_string = false;

	if (str == 0) {
GSCRIPTER_ERROR("文字列が不正です。");
		return (strbuf);
	}

	len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		chr = str[i];

		if ((chr == '"') || (chr == '\"')) {
			flag_string = true;
		}
		else if ((chr == '/') && (i < (len - 1)) && !flag_string)
		{
			if ((i + 1) < len) {
				if (str[i + 1] == '/') {
					break;
				}
			}
		}
		else if (chr == '#' && !flag_string) {
			break;
		}

		strbuf += chr;
	}

	return (strbuf);
}

std::string remove_comment(const std::string& str)
{
	int len = str.length();
	char chr = 0;
	string strbuf = "";
	bool flag_string = false;

	for (int i = 0; i < len; i++)
	{
		chr = str[i];

		if ((chr == '"') || (chr == '\"')) {
			flag_string = true;
		}
		else if ((chr == '/') & (i < (len - 1)) && !flag_string)
		{
			if ((i + 1) < len) {
				if (str[i + 1] == '/') {
					break;
				}
			}
		}
		else if (chr == '#' && !flag_string) {
			break;
		}

		strbuf += chr;
	}

	return (strbuf);
}

} // namespace gscripter

