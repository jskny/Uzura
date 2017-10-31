/*
 *	ガーネット・スクリプター
 *	インタープリタ
 *	2011 / 06 / 16
 *	jskny
*/

#include	<iostream>
#include	<algorithm> // reverse
#include	<sstream>
#include	<vector>
#include	<string>
#include	<map>

#include	<assert.h>

// 実行側は、gscripter.h だけをインクルードしていれば良い。
// 今やろうとしている改造は、 VM をいじるほどのことか？
// 出来る限り VM はいじらないほうが良い。
// VM の改造を本気でやると、二重インクルードという難問にぶち当たるから。
//
// *追記 ( 2011 / 07 / 27 )
// つーか、set_function メソッドで、
// C++ の関数を gscripter から呼べるようにできるので、
// call_native_function の中を C++ 側がいじるのは、
// 意味なくないか？
// ぁぁ、VM を直接書き換えていたあの馬鹿な頃の私・・・
#ifndef __GSCRIPTER_GSCRIPTER_H_INCLUDE_GUARD_402_48293_666_192_111_000__
// 二重インクルード対策。
#include	"gscripter_interpreter.h"
#include	"gscripter_compiler.h"
#include	"gscripter_util.h"

#include	"gscripter_def.h"
#endif // gscripter.h で定義されている ( define ) __GSCRIPTER_GSCRIPTER_H_INCLUDE_GUARD_402_48293_666_192_111_000__

//---------------------------------------------------------
// ここに、GScripter を使用するプロジェクトへの
// #incldue を書きこむ。
// #include	"../file.h"
//---------------------------------------------------------

using namespace std;


