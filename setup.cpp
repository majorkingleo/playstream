#include "setup.h"
#include "string_utils.h"
#include <cgicc/Cgicc.h>
#include <cgicc/CgiEnvironment.h>
#include <cgicc/HTTPHeader.h>
#include <cgicc/HTTPHTMLHeader.h>

Setup SETUP;
Config CONFIG;

Config::Config()
	: count_cars( true ),
	  count_users( true ),
	  count_reload( true ),
	  db_user( "root" ),
	  db_passwd( "" ),
	  render_time( true ),
	  cache_config( false ),
	  fcgi_autodetect( false ),
	  auto_restart( false ),
	  filedate( 0 ),
	  conf_filedate( 0 ),
	  touch_filedate( 0 ),
	  touch_file( "/tmp/playstream.reconfig" ),
	  config_file( "/etc/playstream.ini" ),
          ini( config_file, std::ios_base::in )
	{
	    load_ini( "count_cars",      count_cars );
	    load_ini( "count_reload",    count_reload );
	    load_ini( "db_user",         db_user );
	    load_ini( "db_passwd",       db_passwd );
	    load_ini( "count_users",     count_users );
	    load_ini( "render_time",     render_time );
	    load_ini( "cache_config",    cache_config );
	    load_ini( "fcgi_autodetect", fcgi_autodetect );
	    load_ini( "auto_restart",    auto_restart );
	}

void Config::load_ini( const std::string &key, std::string &val) 
{
    Leo::Ini::Element el(Leo::Ini::Element::TYPE::KEY, "global", key);

    if (ini.read(el)) {
        val = strip(el.value);
    }
}  

std::string Config::load_config_entry( const std::string & section, const std::string & key, const std::string & default_value )
{
    Leo::Ini::Element el(Leo::Ini::Element::TYPE::KEY, section, key);

    if (ini.read(el)) {
        return strip(el.value);
    }    
    
    return default_value;
}

Setup::Setup()
    : cgi(0),
      start_time(0),
      header(0)     
{    
    header = new cgicc::HTTPHTMLHeader();
}

Setup::~Setup()
{
    delete header;
}

Setup::Setup( const Setup &se )
{
    delete header;
    header = new cgicc::HTTPHTMLHeader( *se.header );
    start_time = se.start_time;
    warnings = se.warnings;
    page = se.page;
    cache_config = se.cache_config;
    touch_file = se.touch_file;
}

Setup& Setup::operator=( const Setup &se )
{
    delete header;
    header = new cgicc::HTTPHTMLHeader( *se.header );
    start_time = se.start_time;
    warnings = se.warnings;
    page = se.page;
    cache_config = se.cache_config;
    touch_file = se.touch_file;

    return *this;
}

std::string Setup::get_warnings() const
{
    std::string ret;

    for( unsigned i = 0; i < warnings.size(); i++ )
	ret += warnings[i];

    return ret;
}

void Setup::set_cookie( const std::string &name, const std::string &value )
{
    header->setCookie( cgicc::HTTPCookie( name, value ) );
}

std::string Setup::get_header()
{
    return x2s(*header);
}

