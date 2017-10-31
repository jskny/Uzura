/*
 *	ガーネット・スクリプター
 *	コンパイラ
 *	2011 / 06 / 09
 *	jskny
*/

#include	<iostream>
#include	<string>
#include	<vector>
#include	<list>
#include	<stack>
#include	<map>

#include	<assert.h>

#include	"gscripter_def.h"
#include	"gscripter_lex.h"
#include	"gscripter_compiler.h"

using namespace std;
using namespace gscripter::lex;

namespace gscripter
{
namespace compiler
{

//---------------------------------------------------------
// 式の因子をコンパイルする
extern	void		factor(compiler* this_compiler);
// 式の項をコンパイルする
extern	void		term(compiler* this_compiler);
// 式をコンパイルする
extern	void		expression(compiler* this_compiler);
// 条件式をコンパイルする
extern	void		condition_calculation(compiler* this_compiler);
// ステートメントをコンパイルする。
extern	void		statement(compiler* this_compiler);

// goto 文の作成
extern	bool		create_statement_goto(compiler* this_compiler);

//---------------------------------------------------------
// gen_code
//---------------------------------------------------------

// パラメーターなしの命令をスタックに積む
long int gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op)
{
	code obj;
	obj.type = type_op;

	vector_code->push_back(obj);

	return ((vector_code->size() - 1));
}

// 数値をスタックに積む
long int gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op, const double value)
{
	code obj;
	obj.type = type_op;
	obj.value = value;

	vector_code->push_back(obj);

	return ((vector_code->size() - 1));
}

// 文字列をスタックに積む
long int gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op, const std::string& str_value)
{
	code obj;
	obj.type = type_op;
	obj.str_value = str_value;

	vector_code->push_back(obj);

	return ((vector_code->size() - 1));
}

// 全部積む。
long int gen_code(std::vector<code>* vector_code, const GSCRIPTER_OBJECT_CODE type_op, const double value, const std::string& str_value)
{
	code obj;
	obj.type = type_op;
	obj.value = value;
	obj.str_value = str_value;

	vector_code->push_back(obj);

	return ((vector_code->size() - 1));
}

//---------------------------------------------------------

static std::string gscripter_code_text[] =  {
	"damy",
	// 無効
	"INVALID",
// コマンド
"START_COMMAND", 
	// 数値
	"VALUE",
	// 文字列
	"STRING",
	// 変数
	"VALUE_VARIABLE",
	"STRING_VARIABLE",
	// シンボル
	"SYMBOL",
	// 数値のマイナス化命令 ( value * -1 )
	"MINUS_CONVERT",

	// IF
	"IF",
	// IF BLOCK END.
	"IF_END",

	// LABEL and GOTO
	"LABEL",
	"GOTO",

	// gosub, return
	"GOSUB",
	"RETURN",

	// ジャンプ
	"JAMP",
	// 条件ジャンプ
	"JAMPC",
	// CALL 関数呼び出し
	"CALL",
	// 関数の引数の開始
	"CALL_FUNC_ARGV_START",
	// END
	"END",
	// nop (何もしない)
	"NOP",

// 演算子
"START_OPERAND", 
	"ADD", // +
	"SUB", // - 
	"MULT", // *
	"DIVIDE", // /
	"REMAINDER", // %
	"SMALLER", // < 
	"SMALLER_EQUAL", // <=
	"GREATER", // >
	"GREATER_EQUAL", // >=
	"EQUAL", // ==
	"NOT_EQUAL", // !=
	"ASSIGN", // = (代入)

	"STR_ADD", // . 文字列結合演算子
	"damy"
};

void print_code(const code& a)
{
	cout << "type  : " << a.type << "," << gscripter_code_text[a.type - GSCRIPTER_CODE_INVALID + 1] << endl;
	cout << "value : " << a.value << endl;
	cout << "str   : " << a.str_value << endl;

	return;
}

//---------------------------------------------------------
// compiler
//---------------------------------------------------------