namespace gscripter
{
namespace interpreter
{


#pragma warning (push)
#pragma warning (disable : 4290) // throw 例外無効警告を消す。


//---------------------------------------------------------
// 例外
//---------------------------------------------------------

exception_interpreter::exception_interpreter(
	const std::string& text,
	const long int point,
	const compiler::code& code) :
	m_text(text), m_point(point),
	m_code(code)
{
}

void exception_interpreter::output(void)
{
	cout << this->m_text << endl;
	compiler::print_code(this->m_code);
	return;
}

//---------------------------------------------------------
// インタープリタ
//---------------------------------------------------------

void push(interpreter* this_interpreter, const compiler::code& obj)
{
	this_interpreter->exc_stack.push_back(obj);
	this_interpreter->pointer_memory++;
	return;
}

compiler::code pop(interpreter* this_interpreter)
{
	assert((this_interpreter->pointer_memory - 1) < this_interpreter->exc_stack.size());
	compiler::code obj = this_interpreter->exc_stack.at(this_interpreter->pointer_memory - 1);
	this_interpreter->exc_stack.pop_back();
	this_interpreter->pointer_memory--;
	return (obj);
}

compiler::code peek(interpreter* this_interpreter)
{
	assert(this_interpreter->pointer_memory < this_interpreter->exc_stack.size());
	compiler::code obj = this_interpreter->exc_stack.at(this_interpreter->pointer_memory);
	return (obj);
}

compiler::code peek1(interpreter* this_interpreter)
{
	assert((this_interpreter->pointer_memory - 2)< this_interpreter->exc_stack.size());
	compiler::code obj = this_interpreter->exc_stack.at(this_interpreter->pointer_memory - 2);
	return (obj);
}


//---------------------------------------------------------
// リバーズの実装
// http://www.cplusplus.com/reference/algorithm/reverse/
//---------------------------------------------------------
/*
template <typename T>
void swap(T& a, T& b)
{
	T c(a);
	a=b;
	b=c;
}

template <typename Iterator>
void reverse(Iterator first, Iterator last)
{
	while ((first != last) && (first != --last)) {
		swap(*first, *last);
		*first++;
	}
}*/


//---------------------------------------------------------

// 内蔵関数の呼び出し
compiler::code call_native_function(interpreter* this_interpreter, const std::string& func_name, std::vector<compiler::code>* argv) throw (exception_interpreter)
{
	if (!argv || !this_interpreter) {
throw exception_interpreter("null pointer.");
	}

	compiler::code ret_obj;
	long int argc = argv->size();

	// 内蔵関数の検索。
	map<string, gscripter_user_reg_native_faction>::iterator itr = this_interpreter->map_native_function.find(func_name);
	if (itr != this_interpreter->map_native_function.end()) {
		reverse(argv->begin(), argv->end());

		// 内蔵関数の実行。
		ret_obj = itr->second.run_function(this_interpreter, *argv);
		return (ret_obj);
	}

	if (func_name == "output_code") {
		print_code(argv->at(0));
	}
	else if (compare_string(func_name, "print")) {
		if (argc != 1) {
GSCRIPTER_ERROR("内蔵関数 PRINT への引数が正しくない。");
			return (ret_obj);
		}

		if ((argv->at(0).type == compiler::GSCRIPTER_CODE_VALUE_VARIABLE) || (argv->at(0).type == compiler::GSCRIPTER_CODE_VALUE)) {
			cout << argv->at(0).value;
		} else {
			cout << argv->at(0).str_value;
		}
	}
	else if (compare_string(func_name, "println")) {
		if (argc != 1) {
GSCRIPTER_ERROR("内蔵関数 PRINTLN への引数が正しくない。");
			return (ret_obj);
		}

		if ((argv->at(0).type == compiler::GSCRIPTER_CODE_VALUE_VARIABLE) || (argv->at(0).type == compiler::GSCRIPTER_CODE_VALUE)) {
			cout << argv->at(0).value << endl;
		} else {
			cout << argv->at(0).str_value << endl;
		}
	}
	else if (compare_string(func_name, "wait") || compare_string(func_name, "parser_wait")) {
		if (argc != 1) {
GSCRIPTER_ERROR("内蔵関数 parser_wait もしくは wait への引数が正しくない。");
			return (ret_obj);
		}
		this_interpreter->set_stop_counter(static_cast<int>(argv->at(0).value));
	}
	else if (compare_string(func_name, "parser_break")) {
		if (argc != 0) {
GSCRIPTER_ERROR("内蔵関数 parser_break への引数が正しくない。");
			return (ret_obj);
		}
		this_interpreter->set_stop_counter(1);
	}
	else {
		std::ostringstream oss;
		oss.str("");
		oss << "呼びだそうとした外部定義内蔵関数 '" << func_name << "' は存在しない。";
GSCRIPTER_ERROR(oss.str().c_str());
	}

	return (ret_obj);
}

// if 文の実行。
void exec_if(interpreter* this_interpreter, const ::gscripter::compiler::code& obj_code) throw (exception_interpreter)
{
	if (obj_code.type != compiler::GSCRIPTER_CODE_IF) {
throw exception_interpreter("IF命令が不正に呼び出されています。", this_interpreter->last_load, obj_code);
	}

	// 条件式の結果を取得。
	::gscripter::compiler::code if_exp = pop(this_interpreter);

	// 0 以外なら成功
	if ((int)if_exp.value != 0) {
		// true なら戻って処理続行。
		return;
	}
	else {
		// 飛ばす
		this_interpreter->last_load += static_cast<long int>(obj_code.value);
		if (this_interpreter->last_load < 0) {
throw exception_interpreter("IF命令によってプログラムカウンターが 0 以下の数値に書き換えられた。", this_interpreter->last_load, obj_code);
		}
	}

	return;
}

// goto 文の実行
void exec_goto(interpreter* this_interpreter, const ::gscripter::compiler::code& obj_code) throw (exception_interpreter)
{
	if (obj_code.type != compiler::GSCRIPTER_CODE_GOTO) {
throw exception_interpreter("GOTO命令が不正に呼び出されています。", this_interpreter->last_load, obj_code);
	}

	// 飛ばす
	this_interpreter->last_load = static_cast<long int>(obj_code.value);
	if (this_interpreter->last_load < 0) {
throw exception_interpreter("GOTO命令によってプログラムカウンターが 0 以下の数値に書き換えられた。", this_interpreter->last_load, obj_code);
	}

	return;
}

// gosub 文の実行
void exec_gosub(interpreter* this_interpreter, const ::gscripter::compiler::code& obj_code) throw (exception_interpreter)
{
	if (obj_code.type != compiler::GSCRIPTER_CODE_GOSUB) {
throw exception_interpreter("GOSUB命令が不正に呼び出されています。", this_interpreter->last_load, obj_code);
	}

	// 戻ってくる場所の記録。
	this_interpreter->save_gosub_return_point.push(this_interpreter->last_load);
	// 飛ばす
	this_interpreter->last_load = static_cast<long int>(obj_code.value);
	if (this_interpreter->last_load < 0) {
throw exception_interpreter("GOSUB命令によってプログラムカウンターが 0 以下の数値に書き換えられた。", this_interpreter->last_load, obj_code);
	}

	return;
}

// コードの実行。
void exec_one_code(interpreter* this_interpreter, const compiler::code &code) throw (exception_interpreter)
{
//print_code(code);
	using namespace compiler;

	::gscripter::compiler::code a2;
	::gscripter::compiler::code a1;
	::gscripter::compiler::code ans;

	if (code.type == GSCRIPTER_CODE_END) {
		// 終了。
		this_interpreter->set_flag_end(true);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_VALUE) {
		// 数値
		push(this_interpreter, code);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_STRING) {
		// 文字列
		push(this_interpreter, code);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_ASSIGN) {
		// 代入
		::gscripter::compiler::code  a = pop(this_interpreter);
		this_interpreter->map_variable[code.str_value].value = a.value;
		this_interpreter->map_variable[code.str_value].str_value = a.str_value;
		if (a.type == GSCRIPTER_CODE_STRING) {
			this_interpreter->map_variable[code.str_value].type = GSCRIPTER_CODE_STRING;
		}
		else {
			this_interpreter->map_variable[code.str_value].type = GSCRIPTER_CODE_VALUE;
		}
		return;
	}
	else if (code.type == GSCRIPTER_CODE_SYMBOL) {
		// シンボル
		push(this_interpreter, this_interpreter->map_variable[code.str_value]);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_STR_ADD) {
		// 文字列統合演算子
		ostringstream oss;
		a2 = pop(this_interpreter);
		a1 = pop(this_interpreter);

		if (a1.type == GSCRIPTER_CODE_VALUE) {
			oss << a1.value;
		} else {
			oss << a1.str_value;
		}

		if (a2.type == GSCRIPTER_CODE_VALUE) {
			oss << a2.value;
		} else {
			oss << a2.str_value;
		}

		ans.str_value = oss.str();
		ans.type = GSCRIPTER_CODE_STRING;
		push(this_interpreter, ans);
		return;
	}
	// 足し引き掛け割り、余りを出すアレ ( 加算、減算、乗算、割り算 )
	else if (
		(code.type == GSCRIPTER_CODE_ADD) ||
		(code.type == GSCRIPTER_CODE_SUB) ||
		(code.type == GSCRIPTER_CODE_MULT) ||
		(code.type == GSCRIPTER_CODE_DIVIDE) ||
		(code.type == GSCRIPTER_CODE_REMAINDER)) {
		// 加算
		a2 = pop(this_interpreter);
		a1 = pop(this_interpreter);
		if (code.type == GSCRIPTER_CODE_ADD) {
			ans.value = (a1.value + a2.value);
		} else if (code.type == GSCRIPTER_CODE_SUB) {
			ans.value = (a1.value - a2.value);
		} else if (code.type == GSCRIPTER_CODE_MULT) {
			ans.value = (a1.value * a2.value);
		} else if (code.type == GSCRIPTER_CODE_DIVIDE) {
			ans.value = (a1.value / a2.value);
		} else if (code.type == GSCRIPTER_CODE_REMAINDER) {
			ans.value = ((int)a1.value % (int)a2.value);
		} else {
throw exception_interpreter("system : IF文で弾かれており呼び出される事のない else ブロックが実行された。", this_interpreter->last_load, code);
		}
		ans.type = GSCRIPTER_CODE_VALUE;
		push(this_interpreter, ans);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_MINUS_CONVERT) {
		// 数値のマイナス化命令。
		a1 = pop(this_interpreter);
		ans.value = (-1 * a1.value);
		ans.type = GSCRIPTER_CODE_VALUE;
		push(this_interpreter, ans);
		return;
	}
	// 条件式
	else if (
		(code.type == GSCRIPTER_CODE_EQUAL) ||
		(code.type == GSCRIPTER_CODE_NOT_EQUAL) ||
		(code.type == GSCRIPTER_CODE_SMALLER) ||
		(code.type == GSCRIPTER_CODE_SMALLER_EQUAL) ||
		(code.type == GSCRIPTER_CODE_GREATER) ||
		(code.type == GSCRIPTER_CODE_GREATER_EQUAL)) {
		//  == 演算子
		a2 = pop(this_interpreter);
		a1 = pop(this_interpreter);
		if (code.type == GSCRIPTER_CODE_EQUAL) {
			ans.value = (a1.value == a2.value);
		} else if (code.type == GSCRIPTER_CODE_NOT_EQUAL) {
			ans.value = (a1.value != a2.value);
		} else if (code.type == GSCRIPTER_CODE_SMALLER) {
			ans.value = (a1.value < a2.value);
		} else if (code.type == GSCRIPTER_CODE_SMALLER_EQUAL) {
			ans.value = (a1.value <= a2.value);
		} else if (code.type == GSCRIPTER_CODE_GREATER) {
			ans.value = (a1.value > a2.value);
		} else if (code.type == GSCRIPTER_CODE_GREATER_EQUAL) {
			ans.value = (a1.value >= a2.value);
		} else {
throw exception_interpreter("system : IF文で弾かれており呼び出される事のない else ブロックが実行された。", this_interpreter->last_load, code);
		}
		ans.type = GSCRIPTER_CODE_VALUE;
		push(this_interpreter, ans);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_IF) {
		//  IF命令
		exec_if(this_interpreter, code);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_LABEL) {
		// LABEL 命令
		// コンパイラの改造で不要になったが一応残しておく。
		return;
	}
	else if (code.type == GSCRIPTER_CODE_GOTO) {
		// GOTO 命令
		exec_goto(this_interpreter, code);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_GOSUB) {
		// GOSUB 命令
		exec_gosub(this_interpreter, code);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_RETURN) {
		// RETURN 命令
		if (this_interpreter->save_gosub_return_point.empty()) {
throw exception_interpreter("RETURN文に対応する GOSUB命令は存在しません。", this_interpreter->last_load, code);
		}
		else {
			this_interpreter->last_load = this_interpreter->save_gosub_return_point.top();
			this_interpreter->save_gosub_return_point.pop();
		}
		return;
	}
	else if (code.type == GSCRIPTER_CODE_CALL) {
		// 内蔵関数の呼び出し。
		vector <::gscripter::compiler::code> argv;
		::gscripter::compiler::code ret_code;
		while (true)
		{
			::gscripter::compiler::code a = pop(this_interpreter);

			if (a.type != GSCRIPTER_CODE_CALL_FUNC_ARGV_START) {
				argv.push_back(a);
			}
			else {
				break;
			}
		}

		ret_code = call_native_function(this_interpreter, code.str_value, &argv);
		push(this_interpreter, ret_code);
		return;
	}
	else if (code.type == GSCRIPTER_CODE_CALL_FUNC_ARGV_START) {
		::gscripter::compiler::code a;
		a.type = GSCRIPTER_CODE_CALL_FUNC_ARGV_START;
		push(this_interpreter, a);
		return;
	}
	else {
		GSCRIPTER_ERROR("未定義。");
		print_code(code);
		return;
	}

	return;
}

// 実行
void exec(interpreter* this_interpreter) throw (exception_interpreter)
{
	using namespace compiler;
	::gscripter::compiler::code code;

	if (!this_interpreter) {
throw exception_interpreter("null pointer.");
	}

	while (true)
	{
		if (	(code.type == GSCRIPTER_CODE_END) ||
			(this_interpreter->get_flag_stop() ||
			(this_interpreter->get_flag_end()))) {
			// 終了。
			return;
		}
		// stop_counter が 0 より大きい。 -> parser_break などの関数が呼ばれた。
		else if (this_interpreter->get_stop_counter() > 0) {
			this_interpreter->set_stop_counter(this_interpreter->get_stop_counter() - 1);
			return;
		}
		else {
			if (this_interpreter->last_load < this_interpreter->codes.size()) {
				code = this_interpreter->codes.at(this_interpreter->last_load++);
			} else {
				// オーバー。
				this_interpreter->set_flag_end(true);
				goto exec_end;
			}
		}

		// 実行。
		try {
			exec_one_code(this_interpreter, code);
		}
		catch (exception_interpreter exp) {
			exp.output();
			throw exp; // 再throw
		}
	} // while (true)

exec_end:

	return;
}

//---------------------------------------------------------
// ユーザー定義の内蔵関数
//---------------------------------------------------------

gscripter_user_reg_native_faction::gscripter_user_reg_native_faction() :
	m_ptr_function(NULL), m_func_name("")
{
}

void gscripter_user_reg_native_faction::set_function(compiler::code (*ptr_function) (interpreter* this_interpreter, const std::vector<compiler::code>& argv)) throw (exception_interpreter)
{
	if (!ptr_function) {
throw exception_interpreter("null pointer.");
	}

	this->m_ptr_function = ptr_function;
	return;
}

compiler::code gscripter_user_reg_native_faction::run_function(interpreter* this_interpreter, const std::vector<compiler::code>& argv) throw (exception_interpreter)
{
	if (!this->m_ptr_function) {
throw exception_interpreter("null pointer.");
	}

	compiler::code a = (*this->m_ptr_function)(this_interpreter, argv);
	return (a);
}

//---------------------------------------------------------

interpreter::interpreter() :
	last_load(0), pointer_memory(0),
	flag_stop(false), flag_end(false),
	counter_stop(0)
{
	this->codes.clear();
	this->exc_stack.clear();
	this->map_variable.clear();
	this->map_native_function.clear();
}

void interpreter::set(const std::vector<compiler::code>& vector_codes)
{
	if (vector_codes.empty()) {
GSCRIPTER_DEBUG("コードが空です。");
	}

	this->codes = vector_codes;
	this->set_flag_stop(false);
	this->set_stop_counter(0);
	return;
}

void interpreter::set(compiler::compiler* s_compiler)
{
	if (!s_compiler) {
GSCRIPTER_ERROR("null pointer.");
		return;
	}

	this->set(s_compiler->get_codes());
	this->set_flag_stop(false);
	this->set_stop_counter(0);
	return;
}

// 実行
void interpreter::run(void) throw (exception_interpreter)
{
	if (this->get_flag_stop() || this->get_flag_end()) {
		return;
	}
	else if (this->get_stop_counter() > 0) {
		this->set_stop_counter(this->get_stop_counter() - 1);
		return;
	}

	exec(this);
	return;
}

void interpreter::reset(void)
{
	this->last_load = 0;
	this->pointer_memory = 0;
	this->map_variable.clear();
	this->codes.clear();
	this->exc_stack.clear();
	this->map_native_function.clear();

	this->set_flag_stop(false);
	this->set_stop_counter(0);

	// end文が実行された時 true.
	this->set_flag_end(false);

	// スタック初期化
	if (!this->save_gosub_return_point.empty()) {
		for (unsigned long int i = 0; i < this->save_gosub_return_point.size(); i++) {
			this->save_gosub_return_point.pop();
		}
	}
}

// native function の追加 ( local )
void interpreter::set_function(const gscripter_user_reg_native_faction& n_func)
{
	this->map_native_function[n_func.get_func_name()] = n_func;
	return;
}

// native function の破棄 ( local )
void interpreter::del_function(const std::string& func_name)
{
	this->map_native_function.erase(func_name);
	return;
}

//---------------------------------------------------------

// 変数表に登録されいている変数オブジェクトの取得
compiler::code get_variable(interpreter* this_interpreter, const std::string& name)
{
	compiler::code ret_obj;
	map <string, compiler::code>::iterator itr;
	itr = this_interpreter->map_variable.find(name);

	// 変数は登録されていない
	if (itr == this_interpreter->map_variable.end()) {
		GSCRIPTER_DEBUG("検索した変数は登録されていない。");
	}
	else {
		ret_obj = itr->second;
	}

	return (ret_obj);
}

// 変数表に登録されいている変数オブジェクトの値の取得
double get_variable_value(interpreter* this_interpreter, const std::string& name)
{
	return (get_variable(this_interpreter, name).value);
}

// 変数表に登録されいている変数オブジェクトの値の取得
std::string get_variable_str_value(interpreter* this_interpreter, const std::string& name)
{
	return (get_variable(this_interpreter, name).str_value);
}

// 変数表に値の登録
void set_variable(interpreter* this_interpreter, const std::string& name, const compiler::code& obj)
{
	this_interpreter->map_variable[name] = obj;
	return;
}

// 変数表に値の登録
void set_variable_value(interpreter* this_interpreter, const std::string& name, const double value)
{
	compiler::code a;
	a.value = value;
	a.type = compiler::GSCRIPTER_CODE_VALUE_VARIABLE;
	this_interpreter->map_variable[name] = a;
	return;
}

// 変数表に値の登録
void set_variable_str_value(interpreter* this_interpreter, const std::string& name, const std::string& str_value)
{
	compiler::code a;
	a.str_value = str_value;
	a.type = compiler::GSCRIPTER_CODE_STRING_VARIABLE;
	this_interpreter->map_variable[name] = a;
	return;
}

#pragma warning (pop) // #pragma warning (disable : 4290) // throw 例外無効警告を消す。

} // namespace interpreter
} // namespace gscripter

