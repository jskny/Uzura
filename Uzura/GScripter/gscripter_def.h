/*
 *	ガーネット・スクリプター
 *	def
 *	2011 / 06 / 06
 *	jskny
*/

#ifndef __GSCRIPTER_GSCRIPTER_DEF_H_INCLUDE_GUARD_53_23397_16382103_11974928637716373_1130__
#define __GSCRIPTER_GSCRIPTER_DEF_H_INCLUDE_GUARD_53_23397_16382103_11974928637716373_1130__

#include	<iostream>
#include	<assert.h>

#define		GSCRIPTER_FLAG_DEBUG		(0)

#define		GSCRIPTER_ERROR(str)		std::cerr << str << std::endl

#if GSCRIPTER_FLAG_DEBUG == 1
	#define		GSCRIPTER_DEBUG(str)		std::cout << str << std::endl
#else
	#define		GSCRIPTER_DEBUG(str)
#endif


#endif // __GSCRIPTER_GSCRIPTER_DEF_H_INCLUDE_GUARD_53_23397_16382103_11974928637716373_1130__