// goto 文の作成。
bool create_statement_goto(compiler* this_compiler)
{
	if (!this_compiler) {
GSCRIPTER_ERROR("numm pointer.");
		return (false);
	}

	token tok = this_compiler->list_token.at(this_compiler->next_load);
	if (tok.type != GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL) {
GSCRIPTER_ERROR("goto 文には、特殊な名前は使用できません。");
		return (false);
	}

	map <string, long int>::iterator itr;
	itr = this_compiler->map_label_and_goto.find(tok.name);
	if (itr == this_compiler->map_label_and_goto.end()) {
		// 検索必要フラグを立てる。
		// なぜ、size - 1 ではなく size かというと、
		// 下で、 gen_code でgoto 文を作成しているから。
		this_compiler->save_label_and_goto.push_back(this_compiler->codes.size());
		gen_code(&this_compiler->codes, GSCRIPTER_CODE_GOTO, -1, tok.name);
		return (true);
	}

	long int address = itr->second;
	if (address < 0) {
GSCRIPTER_ERROR("goto 文で指定したラベルの飛んだ先が不正です。 syntax error ?");
		return (false);
	}
	else if (address > this_compiler->codes.size()) {
GSCRIPTER_ERROR("goto 文で指定しているラベルの飛び先が不正です。 syntax error ?");
		return (false);
	}

	// 飛んだ先がラベルか否か、確認。
	code buf_goto_code = this_compiler->codes.at(address);
	if (buf_goto_code.type != GSCRIPTER_CODE_LABEL) {
GSCRIPTER_ERROR("syntax error.");
		return (true);
	}
	gen_code(&this_compiler->codes, GSCRIPTER_CODE_GOTO, address, tok.name);
	return (true);
}

// gosub 文の作成。
bool create_statement_gosub(compiler* this_compiler)
{
	if (!this_compiler) {
GSCRIPTER_ERROR("numm pointer.");
		return (false);
	}

	token tok = this_compiler->list_token.at(this_compiler->next_load);
	if (tok.type != GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL) {
GSCRIPTER_ERROR("gosub 文には、特殊な名前は使用できません。");
		return (false);
	}

	map <string, long int>::iterator itr;
	itr = this_compiler->map_label_and_goto.find(tok.name);
	if (itr == this_compiler->map_label_and_goto.end()) {
		// 検索必要フラグを立てる。
		// なぜ、size - 1 ではなく size かというと、
		// 下で、 gen_code でgoto 文を作成しているから。
		this_compiler->save_label_and_goto.push_back(this_compiler->codes.size());
		gen_code(&this_compiler->codes, GSCRIPTER_CODE_GOSUB, -1, tok.name);
		return (true);
	}

	long int address = itr->second;
	if (address < 0) {
GSCRIPTER_ERROR("gosub 文で指定したラベルの飛んだ先が不正です。 syntax error ?");
		return (false);
	}
	else if (address > this_compiler->codes.size()) {
GSCRIPTER_ERROR("gosub 文で指定しているラベルの飛び先が不正です。 syntax error ?");
		return (false);
	}

	// 飛んだ先がラベルか否か、確認。
	code buf_goto_code = this_compiler->codes.at(address);
	if (buf_goto_code.type != GSCRIPTER_CODE_LABEL) {
GSCRIPTER_ERROR("syntax error.");
		return (true);
	}
	gen_code(&this_compiler->codes, GSCRIPTER_CODE_GOSUB, address, tok.name);
	return (true);
}

// 式の因子をコンパイルする
void factor(compiler* this_compiler)
{
	assert(this_compiler);
GSCRIPTER_DEBUG("\t\t\t\tstart factor.");

	token tok = this_compiler->list_token.at(this_compiler->next_load++);
	string t_name;

	// 因子 ( a = a + 8 などの時の 8 と a )
	switch (tok.type)
	{
		case GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_SYMBOL, 0, tok.name);
			break;
		case GSCRIPTER_TOKEN_TYPE_NUMBER_VALUE:
			// 数値
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_VALUE, tok.value, tok.str_value);
			break;
		case GSCRIPTER_TOKEN_TYPE_STRING_VALUE:
			// 文字列
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_STRING, tok.value, tok.str_value);
			break;
		case GSCRIPTER_TOKEN_TYPE_NUMBER_VARIABLE:
			// 変数・・・コンパイル時には使わない。
GSCRIPTER_ERROR("このTokenTypeは存在しない。");
//			gen_code(&this_compiler->codes, GSCRIPTER_CODE_LOAD_VARIABLE, tok.value, tok.str_value);
			break;
		case GSCRIPTER_TOKEN_TYPE_STRING_VARIABLE:
			// 文字列変数・・・コンパイル時には使わない。
