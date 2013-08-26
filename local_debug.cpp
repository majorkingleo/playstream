#include "local_debug.h"
#include "string_utils.h"
#include <fstream>

std::ofstream fvout( "/tmp/playstream", std::ios_base::app );

Format::PrintF<std::ostream> out( std::cout );
Format::PrintF<std::ostream> vout( fvout );
Format::PrintF<std::ostream> eout( std::cerr );

std::string progname;

MODULE::ETYPE string2module( std::string m )
{
  m = toupper( m );

  if( m == "ALL" )
    return MODULE::ALL;

  if( m == "ARG" )
    return MODULE::ARG;       

  if( m == "CPPDIR" )
    return MODULE::CPPDIR;
  
  if( m == "SETUP_CONFIG" )
    return MODULE::SETUP_CONFIG;

  out[0]( "unkown module name \"%s\". Using default value ALL\n", m );

  return MODULE::ALL;
}

std::string x2s( MODULE module )
{
  switch( module )
    {
    case MODULE::ALL:          return "ALL";
    case MODULE::ARG:          return "ARG";
    case MODULE::CPPDIR:       return "CPPDIR";
    case MODULE::SETUP_CONFIG: return "SETUP_CONFIG";
    case MODULE::PAGES:        return "PAGES";
    case MODULE::FIRST__:
    case MODULE::LAST__:
	break;
    }

  return "UNKNOWN";
}


