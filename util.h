#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <exception>
#include <vector>
#include "backtrace.h"

const double FAKTOR_PS_KW = 0.7354987;

class BaseError : std::exception
{
    std::string err;

 public:
    BaseError( const std::string &err_ ) : err( err_ ) {}
    virtual ~BaseError() throw() {}

    virtual const char* what() const throw() { return err.c_str(); }
};


#define die( x ) really_die( __FILE__, __LINE__, x )
#define warning( x ) really_warning( __FILE__, __LINE__, x )

void really_die( const std::string &file, int line, const std::string &message );
void really_warning( const std::string &file, int line, const std::string &message );

bool send_mail( const std::string &sendmail, const std::string &to, const std::string &subject, const std::string &message,
		const std::string &reply_to = std::string() );
std::string create_activation_code();
std::string convtime( time_t t );
std::string convtime_day( time_t t );
int get_month( time_t t );
int get_year( time_t t );
std::string local_prepand(std::string str, std::string what, std::string prefix);
std::string conv_tel( const std::string & tel );
unsigned long get_microtime();

time_t get_file_date( const char *file );
void touch_file( const std::string &file );
bool is_admin();
std::string deny_access();
std::string ps2kw( const std::string &ps );
std::string kw2ps( const std::string &kw );
void load( std::vector<std::string>::iterator & end, std::vector<std::string>::iterator & it, std::string &what );
void store( std::string &s, const std::string &what );

#endif
