/*
 *	ガーネット・スクリプター
 *	レキシカルアナライザ
 *	2011 / 06 / 06
 *	jskny
*/

#include	<iostream>
#include	<string>
#include	<vector>

#include	<assert.h>

#include	"gscripter_def.h"
#include	"gscripter_loader.h"
#include	"gscripter_lex.h"
#include	"gscripter_util.h"

using namespace std;

namespace gscripter
{
namespace lex
{

//---------------------------------------------------------
// レキシカルアナライザ例外クラス
//---------------------------------------------------------

class exception_lexer
{
public:
	exception_lexer(
		const std::string& s_text,
		const std::string& s_line,
		const bool s_flag_stop) :
		text(s_text),
		line(s_line),
		flag_stop(s_flag_stop)
	{
	}

	std::string	text; // エラー内容等
	std::string	line;
	bool		flag_stop; // レキシカルアナライズを止めるか否か、 止める true
};

//---------------------------------------------------------
// lex_token
//---------------------------------------------------------

token::token() :
	value(0.0), type(GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL),
	str_value(""), name("")
{
}

token::token(const GSCRIPTER_TOKEN_TYPE t) :
	value(0.0), type(t),
	str_value(""), name("")
{
}

void token::reset(void)
{
	this->value = 0.0;
	this->type = GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL;
	this->name = "";
	this->str_value = "";

	return;
}

//---------------------------------------------------------
// レキシカルアナライザ
//---------------------------------------------------------

// 文字列の前後の"を除き取り除いた文字列を返す。
extern	std::string	strip_str(const std::string& s);

// 引数 c が数値なら true 数値以外なら false
static bool is_number(const char &c)
{
	// unsigned が char だから。
	if ((unsigned)(c - '0') <= (unsigned)10) {
		return (true);
	}

	return (false);
}

// 引数 c が英字なら true 英字以外なら false
static bool is_alphabet(const char &c)
{
	// unsigned が char だから。
	//小文字
	if ((unsigned)(c - 'a') <= (unsigned)'z') {
		return (true);
	}

	//大文字
	if ((unsigned)(c - 'A') <= (unsigned)'Z') {
		return (true);
	}

	return (false);
}

// 引数 c が ( か ) なら true それ以外なら false
static bool is_paren(const char &c)
{
	if (c == '(' || c == ')') {
		return (true);
	}

	return (false);
}

// 引数 c が演算子なら true 演算子でなければ false
static bool is_oparator(const char c)
{
	if (c == '-' || c == '+') {
		return (true);
	}
	else if (c == '*' || c == '/' || c == '%') {
		return (true);
	}
	else if (c == '<' || c == '>') {
		return (true);
	}
	else if (c == '!' || c == '=') {
	        return (true);
	}
	else if (c == ':' || c == ';') {
	        return (true);
	}
	else if (c == '{' || c == '}') {
		return (true);
	}
	else if (c == ',' || c == '.') {
		return (true);
	}

	return (false);
}

// トークンが文の始まりのトークンなら true を返しそれ以外なら false を返す。
static bool is_start_token(const token& tok)
{
	switch (tok.type)
	{
		case GSCRIPTER_TOKEN_TYPE_START:
		case GSCRIPTER_TOKEN_TYPE_ASSIGN:
		case GSCRIPTER_TOKEN_TYPE_L_PAREN:
		case GSCRIPTER_TOKEN_TYPE_COMMA:
			return (true);
		default:
			return (false);
	}

	return (false);
}

// トークンがコマンドなら true それ以外なら false
static bool inline is_command_or_assign(const token& tok)
{
	if ((tok.type > GSCRIPTER_TOKEN_TYPE_COMMAND_START && tok.type < GSCRIPTER_TOKEN_TYPE_COMMAND_END)
		|| tok.type == GSCRIPTER_TOKEN_TYPE_ASSIGN) {
		return (true);
	}

	return (false);
}

//---------------------------------------------------------


// 文字列の前後の"を除き取り除いた文字列を返す。
std::string strip_str(const std::string& s)
{
	int len = 0;
	std::string str;

	if (s.empty()) {
throw exception_lexer("文字列が不正です。 検証する文字列が empty() == true です。", "can't use service.", true);
		return (s);
	}

	len = s.length();
	if (len <= 0) {
throw exception_lexer("文字列が不正です。 そもそも、これは文字列ですか？", "can't use service.", true);
		return (str);
	}

	if (s[0] != '\"' || s[len-1] != '\"') {
		// 正しくない つーか、文字列が不正。
throw exception_lexer("文字列が不正です。 \" を付けるのを忘れていませんか？", "can't use service.", true);
		str = "";
	}
	else {
		if (len >= 2) { // length が返すのは、 size と同じで + 1だから。
			str = s.substr(1, len-2);
		}
		else {
throw exception_lexer("文字列が不正です。 \" を付けるのを忘れていませんか？", "can't use service.", true);
			str = "";
		}
	}

	return (str);
}

// 先頭のトークンを取り出して返す。 要素は一つ減る。
token get_next_token(std::vector<token>* vector_tok)
{
	token ret_tok;

	if (!vector_tok) {
		GSCRIPTER_ERROR("0 pointer error.");
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN;
		return (ret_tok);
	}

	if (vector_tok->empty()) {
		// トークンは存在しない。
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN;
		return (ret_tok);
	}

	ret_tok = vector_tok->at(0);
	vector_tok->erase(vector_tok->begin());

	return (ret_tok);
}

// トークンを戻す
void back_token(const token& tok, std::vector<token>* vector_tok)
{
	if (!vector_tok) {
		GSCRIPTER_ERROR("0 pointer error.");
		return;
	}

	vector_tok->insert(vector_tok->begin(), tok);

	return;
}

// N番目のトークンを取得する(N=0,1,2...)
token get_token_n(const unsigned int n, const std::vector<token>& vector_tok)
{
	if ((int)(vector_tok.size()) < (n + 1)) {
		// トークンは存在しない。
		token ret_tok;
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN;
		return (ret_tok);
	}

	return (vector_tok.at(n));
}

// トークンのリストを取得する
unsigned int get_token_list(const std::string& s, std::vector<token>* vector_tok)
{
	token tok, buf_tok;
	std::string buf_string;

	assert(vector_tok);

	// スタート・トークン
	buf_tok.type = GSCRIPTER_TOKEN_TYPE_START;
	vector_tok->clear();

	// 最初の文字列をトークンに切り分ける
	tok = lex_token(s, buf_tok, buf_string);
	while(tok.type != GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN)
	{
		vector_tok->push_back(tok);
		buf_tok = tok;
		try {
			tok = lex_token("", buf_tok, buf_string);
		}
		catch (exception_lexer exp)
		{
			GSCRIPTER_ERROR("lexer error.");
			cerr << exp.text << endl;
			cerr << exp.line << endl;
			if (exp.flag_stop) {
				throw exp;
			}
		}
	}

	return (vector_tok->size());
}

// トークンを切り離す。
token lex_token(const std::string& script_line, token& old_token, std::string& old_script_line)
{
	token ret_tok;
	string buf_str_line;

	ret_tok.type = GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN;

	// 前のトークンが文の開始のトークンだった時は最初だった情報を消す。
	if (is_command_or_assign(old_token)) {
		old_token.type = GSCRIPTER_TOKEN_TYPE_START;
	}

	// 改行を消す。
	buf_str_line = remove_comment(script_line);
	// 空白を消し
	buf_str_line = trim_start(buf_str_line);

	// 古いのに追加する。
	old_script_line += buf_str_line;
	// 空白削除。
	buf_str_line = trim(old_script_line);
	old_script_line = buf_str_line;

	// そして古いのが空ならもう何もない。
	if (old_script_line.length() <= 0) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN;
		return (ret_tok);
	}

