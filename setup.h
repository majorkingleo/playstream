#ifndef SETUP_H
#define SETUP_H

#include "database.h"
#include "page.h"

#include "leoini.h"

using namespace Tools;

namespace cgicc {
    class Cgicc;
    class HTTPHTMLHeader;
}

struct Config
{
    bool        count_cars;
    bool        count_users;
    bool        count_reload;
    std::string db_user;
    std::string db_passwd;
    bool        render_time;
    bool        cache_config;
    bool        fcgi_autodetect;
    bool        auto_restart;
    time_t      filedate;
    time_t      conf_filedate;
    time_t      touch_filedate;
    const std::string touch_file;
    const std::string config_file;
    
private:
    Leo::Ini    ini;
    
public:
    
    template <typename T> void load_ini( const std::string &key, T &val )
	{
	    Leo::Ini::Element el( Leo::Ini::Element::TYPE::KEY, "global", key );

	    if( ini.read( el ) )
	    {
		val = s2x<T>( el.value );
	    }
	}

    void load_ini( const std::string &key, std::string &val );  
    
    Config();
    
    std::string load_config_entry( const std::string & secion, const std::string & key, const std::string & default_value = "" );
	  
};

extern Config CONFIG;

class Setup
{
 public:
    Setup();
    ~Setup();
    Setup( const Setup &se );

    Setup& operator=( const Setup &se );

    cgicc::Cgicc *cgi;
    std::string page; 
    unsigned long start_time;
    std::vector<std::string> warnings;
    cgicc::HTTPHTMLHeader* header;
    bool cache_config;
    std::string touch_file;

    std::string get_warnings() const;
    void set_cookie( const std::string &name, const std::string &value );
    std::string get_header();    

};

extern Setup SETUP;

#endif
