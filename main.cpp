#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <iostream>

#include "format.h"
#include "string_utils.h"
#include "setup.h"
#include "arg.h"
#include "debug.h"
#include "util.h"
#include "pages.h"
#include "leoini.h"
#include "cppdir.h"
#include "local_debug.h"

class VarIO
{
public:
    std::ostream *out;

    VarIO()
	: out( 0 ) {}

    VarIO( std::ostream *out_ )
	: out( out_ )
	{}

    template <class T> VarIO & operator<<( const T & t ) 
	{ 
	    if( out )
		*out << t;
	    return *this; 
	}

    VarIO & operator<<(std::ostream& (*f)(std::ostream&))
    {
	if( out )
	    *out << f;
	return *this;
    }
};

#if 0
template<typename T> VarIO & operator<<( VarIO &io, const T & sout )
{
    if( io.out )
	*io.out << sout;
    return io;
}
#endif

using namespace std;
using namespace Tools;

int main( int argc, char **argv )
{
    SETUP.start_time =  get_microtime();

#ifdef CODE_FINAL
    out.set_debug_level(5);
    vout.set_debug_level(5);
#else
    out.set_debug_level(0);
    vout.set_debug_level(0);
#endif

    out.set_module( MODULE::PAGES );
    vout.set_module( MODULE::PAGES );

    VOUT(0)( "========= start =============\n" );

    Arg::Arg arg( argc, argv );

    arg.addPrefix( "-" );
    arg.addPrefix( "--" );

    arg.setMinMatch( 0 );   

    Arg::StringOption opage( "p", "PAGE" );
    opage.addName( "page" );
    opage.setMinValues( 1 );
    opage.setMaxValues( 1 );
    opage.setDescription( "name of the page to display" );
    arg.addOptionR( &opage );

    if( !arg.parse() )
    {
	cout << "parsing args failed\n";
	return 1;
    }

    int load_count = 1;

    /*
    std::string s;

    while( getline( std::cin, s ) )
      {
	VOUT(0)( "%s\n", s );
      }

	return 0;
    */

	cgicc::Cgicc cgi;
	VarIO IO( &cout );

	VOUT(0)( cgi.getEnvironment().getPostData() );

	SETUP.cache_config = CONFIG.cache_config;
	SETUP.touch_file = CONFIG.touch_file;

	SETUP.cgi = &cgi;

	cgicc::form_iterator it_page = cgi.getElement( "page" );

	if( it_page != cgi.getElements().end() )
	{
	    SETUP.page = toupper(**it_page);
	} else {
	  VOUT(0)( "page not set\n" );

	  for(cgicc::const_form_iterator it = cgi.getElements().begin();  it != cgi.getElements().end(); it++ )
	    {
	      VOUT(0)( "%s=%s\n", it->getName(), it->getValue() );
	    }
	}
	
	if( opage.isSet() )
	    SETUP.page = toupper( *opage.getValues()->begin() );
		
	try {
	    Ref<Page> page = load_page();
	    
	    if( page.valid() )
	    {
		std::string ret = page->display();
		ret = substitude( ret, "TEMPLATE_WARNINGS", SETUP.get_warnings() );
		ret = substitude( ret, "TEMPLATE_RENDER_TIME", 
				  (CONFIG.render_time ? 
				   format( "%.3fsec %d", (get_microtime() - SETUP.start_time) / 1000.0, load_count++ ) :
				   std::string() ) );

		IO << ret << endl;
		
	    }
	} catch( BaseError &err ) {
	    IO << "Content-Type: text/html\n\n";
	    IO << "<b>ERROR: " << err.what() << "</b>" << endl;	 	
	    VOUT(0) << "ERROR: %s" << err.what() << endl;

	    return 1;

	} catch( std::exception &err ) {
	    IO << "Content-Type: text/html\n\n";
	    cout << "<b>ERROR: " << err.what() << "</b>" << endl;	 
	    VOUT(0) << "ERROR: " << err.what() << endl;

	    return 2;

	} catch( ... ) {
	    IO << "Content-Type: text/html\n\n";
	    IO << "<b>unknown error " << endl;
	    VOUT(0) << "ERROR: unknown error" << endl;

	    return 3;
	}


    VOUT(0)( "========= end =============\n" );
    return 0;   
}
