/*
 *	ガーネット・スクリプター
 *	レキシカルアナライザ
 *	2011 / 06 / 06
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_LEX_H_INCLUDE_GUARD_56_2238_666_17376_132__
#define __GSCRIPTER_GSCRIPTER_LEX_H_INCLUDE_GUARD_56_2238_666_17376_132__

#include	<string>
#include	<vector>

#include	"gscripter_loader.h"

namespace gscripter
{
namespace lex
{

//---------------------------------------------------------

class token;
class lex; // レキシカルアナライザ、長いので省略

//---------------------------------------------------------

// トークン情報
enum GSCRIPTER_TOKEN_TYPE
{
	GSCRIPTER_TOKEN_TYPE_START = 1000,
	GSCRIPTER_TOKEN_TYPE_INVALID, // 無効
	GSCRIPTER_TOKEN_TYPE_NOMORE_TOKEN, // なし、読み込み終了時など、
	// 未定義 関数名など、 要は、変数は $XXX なので、すぐわかる。 つまり、変数名以外の何か。( 関数名、ラベル名 )
	// だったはずが、$ で文字区切りとかしてなかったので、結局変数はこれ。
	GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL,

// 定数の開始 ( スクリプトに直接書かれた値とか、 )
GSCRIPTER_TOKEN_TYPE_START_VALUE,
	GSCRIPTER_TOKEN_TYPE_NUMBER_VALUE, // 0.1 or 324 or 1.3628 など、
	GSCRIPTER_TOKEN_TYPE_STRING_VALUE, // "あ" or "text" など、

// 変数の開始
GSCRIPTER_TOKEN_TYPE_START_VARIABLE,
	GSCRIPTER_TOKEN_TYPE_NUMBER_VARIABLE, // 数値変数   ( double )
	GSCRIPTER_TOKEN_TYPE_STRING_VARIABLE, // 文字列変数 ( std::string )

// 演算子の開始
GSCRIPTER_TOKEN_TYPE_START_OPERAND,
	GSCRIPTER_TOKEN_TYPE_L_PAREN, // (
	GSCRIPTER_TOKEN_TYPE_R_PAREN, // )

	GSCRIPTER_TOKEN_TYPE_L_BLAISE, // {
	GSCRIPTER_TOKEN_TYPE_R_BLAISE, // }

	GSCRIPTER_TOKEN_TYPE_COMMA, // カンマ ,
	GSCRIPTER_TOKEN_TYPE_PERIOD, // ピリオド . ( 文字列統合演算子 )
	GSCRIPTER_TOKEN_TYPE_SEMICOLON, // セミコロン ;
	GSCRIPTER_TOKEN_TYPE_COLON, // コロン :

// 演算子の開始２
GSCRIPTER_TOKEN_TYPE_START_OPERAND_TWO,
	GSCRIPTER_TOKEN_TYPE_PLUS, // +
	GSCRIPTER_TOKEN_TYPE_MINUS, // -
	GSCRIPTER_TOKEN_TYPE_MULT, // *
	GSCRIPTER_TOKEN_TYPE_DIVIDE, // /
	GSCRIPTER_TOKEN_TYPE_REMAINDER, // %
 
// 演算子の開始３
GSCRIPTER_TOKEN_TYPE_START_OPERAND_THREE,
	GSCRIPTER_TOKEN_TYPE_SMALLER, // <
	GSCRIPTER_TOKEN_TYPE_SMALLER_EQUAL, // <=
	GSCRIPTER_TOKEN_TYPE_GREATER, // >
	GSCRIPTER_TOKEN_TYPE_GREATER_EQUAL, // >=
	GSCRIPTER_TOKEN_TYPE_EQUAL, // ==
	GSCRIPTER_TOKEN_TYPE_NOT_EQUAL, // !=
	GSCRIPTER_TOKEN_TYPE_ASSIGN, // = 代入

// コマンドの開始
GSCRIPTER_TOKEN_TYPE_COMMAND_START,
	GSCRIPTER_TOKEN_TYPE_END,
	GSCRIPTER_TOKEN_TYPE_CALL,

	GSCRIPTER_TOKEN_TYPE_IF,
	GSCRIPTER_TOKEN_TYPE_IF_END,

	GSCRIPTER_TOKEN_TYPE_LABEL,
	GSCRIPTER_TOKEN_TYPE_GOTO,

	GSCRIPTER_TOKEN_TYPE_GOSUB,
	GSCRIPTER_TOKEN_TYPE_RETURN,

GSCRIPTER_TOKEN_TYPE_COMMAND_END,

	GSCRIPTER_TOKEN_TYPE_TOKEN_TYPE_END // type list end.
};

//---------------------------------------------------------

// トークン
class token
{
public:
	double			value;
	std::string		str_value;
	GSCRIPTER_TOKEN_TYPE	type;
	std::string		name;
public:
			token();
	explicit	token(const GSCRIPTER_TOKEN_TYPE t);

	void		reset(void);
};

// 字解析機
class lex
{
public:
	void		set(loader* s_loader);
	bool		is_set_loader(void);

	// 字解析 成功 true 失敗 false
	bool		lexical_analysis(void);

	// 中身を標準出力に出力する。
	void		output(void);

	// 初期化
	void		clear(void) { this->reset(); };
	void		reset(void);

	// 解析結果が入る vector のサイズ
unsigned long int	get_vector_size(void) { return (this->m_vector_token.size()); };

	// 最終手段
std::vector<std::vector<token>>	const	get_vector_token(void) { return (this->m_vector_token); };

	lex();

private:
	loader		m_loader;
std::vector<std::vector<token>>		m_vector_token;

};

//---------------------------------------------------------
// 先頭のトークンを取り出して返す。 要素は一つ減る。
extern	token			get_next_token(std::vector<token>* vector_tok);
// トークンを戻す
extern	void			back_token(const token& tok, std::vector<token>* vector_tok);
// N番目のトークンを取得する(N=0,1,2...)
extern	token			get_token_n(const unsigned int n, const std::vector<token>& vector_tok);
// トークンのリストを取得する
extern	unsigned int		get_token_list(const std::string& s, std::vector<token>* vector_tok);
// トークンを切り離す。
extern	token			lex_token(const std::string& script_line, token& old_token, std::string& old_script_line);
// 出力
extern	void			print_token(const token& a);
//---------------------------------------------------------

} // namespace lex
} // namespace gscripter


#endif // __GSCRIPTER_GSCRIPTER_LEX_H_INCLUDE_GUARD_56_2238_666_17376_132__