GSCRIPTER_ERROR("このTokenTypeは存在しない。");
//			gen_code(&this_compiler->codes, GSCRIPTER_CODE_LOAD_VARIABLE, tok.value, tok.str_value);
			break;
		case GSCRIPTER_TOKEN_TYPE_CALL:
			//( であろうトークンを読み込む。
			t_name = this_compiler->list_token.at(this_compiler->next_load++).name;
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_CALL_FUNC_ARGV_START, 0, t_name);

			// 関数呼び出しの CALL func-name(123); の ( かどうか。
			if (this_compiler->list_token.at(this_compiler->next_load++).type == GSCRIPTER_TOKEN_TYPE_L_PAREN)
			{
				// 次が ) なら抜ける。
				tok = this_compiler->list_token.at(this_compiler->next_load);
				if (tok.type == GSCRIPTER_TOKEN_TYPE_R_PAREN)
				{
					//  ) 発見。
					//  セミコロン (;) はあるかどうか
					tok = this_compiler->list_token.at(++this_compiler->next_load);
					if (tok.type == GSCRIPTER_TOKEN_TYPE_SEMICOLON) {
						// セミコロンあり。
						// 正常終了。
						break;
					}
					else {
// セミコロンなし。
GSCRIPTER_ERROR("セミコロン (;) がありません。");
						// 戻す。
						this_compiler->next_load--;
						break;
					}
				}
				else {
					//  ) が見つかるまで引数を探す。
					while (true)
					{
						// 計算
						expression(this_compiler);
						// 更新
						tok = this_compiler->list_token.at(this_compiler->next_load++);
						// カンマなら引数が続く。
						if (tok.type == GSCRIPTER_TOKEN_TYPE_COMMA) {
							// ここで NextLoadToken をインクリメントしなくてよい
							// なぜなら上のインクリメントで次のをすでに指しているから。
							continue;
						} else {
							// 違うなら抜ける。
							break;
						}
					} //  while (true)

					gen_code(&this_compiler->codes, GSCRIPTER_CODE_CALL, 0, t_name);

				} // if (tok.type == CSL_TOKEN_TYPE_R_PAREN) { ... } else { ... ]
			} // if (i < This_Quick->compile.TokenList[TokenI++].type == CSL_TOKEN_TYPE_L_PAREN) { ... } else { ... ]
			else {
GSCRIPTER_ERROR("関数呼び出しに ( がありません。");
				// 戻す。
				this_compiler->next_load--;
				goto factor_end;
			}
			break;
		case GSCRIPTER_TOKEN_TYPE_L_PAREN:
			// 因子
			expression(this_compiler);
			tok = this_compiler->list_token.at(this_compiler->next_load++);
			// カンマなら引数が続く。
			if (tok.type == GSCRIPTER_TOKEN_TYPE_R_PAREN) {
				goto factor_end;
			} else {
// MEMO: このエラーが出る意味と、コメントの意味が分からない。
// GSCRIPTER_ERROR("右括弧 ')' がありません。");
// print_token(tok);
				this_compiler->next_load--;
				goto factor_end;
			}
			break;
		case GSCRIPTER_TOKEN_TYPE_SEMICOLON:
		case GSCRIPTER_TOKEN_TYPE_R_BLAISE:
GSCRIPTER_DEBUG("式の因子 >> セミコロン発見。");
			// 文の終わり
			goto factor_end;
			break;
		default:
			// ここにきているのは * / % の演算子のはず。
			goto factor_end;
			break;
	}//  switch (tok.type)

	// 終了。
factor_end:
	GSCRIPTER_DEBUG("\t\t\t\tfinish factor.");

	return;
}

