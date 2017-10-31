/*
 *	ガーネット・スクリプター
 *	gscripter
 *	2011 / 06 / 06
 *	jskny
*/

#include	<iostream>
#include	<string>

#include	"gscripter.h"
#include	"gscripter_def.h"

using namespace std;

namespace gscripter
{

//---------------------------------------------------------
// scripter
//---------------------------------------------------------

scripter::scripter() :
	m_flag_load(false),
	m_flag_lexical_analysis(false),
	m_flag_compile(false)
{
	this->m_loader.clear();
	this->m_lexer.clear();
	this->m_compiler.clear();
	this->m_interpreter.clear();
}

scripter::~scripter()
{
}

void scripter::reset(void)
{
	this->m_loader.clear();
	this->m_lexer.clear();
	this->m_compiler.clear();
	this->m_interpreter.clear();

	this->m_flag_load = false;
	this->m_flag_lexical_analysis = false;
	this->m_flag_compile = false;

	return;
}

//---------------------------------------------------------
// 読み込み処理
//---------------------------------------------------------

bool scripter::load(const char* url_file)
{
	if (!url_file) {
GSCRIPTER_ERROR("GS >> null pointer.");
		return (false);
	}


	// 初期化。
	this->reset();
	if (!this->m_loader.load(url_file)) {
GSCRIPTER_ERROR("GS >> can't find script file.");
		return (false);
	}

	this->m_flag_load = true;
	return (true);
}

bool scripter::load(const std::string& url_file)
{
	if (!this->load(url_file.c_str())) {
GSCRIPTER_ERROR("GS >> can't find script file.");
		return (false);
	}

	return (true);
}

//---------------------------------------------------------
// レキシカルアナライズ
//---------------------------------------------------------

bool scripter::lexical_analysis(void)
{
	if (this->is_load()) {
		this->m_lexer.set(&this->m_loader);
	}
	else {
GSCRIPTER_ERROR("GS >> not load 'script file'.");
		return (false);
	}

	if (!this->m_lexer.lexical_analysis()) {
GSCRIPTER_ERROR("GS >> can't finish lexical_analysis.");
		return (false);
	}

	this->m_flag_lexical_analysis = true;
	return (true);
}

//---------------------------------------------------------
// コンパイル処理
//---------------------------------------------------------

bool scripter::compile(void)
{
	if (this->is_lex()) {
		this->m_compiler.set(&this->m_lexer);
	}
	else {
GSCRIPTER_ERROR("GS >> not lexical_analysis object.");
		return (false);
	}

	if (!this->m_compiler.compile()) {
GSCRIPTER_ERROR("GS >> can't finish compile.");
		return (false);
	}

	this->m_flag_compile = true;
	return (true);
}

//---------------------------------------------------------
// 実行処理
//---------------------------------------------------------

bool scripter::run(void)
{
	if (!this->is_load()) {
GSCRIPTER_ERROR("GS >> スクリプトファイルが読み込まれていないため、処理の続行が不可能です。");
		return (false);
	}

	if (!this->is_compile()) {
		if (!this->lexical_analysis()) {
GSCRIPTER_ERROR("GS >> レキシカルアナライズ中にエラーが発生しました。");
			return (false);
		}

		if (!this->compile()) {
GSCRIPTER_ERROR("GS >> コンパイル中にエラーが発生しました。");
			return (false);
		}
	}

	if (!this->m_interpreter.is_set_codes()) {
		this->m_interpreter.set(&this->m_compiler);
	}

	this->m_interpreter.run(); // void run()

	return (true);
}

//---------------------------------------------------------
// 変数の set and get.
//---------------------------------------------------------

double scripter::get_value(const char* vname)
{
	return (interpreter::get_variable_value(&this->m_interpreter, vname));
}

std::string scripter::get_string(const char* vname)
{
	return (interpreter::get_variable_str_value(&this->m_interpreter, vname));
}

void scripter::set_value(const char* vname, const double value)
{
	interpreter::set_variable_value(&this->m_interpreter, vname, value);
}

void scripter::set_string(const char* vname, const std::string& str)
{
	interpreter::set_variable_str_value(&this->m_interpreter, vname, str);
}

//---------------------------------------------------------
// vm flag
//---------------------------------------------------------

// スクリプトが END を迎えているか否か、
bool scripter::is_script_end(void) const
{
	return (this->m_interpreter.get_flag_end());
}

//---------------------------------------------------------
// vm control
//---------------------------------------------------------

void scripter::vm_reset(void)
{
	this->m_interpreter.clear();
}

void scripter::vm_reset_variable(void)
{
	this->m_interpreter.reset_variable();
}

// native function の追加 ( local )
void scripter::set_function(
	const std::string& func_name,
	compiler::code (*ptr_function) (interpreter::interpreter* this_interpreter, const std::vector<compiler::code>& argv))
{
	interpreter::gscripter_user_reg_native_faction a;
	a.set_function(ptr_function);
	a.set_func_name(func_name);
	this->m_interpreter.set_function(a);
	return;
}

// native function の破棄 ( local )
void scripter::del_function(const std::string& func_name)
{
	this->m_interpreter.del_function(func_name);
}

} // namespace gscripter

