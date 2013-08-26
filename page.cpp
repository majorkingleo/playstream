#include "page.h"
#include "setup.h"
#include "format.h"
#include "cppdir.h"
#include "util.h"
#include "string_utils.h"
#include <fstream>
#include <cgicc/Cgicc.h>

using namespace Tools;

Page::Page( const std::string &name_ )
    : name( name_ ),
      prefix( name_ + '_' )
{
    BT;
    skin_dir   = CONFIG.load_config_entry( "page_all", "skin_dir", "skins" );
    skin       = CONFIG.load_config_entry( name, "skin" );
    skin_index = CONFIG.load_config_entry( "page_all", "skin_index", "index.htm" );
    host_url   = CONFIG.load_config_entry( "page_all", "host_url", "http://localhost" );
    host_dir   = CONFIG.load_config_entry( "page_all", "host_dir", "auto" );    
    sendmail   = CONFIG.load_config_entry( "page_all", "sendmail", "sendmail" );
    convert    = CONFIG.load_config_entry( "page_all", "convert", "convert" );
    image_dir  = CONFIG.load_config_entry( "page_all", "image_dir", "/srv/auto/car_images/" );
    image_url  = CONFIG.load_config_entry( "page_all", "image_url", "http://localhost/auto/car_images" );
}

Page::~Page()
{

}

std::string Page::header()
{
    // return "Content-type: text/html\n\n";
    return SETUP.get_header();
}


std::string Page::load_simple_skin( const std::string &skin_name )
{
    BT;
    std::string skin_file = CppDir::concat_dir( skin_dir, skin_name );

    VOUT(0)( "Loading: %s\n", skin_file);
    
    std::ifstream in( skin_file.c_str() );
    
    if( !in )
	throw BaseError( format( "cannot load skin: '%s' at dir: '%s'", skin_name, skin_file) );
    
    std::string ret;

    while( !in == false )
    {
	std::string s;
	getline( in, s );
	ret += s + '\n';
    }    

    std::string base_url = host_url + '/' + host_dir + '/';

    ret = local_prepand( ret, ".jpg", base_url );
    ret = local_prepand( ret, ".jpeg", base_url );
    ret = local_prepand( ret, ".css", base_url );
    ret = local_prepand( ret, ".gif", base_url );
    ret = local_prepand( ret, ".js", base_url );

    return ret;
}

std::string Page::load_skin( const std::string &skin_name )
{
    BT;
    std::string ret = load_simple_skin( skin_index );
    
    std::string s = load_simple_skin( skin_name );
    
    std::vector<std::string> sl = split_string( s, "TEMPLATE_PAGE", 3 );

    if( sl.size() != 3 )
	die( format( "Invalid skin definition: %d. TEMPLATE_PAGE missing?", sl.size() ) );

    ret = substitude( ret, "TEMPLATE_INDEX", sl[1] );
    
    return ret;
}