// 式の項をコンパイルする
void term(compiler* this_compiler)
{
	assert(this_compiler);

GSCRIPTER_DEBUG("\t\t\tstart trem.");

	factor(this_compiler);
	// ここで This_Quick->compile.NextLoadToken をインクリメントするな。
	token tok = this_compiler->list_token.at(this_compiler->next_load);

	// 掛け算と割り算なら続ける ( a = 3 + 5 * 8 * 7 / 7 + 1 などのため )
	while (
		tok.type == GSCRIPTER_TOKEN_TYPE_MULT ||
		tok.type == GSCRIPTER_TOKEN_TYPE_DIVIDE ||
		tok.type == GSCRIPTER_TOKEN_TYPE_REMAINDER)
	{
		// 計算前のトークンを記録。
		token _tok_buf = this_compiler->list_token.at(this_compiler->next_load++);

		if (
			_tok_buf.type == GSCRIPTER_TOKEN_TYPE_SEMICOLON ||
			_tok_buf.type == GSCRIPTER_TOKEN_TYPE_R_PAREN)
		{
			// セミコロンか ) ならループを抜ける。}
			// ExpressionCompile でインクリメントしているからスタックオーバーフロー回避策。
			this_compiler->next_load--;
			break;
		}

		factor(this_compiler);
		if (tok.type == GSCRIPTER_TOKEN_TYPE_MULT) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_MULT, 0);
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_DIVIDE) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_DIVIDE, 0);
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_REMAINDER) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_REMAINDER, 0);
		}
		else {
GSCRIPTER_ERROR("演算不能なトークンを計算させようとしています。");
print_token(tok);
		}

		// ここでも、インクリメントしない。
		tok = this_compiler->list_token.at(this_compiler->next_load);
	}
GSCRIPTER_DEBUG("\t\t\tfinish trem.");

	return;
}

// 式をコンパイルする
void expression(compiler* this_compiler)
{
	assert(this_compiler);

GSCRIPTER_DEBUG("\t\tstart expression.");
	// ここで This_Quick->compile.NextLoadToken をインクリメントするな。
	token tok = this_compiler->list_token.at(this_compiler->next_load);

	if (
		tok.type == GSCRIPTER_TOKEN_TYPE_PLUS ||
		tok.type == GSCRIPTER_TOKEN_TYPE_MINUS)
	{
		// 計算前のトークンを保存する。
		token _tok_buf = this_compiler->list_token.at(this_compiler->next_load++);
		// 式の項のコンパイル
		term(this_compiler);

		if (tok.type == GSCRIPTER_TOKEN_TYPE_MINUS) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_MINUS_CONVERT, 0);
		}

		tok = _tok_buf;
	}
	else {
		term(this_compiler);
	}

	// 計算結果を更新。 インクリメントするな。
	tok = this_compiler->list_token.at(this_compiler->next_load);

	while (
		tok.type == GSCRIPTER_TOKEN_TYPE_PLUS ||
		tok.type == GSCRIPTER_TOKEN_TYPE_MINUS ||
		tok.type == GSCRIPTER_TOKEN_TYPE_PERIOD)
	{
		// 計算前のトークンを保存する。
		token _tok_buf = this_compiler->list_token.at(this_compiler->next_load++);

		if (
			_tok_buf.type == GSCRIPTER_TOKEN_TYPE_SEMICOLON ||
			_tok_buf.type == GSCRIPTER_TOKEN_TYPE_R_PAREN) {
			//  セミコロンか ) ならループを抜ける。
			break;
		}

		// 式の項のコンパイル
		term(this_compiler);
		if (tok.type == GSCRIPTER_TOKEN_TYPE_PLUS) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_ADD, 0);
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_MINUS) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_SUB, 0);
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_PERIOD) {
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_STR_ADD, 0);
		}
		else {
GSCRIPTER_ERROR("演算不能なトークンを計算させようとしています。");
print_token(tok);
		}

		// インクリメントするな。
		tok = this_compiler->list_token.at(this_compiler->next_load);
	}

GSCRIPTER_DEBUG("\t\tfinish expression.");

	return;
}

