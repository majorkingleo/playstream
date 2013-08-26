#ifndef LOCAL_DEBUG_H
#define LOCAL_DEBUG_H
#include "format.h"
#include "string_utils.h"
#include <fstream>
#include <range.h>

using namespace Tools;

extern std::ofstream fvout;

extern Format::PrintF<std::ostream> out;
extern Format::PrintF<std::ostream> vout;
extern Format::PrintF<std::ostream> eout;


struct EnumModule {
  enum ETYPE {
    FIRST__ = - 2,
    ALL  = -1,
    ARG  = 1,
    CPPDIR,
    SETUP_CONFIG,
    PAGES,
    LAST__
  };
};

typedef EnumRange<EnumModule> MODULE;

MODULE::ETYPE string2module( std::string module );

std::string x2s( MODULE module );

extern std::string progname;

#define ERROR eout( MODULE::ALL )[0]( "%s: ", progname )[0]

#ifdef NDEBUG
#  // define DEBUG( expr )
#  define DEBUG_OUT( level, module ) out(module)[level]
#  define DEBUG_VOUT( level, module ) vout(module)[level]
#else
#  // define DEBUG( expr ) expr
#  define DEBUG_OUT( level, module ) out(module)[level]( "%s:%s: ", __FILE__, __LINE__ )[level]
#  define DEBUG_VOUT( level, module ) vout(module)[level]( "%s:%s: ", __FILE__, __LINE__ )[level]
#endif

#define OUT(level) DEBUG_OUT( level, MODULE::PAGES )
#define VOUT(level) DEBUG_VOUT( level, MODULE::PAGES )

#endif