	// 現在の文字位置
	int pos = 0;
	// 先頭の文字
	char c1 = old_script_line[pos++];
	char c2 = 0;
	char c3 = 0;

	if (old_script_line.length() > 1) {
		c2 = old_script_line[pos];
		if (old_script_line.length() > 2) {
			c3 = old_script_line[pos + 1];
		}
	}

	if (c1 == '=' && c2 == '=') {
		old_script_line = old_script_line.substr(2, old_script_line.length() - 2);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_EQUAL;
		return (ret_tok);
	}
	else if (c1 == '!' && c2 == '=') {
		old_script_line = old_script_line.substr(2, old_script_line.length() - 2);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_NOT_EQUAL;
		return (ret_tok);
	}
	else if (c1 == '<' && c2 == '=') {
		old_script_line = old_script_line.substr(2, old_script_line.length() - 2);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_SMALLER_EQUAL;
		return (ret_tok);
	}
	else if (c1 == '>' && c2 == '=') {
		old_script_line = old_script_line.substr(2, old_script_line.length() - 2);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_GREATER_EQUAL;
		return (ret_tok);
	}

	// 単項演算子
	if (c1 == '+' && !(is_start_token(old_token))) {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_PLUS;
		return (ret_tok);
	}
	else if (c1 == '-' && !(is_start_token(old_token))) {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_MINUS;
		return (ret_tok);
	}
	else if (c1 == '*') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_MULT;
		return (ret_tok);
	}
	else if (c1 == '/') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_DIVIDE;
		return (ret_tok);
	}
	else if (c1 == '%') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_REMAINDER;
		return (ret_tok);
	}
	else if (c1 == '<') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_SMALLER;
		return (ret_tok);
	}
	else if (c1 == '>') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_GREATER;
		return (ret_tok);
	}
	else if (c1 == '=') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_ASSIGN;
		return (ret_tok);
	}
	else if (c1 == '(') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_L_PAREN;
		return (ret_tok);
	}
	else if (c1 == ')') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_R_PAREN;
		return (ret_tok);
	}
	else if (c1 == '{') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_L_BLAISE;
		return (ret_tok);
	}
	else if (c1 == '}') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_R_BLAISE;
		return (ret_tok);
	}
	else if (c1 == '.') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_PERIOD;
		return (ret_tok);
	}
	else if (c1 == ',') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_COMMA;
		return (ret_tok);
	}
	else if (c1 == ';') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_SEMICOLON;
		return (ret_tok);
	}
	else if (c1 == ':') {
		old_script_line = old_script_line.substr(1, old_script_line.length() - 1);
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_COLON;
		return (ret_tok);
	}

	// トークン文字列保管用。
	string buf_token_str;

	// -符号がマイナス演算子か -50 の - かどうか判断する。
	if (c1 == '-' && is_alphabet(c2))
	{
		buf_token_str += c1;
		while (true)
		{
			if ((int)old_script_line.length() == pos) {
				// 解析終了。
				old_script_line = "";
				break;
			}

			char c = old_script_line[pos++];
			if (c == ' ' || is_oparator(c) || is_paren(c) || c== ',' || c == '\t') {
				pos--;
				int len = old_script_line.length() - pos;
				old_script_line = old_script_line.substr(pos, len);
				break;
			}
			else {
				buf_token_str += c;
			}
		}

		ret_tok.type = GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL;
		ret_tok.name = buf_token_str;
		ret_tok.str_value = buf_token_str;
		return (ret_tok);
	}

	if (c1 == '+' || c1 == '-' || c1 == '.' || is_number(c1))
	{
		buf_token_str = c1;
		int count_period = 0;

		while (true)
		{
			// 解析終了
			if ((int)(old_script_line.length()) == pos) {
				old_script_line = "";
				ret_tok.type = GSCRIPTER_TOKEN_TYPE_NUMBER_VALUE;
				ret_tok.value = atof(old_script_line.c_str());
				return (ret_tok);
			}

			char c = old_script_line[pos++];
			if (c == '.' || is_number(c))
			{
				if (count_period > 2)
				{
// GSCRIPTER_ERROR("数値にピリオドが一つ以上あります。");
throw exception_lexer("実数リテラル(数値)にピリオドが一つ以上あります。", old_script_line, false);
					ret_tok.type = GSCRIPTER_TOKEN_TYPE_INVALID;
					return (ret_tok);
				}
				else {
					buf_token_str += c;
					if (c == '.') {
						count_period++;
					}
				}
			}
			else {
				pos--;
				int len = old_script_line.length() - pos;
				old_script_line = old_script_line.substr(pos, len);
				ret_tok.type = GSCRIPTER_TOKEN_TYPE_NUMBER_VALUE;
				ret_tok.value = std::atof(buf_token_str.c_str());
				return (ret_tok);
			}
		} // while (true)
	} // if (c1 == '+' || c1 == '-' || c1 == '.' || IsNumber(c1))

	// 文字列かどうか 0x22 == \"
	buf_token_str = c1;
	// 文字列の時 true それ以外 false
	bool flag_string = false;;
	if (c1 == 0x22 || c1 == '\"') {
		flag_string = true;
	}

	while (true)
	{
		if ((int)(old_script_line.length()) == pos) {
			// 解析終わり。
			old_script_line = "";
			break;
		}
		char c = old_script_line[pos++];
		if (flag_string)
		{
			// \" の後の処理。
			if (c == '\"' || c == 0x22) {
				buf_token_str += c;
				int len = old_script_line.length() - pos;
				old_script_line = old_script_line.substr(pos, len);
				flag_string = false;
				break;
			}
			else {
				buf_token_str += c;
			}
		}
		else {
			if (c == ' ' || c == '\t' || is_oparator(c) || is_paren(c) || c == ',')
			{
				pos--;
				int len = old_script_line.length() - pos;
				old_script_line = old_script_line.substr(pos, len);
				break;
			}
			else {
				buf_token_str += c;
			}
		}
	}//  while (true)

	if (buf_token_str[0] == '\"') {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_STRING_VALUE;
		try {
		ret_tok.str_value = strip_str(buf_token_str);
		}
		catch (exception_lexer exp)
		{
			GSCRIPTER_ERROR("lexer error.");
			cerr << exp.text << endl;
			cerr << exp.line << endl;
			if (exp.flag_stop) {
				throw exp;
			}
		}
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "end")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_END;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "call")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_CALL;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "if")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_IF;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "endif")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_IF_END;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "label")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_LABEL;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "goto")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_GOTO;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "gosub")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_GOSUB;
		return (ret_tok);
	}
	else if (compare_string(buf_token_str, "return")) {
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_RETURN;
		return (ret_tok);
	}
	else {
		// ここまで来ているのなら
		// このトークンはシンボルである。
		ret_tok.type = GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL;
		ret_tok.name = buf_token_str;
		ret_tok.str_value = buf_token_str;
		return (ret_tok);
	}

	return (ret_tok);
}

