/*
 *	ガーネット・スクリプター
 *	コンパイラ
 *	2011 / 06 / 07
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_COMPILER_H_INCLUDE_GUARD_201139_4645_704575_33__
#define __GSCRIPTER_GSCRIPTER_COMPILER_H_INCLUDE_GUARD_201139_4645_704575_33__

#include	<string>
#include	<vector>
#include	<stack>
#include	<map>

#include	"gscripter_lex.h"

namespace gscripter
{
namespace compiler
{

class code;
class compiler;

//---------------------------------------------------------
extern	void			factor(compiler* this_compiler);
extern	void			term(compiler* this_compiler);
extern	void			expression(compiler* this_compiler);
extern	void			condition_calculation(compiler* this_compiler);
extern	void			statement(compiler* this_compiler);
extern	bool			create_statement_goto(compiler* this_compiler);
//---------------------------------------------------------

enum GSCRIPTER_OBJECT_CODE
{
	// 無効
	GSCRIPTER_CODE_INVALID = 800,
// コマンド
GSCRIPTER_CODE_START_COMMAND, 
	// 数値
	GSCRIPTER_CODE_VALUE,
	// 文字列
	GSCRIPTER_CODE_STRING,
	// 変数 ( 数値、文字列 VMで使う )
	GSCRIPTER_CODE_VALUE_VARIABLE,
	GSCRIPTER_CODE_STRING_VARIABLE,
	// シンボル
	GSCRIPTER_CODE_SYMBOL,
	// 数値のマイナス化命令 ( value * -1 )
	GSCRIPTER_CODE_MINUS_CONVERT,

	// IF
	GSCRIPTER_CODE_IF,
	// IF BLOCK END.
	GSCRIPTER_CODE_IF_END,

	// LABEL and GOTO
	GSCRIPTER_CODE_LABEL,
	GSCRIPTER_CODE_GOTO,

	// gosub, return
	GSCRIPTER_CODE_GOSUB,
	GSCRIPTER_CODE_RETURN,

	// ジャンプ
	GSCRIPTER_CODE_JAMP,
	// 条件ジャンプ
	GSCRIPTER_CODE_JAMPC,
	// CALL 関数呼び出し
	GSCRIPTER_CODE_CALL,
	// 関数の引数の開始
	GSCRIPTER_CODE_CALL_FUNC_ARGV_START,
	// END
	GSCRIPTER_CODE_END,
	// nop (何もしない)
	GSCRIPTER_CODE_NOP,

// 演算子
GSCRIPTER_CODE_START_OPERAND, 
	GSCRIPTER_CODE_ADD, // +
	GSCRIPTER_CODE_SUB, // - 
	GSCRIPTER_CODE_MULT, // *
	GSCRIPTER_CODE_DIVIDE, // /
	GSCRIPTER_CODE_REMAINDER, // %
	GSCRIPTER_CODE_SMALLER, // < 
	GSCRIPTER_CODE_SMALLER_EQUAL, // <=
	GSCRIPTER_CODE_GREATER, // >
	GSCRIPTER_CODE_GREATER_EQUAL, // >=
	GSCRIPTER_CODE_EQUAL, // ==
	GSCRIPTER_CODE_NOT_EQUAL, // !=
	GSCRIPTER_CODE_ASSIGN, // = (代入)

	GSCRIPTER_CODE_STR_ADD, // . 文字列結合演算子

};

// 実行されるコード
class code
{
public:
	double		value;
	std::string	str_value;
GSCRIPTER_OBJECT_CODE	type;

	code() :
		value(0.0),
		str_value(""),
		type(GSCRIPTER_CODE_NOP)
	{
	}

};

// コンパイラ
class compiler
{
friend	void		factor(compiler* this_compiler);
friend	void		term(compiler* this_compiler);
friend	void		expression(compiler* this_compiler);
friend	void		condition_calculation(compiler* this_compiler);
friend	void		statement(compiler* this_compiler);
friend	void		compile(compiler* this_compiler);

// ステートメント
friend	bool		create_statement_goto(compiler* this_compiler);
friend	bool		create_statement_gosub(compiler* this_compiler);

public:
	explicit	compiler();

	// コンパイル 成功 true 失敗 false
	bool		compile(void);

	void		set(lex::lex* s_lex);
	void		set(const std::vector<std::vector<lex::token>>& vector_token);

	// 初期化
	void		clear(void) { this->reset(); };
	void		reset(void);

	// コードを出力
	void		output(void);

	// 最終手段
std::vector<code>	get_codes(void) const { return (this->codes); };

private:
std::vector<std::vector<lex::token>>		m_vector_token;
std::vector<code>	codes;
std::vector<lex::token>	list_token; // token list. ( バッファ )

std::stack<long int>	save_address_if_start; // IF文の開始アドレスを保存する。
std::map<std::string, long int> map_label_and_goto; // ラベルと、goto 文の位置記録用、
std::vector<long int>	save_label_and_goto; // ラベルが存在しなかった goto文の位置を記録、コンパイル終了後に一斉検索。

	long int	next_load; // 次に読み込むトークン ( vector.at(next_load) )
	long int	load_line; // コンパイルしている行 ( ファイル ) 読み込み行

};

//---------------------------------------------------------
// コードを追加
extern	long int		gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op);
extern	long int		gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op, const double value);
extern	long int		gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op, const std::string &str_value);
extern	long int		gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op, const double value, const std::string &str_value);
// 出力
extern	void			print_code(const code& a);
//---------------------------------------------------------

} // namespace compiler
} // namespace gscripter


#endif // __GSCRIPTER_GSCRIPTER_COMPILER_H_INCLUDE_GUARD_201139_4645_704575_33__