// 条件式をコンパイルする
void condition_calculation(compiler* this_compiler)
{
	assert(this_compiler);
GSCRIPTER_DEBUG("\tstart condition_calculation.");

	expression(this_compiler);
	token tok = this_compiler->list_token.at(this_compiler->next_load++);

	// 条件式でつ返すトークンかどうかを調べる。
	switch (tok.type)
	{
		case GSCRIPTER_TOKEN_TYPE_SMALLER:
		case GSCRIPTER_TOKEN_TYPE_SMALLER_EQUAL:
		case GSCRIPTER_TOKEN_TYPE_GREATER:
		case GSCRIPTER_TOKEN_TYPE_GREATER_EQUAL:
		case GSCRIPTER_TOKEN_TYPE_EQUAL:
		case GSCRIPTER_TOKEN_TYPE_NOT_EQUAL:
			break;
		default:
GSCRIPTER_ERROR("条件式が不正です。");
print_token(tok);
			goto condition_calculation_end;
			break;
	}

	expression(this_compiler);

	// コード作成
	switch (tok.type)
	{
		case GSCRIPTER_TOKEN_TYPE_SMALLER:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_SMALLER, 0);
			break;
		case GSCRIPTER_TOKEN_TYPE_SMALLER_EQUAL:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_SMALLER_EQUAL, 0);
			break;
		case GSCRIPTER_TOKEN_TYPE_GREATER:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_GREATER, 0);
			break;
		case GSCRIPTER_TOKEN_TYPE_GREATER_EQUAL:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_GREATER_EQUAL, 0);
			break;
		case GSCRIPTER_TOKEN_TYPE_EQUAL:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_EQUAL, 0);
			break;
		case GSCRIPTER_TOKEN_TYPE_NOT_EQUAL:
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_NOT_EQUAL, 0);
			break;
		default:
GSCRIPTER_ERROR("条件式のコード作成において通常ではありえないトークンが存在します。");
print_token(tok);
			goto condition_calculation_end;
			break;
	}

	// 終了。
condition_calculation_end:
	GSCRIPTER_DEBUG("\tfinish condition_calculation.");

	return;
}