//---------------------------------------------------------

static std::string gscripter_token_text[] = {
	"damy.",
	"START",
	"INVALID", // 無効
	"NOMORE_TOKEN", // なし、読み込み終了時など、
	// 未定義 関数名など、 要は、変数は $XXX なので、すぐわかる。 つまり、変数名以外の何か。( 関数名、ラベル名 )
	// だったはずが、$ で文字区切りとかしてなかったので、結局変数はこれ。
	"UNKNOWN_SYMBOL",

// 定数の開始 ( スクリプトに直接書かれた値とか、 )
"START_VALUE",
	"NUMBER_VALUE", // 0.1 or 324 or 1.3628 など、
	"STRING_VALUE", // "あ" or "text" など、

// 変数の開始
"START_VARIABLE",
	"NUMBER_VARIABLE", // 数値変数   ( double )
	"STRING_VARIABLE", // 文字列変数 ( std::string )

// 演算子の開始
"START_OPERAND",
	"L_PAREN", // (
	"R_PAREN", // )

	"L_BLAISE", // {
	"R_BLAISE", // }

	"COMMA", // カンマ ,
	"PERIOD", // ピリオド . ( 文字列統合演算子 )
	"SEMICOLON", // セミコロン ;
	"COLON", // コロン :

// 演算子の開始２
"START_OPERAND_TWO",
	"PLUS", // +
	"MINUS", // -
	"MULT", // *
	"DIVIDE", // /
	"REMAINDER", // %
 
// 演算子の開始３
"START_OPERAND_THREE",
	"SMALLER", // <
	"SMALLER_EQUAL", // <=
	"GREATER", // >
	"GREATER_EQUAL", // >=
	"EQUAL", // ==
	"NOT_EQUAL", // !=
	"ASSIGN", // = 代入

// コマンドの開始
"COMMAND_START",
	"END",
	"CALL",

	"IF",
	"ENDIF",

	"LABEL",
	"GOTO",

	"GOSUB",
	"RETURN",

"COMMAND_END",

	"TOKEN_TYPE_END", // type list end.
	"damy."
};

