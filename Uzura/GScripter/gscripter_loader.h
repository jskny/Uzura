/*
 *	ガーネット・スクリプター
 *	loader
 *	2011 / 06 / 06
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_LOADER_H_INCLUDE_GUARD_6_624343_12343_2357865008__
#define __GSCRIPTER_GSCRIPTER_LOADER_H_INCLUDE_GUARD_6_624343_12343_2357865008__

#include	<string>
#include	<vector>

namespace gscripter
{

class loader;

class loader
{
public:
	bool		load(const char* url_file);
	bool		load(const std::string& url_file);

	void		clear(void);

//	flag
	bool		is_load(void) const;

//	get
	long int	get_line_max(void) const;
std::string		get_line(const unsigned long int line) throw (const char*); // vector.at(line)

	// ↓最終手段↓
std::vector<std::string>	get_file(void);

	loader();
	~loader();

private:
std::vector<std::string>	m_file;

};

}


#endif // __GSCRIPTER_GSCRIPTER_LOADER_H_INCLUDE_GUARD_6_624343_12343_2357865008__
