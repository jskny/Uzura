/*
 *	ガーネット・スクリプター
 *	インタープリタ
 *	2011 / 06 / 07
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_INTERPRETER_H_INCLUDE_GUARD_6731_32_2243690_745_01492__
#define __GSCRIPTER_GSCRIPTER_INTERPRETER_H_INCLUDE_GUARD_6731_32_2243690_745_01492__

#include	<string>
#include	<vector>
#include	<map>

#include	"gscripter_compiler.h"

namespace gscripter
{
namespace interpreter
{

class interpreter;
class exception_interpreter;
class gscripter_user_reg_native_faction;


#pragma warning (push)
#pragma warning (disable : 4290) // throw 例外無効警告を消す。


//---------------------------------------------------------
// VM 関連
//---------------------------------------------------------
extern	compiler::code	get_variable(interpreter* this_interpreter, const std::string& name);
extern	double		get_variable_value(interpreter* this_interpreter, const std::string& name);
extern	std::string	get_variable_str_value(interpreter* this_interpreter, const std::string& name);
extern	void		set_variable(interpreter* this_interpreter, const std::string& name, const compiler::code& obj);
extern	void		set_variable_value(interpreter* this_interpreter, const std::string& name, const double value);
extern	void		set_variable_str_value(interpreter* this_interpreter, const std::string& name, const std::string& str_value);

//---------------------------------------------------------

class exception_interpreter
{
public:
	exception_interpreter(
		const std::string& text,
		const long int point = -1,
		const compiler::code& code = compiler::code());

	void		output(void);

private:
const	std::string	m_text;
const	long int	m_point; // break したアドレス。
const	compiler::code	m_code;

};


// ユーザー定義の内蔵関数 関数ポインタを使う。
class gscripter_user_reg_native_faction
{
public:
	// 関数ポインタ
	void		set_function(compiler::code (*ptr_function) (interpreter* this_interpreter, const std::vector<compiler::code>& argv)) throw (exception_interpreter);
	compiler::code	run_function(interpreter* this_interpreter, const std::vector<compiler::code>& argv) throw (exception_interpreter);

	void		set_func_name(const std::string& func_name) { this->m_func_name = func_name; };
	std::string	get_func_name(void) const { return (this->m_func_name); };

	gscripter_user_reg_native_faction();

private:
	compiler::code	(*m_ptr_function) (interpreter* this_interpreter, const std::vector<compiler::code>& argv);
	std::string	m_func_name;

};

class interpreter
{
friend	void		push(interpreter* this_interpreter, const compiler::code& obj);
friend	compiler::code	pop(interpreter* this_interpreter);
friend	compiler::code	peek(interpreter* this_interpreter);
friend	compiler::code	peek1(interpreter* this_interpreter);

friend	compiler::code	call_native_function(
				interpreter* this_interpreter,
				const std::string& func_name,
				std::vector<compiler::code>* argv) throw (exception_interpreter);

friend	void		exec_if(interpreter* this_interpreter, const ::gscripter::compiler::code& obj_code) throw (exception_interpreter);
friend	void		exec_goto(interpreter* this_interpreter, const ::gscripter::compiler::code& obj_code) throw (exception_interpreter);
friend	void		exec_gosub(interpreter* this_interpreter, const ::gscripter::compiler::code& obj_code) throw (exception_interpreter);

friend	void		exec_one_code(interpreter* this_interpreter, const compiler::code &code) throw (exception_interpreter);
friend	void		exec(interpreter* this_interpreter) throw (exception_interpreter);

friend	compiler::code	get_variable(interpreter* this_interpreter, const std::string& name);
friend	double		get_variable_value(interpreter* this_interpreter, const std::string& name);
friend	std::string	get_variable_str_value(interpreter* this_interpreter, const std::string& name);
friend	void		set_variable(interpreter* this_interpreter, const std::string& name, const compiler::code& obj);
friend	void		set_variable_value(interpreter* this_interpreter, const std::string& name, const double value);
friend	void		set_variable_str_value(interpreter* this_interpreter, const std::string& name, const std::string& str_value);

public:
	explicit	interpreter();
	void		set(compiler::compiler* s_compiler);
	void		set(const std::vector<compiler::code>& vector_codes);

	// 実行
	void		run(void) throw (exception_interpreter);

	// 初期化
	void		clear(void) { this->reset(); };
	void		reset(void);
	// 変数表の初期化
	void		reset_variable(void) { this->map_variable.clear(); return; };

	// flag 関連
	void		set_flag_stop(const bool a) { this->flag_stop = a; };
	void		set_stop_counter(const long int a) { this->counter_stop = a; };

	// C++ 側から gscripter の実行を止めたい時に使う。
	bool		get_flag_stop(void) const { return (this->flag_stop); };
	long int	get_stop_counter(void) const { return (this->counter_stop); };

	// コードが読み込まれているか否か、
	bool		is_set_codes(void) const { return (!this->codes.empty()); };

	// end 文が実行されているか否か、
	bool		get_flag_end(void) const { return (this->flag_end); };
	void		set_flag_end(const bool a) { this->flag_end = a; };

//	native function の追加 ( local )
	void		set_function(const gscripter_user_reg_native_faction& n_func);
//	native function の破棄 ( local )
	void		del_function(const std::string& func_name);

private:
std::vector<compiler::code> codes; // 実行されるコード
std::vector<compiler::code> exc_stack; // バッファ
std::map<std::string, compiler::code> map_variable; // 配列表
std::stack<long int> save_gosub_return_point; // gosub 命令の帰ってくる位置を記録。
std::map<std::string, gscripter_user_reg_native_faction> map_native_function; // ユーザー定義の内蔵関数
unsigned long int	last_load; // codes から読み込むための i
unsigned long int	pointer_memory; // ポインタ座標。
	bool		flag_stop; // true の時に、 run が呼ばれても無視する。 ( counter_stop はデクリメントされない。 )
	bool		flag_end; // end文 が実行された時に true になる。 ( counter_stop はデクリメントされない。 )
	long int	counter_stop; // 0 以外の時に 0 になるまで run を無視する。 一回 run が呼ばれるごとにデクリメント。

};


#pragma warning (pop) // #pragma warning (disable : 4290) // throw 例外無効警告を消す。


} // namespace interpreter
} // namespace gscripter


#endif // __GSCRIPTER_GSCRIPTER_INTERPRETER_H_INCLUDE_GUARD_6731_32_2243690_745_01492__

