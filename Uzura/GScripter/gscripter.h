/*
 *	ガーネット・スクリプター
 *	gscripter
 *	2011 / 06 / 06
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_H_INCLUDE_GUARD_402_48293_666_192_111_000__
#define __GSCRIPTER_GSCRIPTER_H_INCLUDE_GUARD_402_48293_666_192_111_000__

#include	<string>

#include	"gscripter_loader.h"
#include	"gscripter_lex.h"
#include	"gscripter_compiler.h"
#include	"gscripter_interpreter.h"

namespace gscripter
{

// loader が読み込んだファイルを渡して操作？
//class lex;
//class compiler;
//class interpreter;

// main

// 旧コーディング規約。
class scripter;

// 現在のコーディング規約バージョン。
// と言っても、ラップしただけ。
class Scripter;

class scripter
{
public:
	// スクリプト読み込み 成功 true 失敗 false
	bool		load(const char* url_file);
	bool		load(const std::string& url_file);

	// 字解析、成功 true 失敗 false
	bool		lexical_analysis(void);

	// コンパイル 成功 true 失敗 false
	bool		compile(void);

	// 実行。
	bool		run(void);

	// 初期化
	void		clear(void) { this->reset(); };
	void		reset(void);

//	get
	double		get_value(const char* vname); // 変数の数値を返す。
	std::string	get_string(const char* vname);
//	set
	void		set_value(const char* vname, const double value); // 変数を作成、すでに存在すれば、中身の変更。
	void		set_string(const char* vname, const std::string& str);

//	native function の追加 ( local )
	void		set_function(
				const std::string& func_name,
				compiler::code (*ptr_function) (interpreter::interpreter* this_interpreter, const std::vector<compiler::code>& argv));
//	native function の破棄 ( local )
	void		del_function(const std::string& func_name);

//	flag
	bool		is_load(void) const { return (this->m_flag_load); };
	bool		is_lex(void) const { return (this->m_flag_lexical_analysis); };
	bool		is_compile(void) const { return (this->m_flag_compile); };

//	vm flag
	bool		is_script_end(void) const; // スクリプトが END を迎えているか否か、

//	vm control
	void		vm_reset(void); // VM の初期化
	void		vm_reset_variable(void); // 変数のみ初期化。

	scripter();
virtual	~scripter();

protected:
	loader		m_loader;
	lex::lex	m_lexer;
compiler::compiler	m_compiler;
interpreter::interpreter	m_interpreter;

	bool		m_flag_load;
	bool		m_flag_lexical_analysis;
	bool		m_flag_compile;

};

class Scripter :public scripter
{
public:
	// 字解析、成功 true 失敗 false
	bool		lex(void) { return (this->lexical_analysis()); };

//	get
	double		getValue(const char* vname)
			{
				return (this->get_value(vname));
			}
	std::string	getString(const char* vname)
			{
				return (this->get_string(vname));
			}

//	set
	void		setValue(const char* vname, const double value)
			{
				this->set_value(vname, value);
			}
	void		setString(const char* vname, const std::string& str)
			{
				this->set_string(vname, str);
			}

//	native function の追加 ( local )
	void		setFunction(
				const std::string& func_name,
				compiler::code (*ptr_function) (interpreter::interpreter* this_interpreter, const std::vector<compiler::code>& argv))
			{
				this->set_function(func_name, ptr_function);
			}

	void		attachFunction(
				const std::string& funcName,
				compiler::code (*ptrFunction) (interpreter::interpreter* this_interpreter, const std::vector<compiler::code>& argv))
			{
				this->set_function(funcName, ptrFunction);
			}

//	native function の破棄 ( local )
	void		delFunction(const std::string& func_name) { this->del_function(func_name); };
	void		detachFunction(const std::string& funcName) { this->delFunction(funcName); };

//	flag
	bool		isLoad(void) const { return (this->is_load()); };
	bool		isLex(void) const { return (this->is_lex()); };
	bool		isCompile(void) const { return (this->is_compile()); };

//	vm flag
	bool		isScriptEnd(void) const { return (this->is_script_end()); }; // スクリプトが END を迎えているか否か、

//	vm control
	void		vmReset(void) { this->vm_reset(); };
	void		vmResetVariable(void) { this->vm_reset_variable(); };

	Scripter() {};
virtual	~Scripter() {};


};

} // namespace gscripter


#endif // __GSCRIPTER_GSCRIPTER_H_INCLUDE_GUARD_402_48293_666_192_111_000__

