/*
 *	ガーネット・スクリプター
 *	loader
 *	2011 / 06 / 06
 *	jskny
*/

#include	<iostream>
#include	<fstream>
#include	<string>
#include	<vector>

#include	"gscripter_loader.h"
#include	"gscripter_util.h"

using namespace std;

namespace gscripter
{

//---------------------------------------------------------
// 読み込み本体
//---------------------------------------------------------

// 最大横幅
static const unsigned long int SCRIPT_FILE_LOAD_MAX_LINE = 0xFF;

static std::vector<std::string> load_file(const char* filename) throw (const char*)
{
	std::vector <std::string> ret_vector;
	ifstream handle_file(filename, ios::in);
	char* buf_load = 0;
	string s;

	if (!handle_file) {
		return (ret_vector);
	}

	// メモリの確保
	buf_load = new char[SCRIPT_FILE_LOAD_MAX_LINE];
	if (!buf_load) {
		throw ("cat't new of memory...");
		return (ret_vector);
	}

try {
	while (!handle_file.eof())
	{
		handle_file.getline(buf_load, SCRIPT_FILE_LOAD_MAX_LINE);
		s = string(buf_load);

		// コメント消去
		s = remove_comment(s);
		// 改行を消す
		s = remove_last_CRLF(s);

		if (compare_string(s, "\t")) {
			continue;
		}
		else if (s.length() > 0) {
			ret_vector.push_back(s);
			continue;
		}
	}

	handle_file.close();
}
catch (...)
{
	delete[] buf_load;
	buf_load = 0;

	if (handle_file.is_open()) {
		handle_file.close();
	}

	throw ("loader -> loading error. ( level : user )");
}

	delete[] buf_load;
	buf_load = 0;

	return (ret_vector);
}

//---------------------------------------------------------
// loader
//---------------------------------------------------------

bool loader::load(const char* url_file)
{
	try {
		this->m_file = load_file(url_file);
	} catch (const char* err) {
		cerr << "error : " << err << endl;
	}

	return (!this->m_file.empty());
}

bool loader::load(const std::string& url_file)
{
	return (this->load(url_file.c_str()));
}

bool loader::is_load(void) const
{
	return (!this->m_file.empty());
}

void loader::clear(void)
{	
	this->m_file.clear();
	return;
}

//---------------------------------------------------------

std::string loader::get_line(const unsigned long int line) throw (const char*)
{
	if (line > this->m_file.size()) {
		throw ("bufferover run.");
	}
	else if (line < 0) {
		throw ("bufferover run.");
	}
	else if (this->m_file.empty()) {
		throw ("bufferover run.");
	}

	return (this->m_file.at(line));
}

long int loader::get_line_max(void) const
{
	return (this->m_file.size());
}

std::vector<std::string> loader::get_file(void)
{
	return (this->m_file);
}

//---------------------------------------------------------

loader::loader()
{
	this->m_file.clear();
}

loader::~loader()
{
	this->m_file.clear();
}

} // namespace gscripter