// 出力
void print_token(const token& a)
{
	cout << "type  : " << a.type << "," << gscripter_token_text[a.type - GSCRIPTER_TOKEN_TYPE_START + 1] << endl;
	cout << "value : " << a.value << endl;
	cout << "str   : " << a.str_value << endl;
	cout << "name  : " << a.name << endl;
	return;
}

//---------------------------------------------------------
// 字解析機
//---------------------------------------------------------

lex::lex()
{
	this->m_loader.clear();
	this->m_vector_token.clear();
}

void lex::set(loader* s_loader)
{
	if (!s_loader) {
GSCRIPTER_ERROR("null pointer.");
		return;
	}

	if (this->m_loader.is_load()) {
		this->m_loader.clear();
	}

	this->m_loader = *s_loader;
	return;
}

bool lex::is_set_loader(void)
{
	return (this->m_loader.is_load());
}

// レキシカルアナライズ
bool lex::lexical_analysis(void)
{
	std::string buf_str;
	std::vector<token> buf_vector;

	if (!this->is_set_loader()) {
		GSCRIPTER_ERROR("lex -> lexical_analysis -> ファイルが読み込まれていません。");
		return (false);
	}

	// ファイルの中身がないのは、エラーではないと思う。
	if (this->m_loader.get_line_max() <= 0) {
		GSCRIPTER_DEBUG("ファイルの中身が空です。");
		return (true);
	}

	if (!this->m_vector_token.empty()) {
		this->m_vector_token.clear();
	}

GSCRIPTER_DEBUG("start lex::lexical_analysis : lexical analysis");
GSCRIPTER_DEBUG("line : script");
	for (int i = 0; i < this->m_loader.get_line_max(); i++)
	{
		buf_str = this->m_loader.get_line(i);
		try {
		get_token_list(buf_str, &buf_vector);
		}
		catch (exception_lexer exp)
		{
			if (exp.flag_stop) {
				return (false);
			}
		}
#if GSCRIPTER_FLAG_DEBUG == 1
cout << i << " : " << buf_str << endl;
#endif
		this->m_vector_token.push_back(buf_vector);
		buf_vector.clear();
	}
GSCRIPTER_DEBUG("finish lex::lexical_analysis : lexical analysis");

	return (true);
}

void lex::output(void)
{
	std::vector<std::vector<token>>::iterator itr;
	std::vector<token>::iterator itr2;
	for (itr = this->m_vector_token.begin(); itr != this->m_vector_token.end(); itr++) {
		for (itr2 = itr->begin(); itr2 != itr->end(); itr2++) {
			print_token(*itr2);
		}
	}

	return;
}

void lex::reset(void)
{
	this->m_loader.clear();
	this->m_vector_token.clear();
}

} // namespace lex
} // namespace gscripter