// ステートメントをコンパイルする。
static void statement(compiler* this_compiler)
{
	assert(this_compiler);

	// ここのみインクリメントする
	token tok;
	if (this_compiler->next_load >= this_compiler->list_token.size()) {
		return;
	} else {
		tok = this_compiler->list_token.at(this_compiler->next_load++);
	}

GSCRIPTER_DEBUG("\tstart compile : statement.");
	while (true)
	{
		if (tok.type == GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL) {
			token old_tok; // 名前の保持用
			// シンボル。
// MEMO:			string t_name = tok.name;
			// 名前表の上書き
// MEMO:			this_compiler->map_name[t_name] = tok;

			// 次は代入命令 = のはず。
			// なぜなら
			// 式の中で一番最初にユーザー定義のトークンがあるということは
			//  aaa = 1 などの時の aaa だから。
			old_tok = tok;
			tok = this_compiler->list_token.at(this_compiler->next_load);

			if (tok.type == GSCRIPTER_TOKEN_TYPE_ASSIGN) {
				this_compiler->next_load++;
				expression(this_compiler);
				gen_code(&this_compiler->codes, GSCRIPTER_CODE_ASSIGN, 0, old_tok.name);
				goto statement_end;
			} else {
				gen_code(&this_compiler->codes, GSCRIPTER_CODE_SYMBOL, 0, tok.name);
				goto statement_end;
			}

			return;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_IF)
		{
			//  IF
GSCRIPTER_DEBUG("\tstart command 'if' create.");
			condition_calculation(this_compiler);
			int index = gen_code(&this_compiler->codes, GSCRIPTER_CODE_IF, 0);
			if (index < 0) {
GSCRIPTER_ERROR("IF命令の作成に失敗しました。");
			}
			else {
				// アドレスを保存
				this_compiler->save_address_if_start.push(index);
			}
GSCRIPTER_DEBUG("\tfinish command 'if' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_IF_END)
		{
			//  IF_END
GSCRIPTER_DEBUG("\tstart command 'endif' create.");
			if (this_compiler->save_address_if_start.size() <= 0) {
GSCRIPTER_ERROR("不正な位置に ENDIF 文が存在します。");
goto statement_end;
			}

			// IF文の終わりの位置を IF 文に追加
			long int index = this_compiler->save_address_if_start.top();
			this_compiler->save_address_if_start.pop();
			this_compiler->codes.at(index).value = (this_compiler->codes.size() - 1) - index;
GSCRIPTER_DEBUG("\tfinish command 'endif' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_CALL)
		{
			// 関数名
GSCRIPTER_DEBUG("\tstart command 'call' create.");
			tok = this_compiler->list_token.at(this_compiler->next_load++);
			if (tok.type != GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL) {
GSCRIPTER_ERROR("関数名が不正です。");
print_token(tok);
				goto statement_end;
			}

			// 関数名
			string t_name = tok.name;
			int FuncParamCounter = 0;
			//  ( を飛ばす 理由 ) がないというエラーを出すから。
			this_compiler->next_load++;
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_CALL_FUNC_ARGV_START, 0, t_name);

			while (true)
			{
				//  ) か ; があるまで続ける
				expression(this_compiler);
				tok = this_compiler->list_token.at(this_compiler->next_load++);
				if (tok.type == GSCRIPTER_TOKEN_TYPE_COMMA) {
					// PrintToken(tok);
					// カンマの時はループが続く。
					FuncParamCounter++;
					continue;
				}
				else if (tok.type == GSCRIPTER_TOKEN_TYPE_SEMICOLON || tok.type == GSCRIPTER_TOKEN_TYPE_R_PAREN) {
					//  ; と ) の時はループを抜ける。
					break;
				}

				break;
			}

			// 内蔵関数呼び出しコード
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_CALL, 0, t_name);
GSCRIPTER_DEBUG("\tfinish command 'call' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_LABEL)
		{
			// LABEL
GSCRIPTER_DEBUG("\tstart command 'label' create.");
			tok = this_compiler->list_token.at(this_compiler->next_load);
			if (tok.type != GSCRIPTER_TOKEN_TYPE_UNKNOWN_SYMBOL) {
GSCRIPTER_ERROR("label 文には、特殊な名前は使用できません。");
			}

			map <string, long int>::iterator itr;
			itr = this_compiler->map_label_and_goto.find(tok.name);
			if (itr != this_compiler->map_label_and_goto.end()) {
GSCRIPTER_ERROR("同じラベル名がすでに存在します。");
				goto statement_end;
			}

			int index = gen_code(&this_compiler->codes, GSCRIPTER_CODE_LABEL, 0, tok.name);
			this_compiler->map_label_and_goto[tok.name] = index;
GSCRIPTER_DEBUG("\tfinish command 'label' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_GOTO)
		{
			// GOTO
GSCRIPTER_DEBUG("\tstart command 'goto' create.");
			if (!create_statement_goto(this_compiler)) {
				goto statement_end;
			}
GSCRIPTER_DEBUG("\tfinish command 'goto' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_GOSUB)
		{
			// GOSUB
GSCRIPTER_DEBUG("\tstart command 'gosub' create.");
			if (!create_statement_gosub(this_compiler)) {
				goto statement_end;
			}
GSCRIPTER_DEBUG("\tfinish command 'gosub' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_RETURN)
		{
			// RETURN
GSCRIPTER_DEBUG("\tstart command 'return' create.");
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_RETURN, tok.value, tok.name);
GSCRIPTER_DEBUG("\tfinish command 'return' create.");
			goto statement_end;
		}
		else if (tok.type == GSCRIPTER_TOKEN_TYPE_END)
		{
			// END
GSCRIPTER_DEBUG("\tstart command 'end' create.");
			this_compiler->next_load++;
			gen_code(&this_compiler->codes, GSCRIPTER_CODE_END, 0);
GSCRIPTER_DEBUG("\tfinish command 'end' create.");
			goto statement_end;
		}
		else if (
			tok.type == GSCRIPTER_TOKEN_TYPE_SEMICOLON ||
			tok.type == GSCRIPTER_TOKEN_TYPE_R_BLAISE)
		{
			// 文の終わり。
			goto statement_end;
		}
		else {
GSCRIPTER_ERROR("ステートメントとして使用できないトークンが存在します。");
print_token(tok);
			goto statement_end;
		}
	}

	// 終了。
statement_end:
	GSCRIPTER_DEBUG("\tfinish compile : statement.");

	return;
}

// コンパイル
void compile(compiler* this_compiler)
{
GSCRIPTER_DEBUG("start compile : compile.");

	while (true)
	{
		if (this_compiler->load_line < this_compiler->m_vector_token.size()) {
			this_compiler->list_token = this_compiler->m_vector_token.at(this_compiler->load_line);
			this_compiler->load_line++;
		} else {
			break;
		}
#if GSCRIPTER_FLAG_DEBUG == 1
cout << "compile statement : line = " << (this_compiler->load_line - 1) << endl; // 読み込みの時、インクリメントされているから。
#endif
		try {
			statement(this_compiler);
		}
		catch (...)
		{
			GSCRIPTER_ERROR("syntax error.");
			cerr << "line  : " << this_compiler->load_line << endl;
			cerr << "token : " << this_compiler->next_load << endl;
			abort(); // TODO: いずれ、abort ではない例外処理を書く。
		}
		this_compiler->list_token.clear();
		this_compiler->next_load = 0;
	}

GSCRIPTER_DEBUG("finish compile : compile.");

	// goto文の指すラベルを検索。
GSCRIPTER_DEBUG("start compile : search label to set.");
	map <string, long int>::iterator map_itr;
	vector <long int>::iterator vector_itr;
	token tok;

	for (
		vector_itr = this_compiler->save_label_and_goto.begin();
		vector_itr != this_compiler->save_label_and_goto.end();
		vector_itr++)
	{
		if ((*vector_itr) < 0) {
GSCRIPTER_ERROR("goto または gosub 文で指定したラベルの飛んだ先が不正です。 syntax error ?");
			continue;
		}
		else if ((*vector_itr) > this_compiler->codes.size()) {
GSCRIPTER_ERROR("goto または gosub 文で指定しているラベルの飛び先が不正です。 syntax error ?");
			continue;
		}

		code buf_goto_code = this_compiler->codes.at((*vector_itr));
		if (	(buf_goto_code.type != GSCRIPTER_CODE_GOTO) &&
			(buf_goto_code.type != GSCRIPTER_CODE_GOSUB))
		{
			// 移動先を指定するコードが GOTO 文ではない。
GSCRIPTER_ERROR("compiler : system error. ( can not find goto command. )");
			continue;
		}

		map_itr = this_compiler->map_label_and_goto.find(buf_goto_code.str_value);
		if (map_itr == this_compiler->map_label_and_goto.end()) {
GSCRIPTER_ERROR("goto または gosub 文で指定したラベルが存在しません。");
			continue;
		}

		// 飛び先をセット
GSCRIPTER_DEBUG("\tlabel find.");
		if ((this_compiler->codes.at((*vector_itr)).type != GSCRIPTER_CODE_GOTO) &&
		    (this_compiler->codes.at((*vector_itr)).type != GSCRIPTER_CODE_GOSUB))
		{
			// why ? goto 文じゃないのをなぜ変更する必要がある？
GSCRIPTER_ERROR("compiler : system error. ( can't find ('goto' or 'gosub') command. )");
			continue;
		}
		this_compiler->codes.at((*vector_itr)).value = map_itr->second;
	}

GSCRIPTER_DEBUG("finish compile : search label to set.");

	return;
}

//---------------------------------------------------------
// コンパイラ
//---------------------------------------------------------

compiler::compiler() :
	next_load(0), load_line(0)
{
	this->codes.clear();
	if (!this->save_address_if_start.empty()) {
		for (unsigned int i = 0; i < this->save_address_if_start.size(); i++) {
			this->save_address_if_start.pop();
		}
	}
	this->list_token.clear();
	this->m_vector_token.clear();
	this->save_label_and_goto.clear();

	return;
}

bool compiler::compile(void)
{
	::gscripter::compiler::compile(this);
	return (true);
}

void compiler::set(lex::lex* s_lex)
{
	if (!s_lex) {
GSCRIPTER_ERROR("null pointer.");
		return;
	}

	this->m_vector_token = s_lex->get_vector_token();
	if (this->m_vector_token.empty()) {
GSCRIPTER_DEBUG("レキシカルアナライズされたトークンが存在しない。");
	}

	return;
}

void compiler::set(const std::vector<std::vector<lex::token>>& vector_token)
{
	this->m_vector_token = vector_token;
	if (this->m_vector_token.empty()) {
GSCRIPTER_DEBUG("レキシカルアナライズされたトークンが存在しない。");
	}

	return;
}

void compiler::reset(void)
{
	this->codes.clear();
	this->list_token.clear();
	this->m_vector_token.clear();
	this->map_label_and_goto.clear();
	this->next_load = 0;
	this->load_line = 0;
	this->save_label_and_goto.clear();

	for (unsigned long int i = 0; i < this->save_address_if_start.size(); i++) {
		this->save_address_if_start.pop();
	}

	return;
}

void compiler::output(void)
{
	std::vector<code>::iterator itr;

	for (itr = this->codes.begin(); itr != this->codes.end(); itr++) {
		print_code(*itr);
	}

	return;
}

} // namespace compiler
} // namespace gscripter

