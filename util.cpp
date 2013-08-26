#include "util.h"
#include <fstream>
#include "format.h"
#include <iostream>
#include "debug.h"
#include "setup.h"
#include "string_utils.h"
#include <time.h>
#include <sys/time.h>
#include "cppdir.h"
#include <math.h>
#include <stdlib.h>

void really_die( const std::string &file, int line, const std::string &message )
{
    std::cout << "Content-type: text/html\n\n";
    std::cout << "<br>BackTrace: <br>\n" << BackTraceHelper::bt.bt();
    std::cout << format( "<br>%s:%d: <b>%s</b><br>", file, line, message ) << std::endl;
    VOUT(0)( "BackTrace: \n%s\n", BackTraceHelper::bt.bt() );
    VOUT(0)( "died at: %s:%d: %s\n", file, line, message );
    exit( 1 );
}

void really_warning( const std::string &file, int line, const std::string &message )
{
    VOUT(0)( "BackTrace: \n%s\n", BackTraceHelper::bt.bt() );
    VOUT(0)( "warning at: %s:%d: %s\n", file, line, message );

    SETUP.warnings.push_back( BackTraceHelper::bt.bt() );
    SETUP.warnings.push_back( format( "<br>%s:%d: <b>%s</b><br>", file, line, message ) );
}

bool send_mail( const std::string &sendmail, const std::string &to, const std::string &subject, const std::string &message, 
		const std::string &reply_to )
{
    std::string cmd;

    if( reply_to.empty() )
	cmd = format( "%s %s <<EOF\nFrom: noreply@austromobil.at\nTo: %s\nSubject: %s\n%s\nEOF", sendmail, to, to, subject, message );
    else
	cmd = format( "%s %s <<EOF\nFrom: noreply@austromobil.at\nTo: %s\nSubject: %s\nReply-To: %s\n%s\nEOF", sendmail, to, to, subject, reply_to, message );

    VOUT(0)( "%s\n", cmd );

    return system( cmd.c_str() ) == 0; 
}

std::string create_activation_code()
{
    std::ifstream in( "/dev/urandom" );

    if( !in )
	die( "cannot read random numbers from /dev/urandom" );

    std::string ret;

    for( int i = 0; i < 255; i++ )
    {
	int c = in.get();
	
	if( c < 0 )
	    c *= -1;

	ret += format( "%X", c );

	if( ret.size() >= 50 )
	    break;
    }

    if( ret.size() >= 50 )
	ret = ret.substr( 0, 50 );

    return ret;
}

std::string convtime( time_t t )
{
    struct tm *tm;

    tm = localtime( &t );

    std::string ret;

    ret = format("%02d", tm->tm_mon + 1 ) + '/';
    ret += x2s( tm->tm_year + 1900 ).substr( 2 );

    return ret;
}

int get_year( time_t t )
{
    struct tm *tm;

    tm = localtime( &t );
    return tm->tm_year + 1900;
}


int get_month( time_t t )
{
    struct tm *tm;

    tm = localtime( &t );
    return tm->tm_mon + 1;
}


std::string conv_tel( const std::string & tel )
{
    std::string ret;

    for( unsigned i = 0; i < tel.size(); i++ )
    {
	if( tel[i] >= '0' && tel[i] <= '9' )
	    ret += tel[i];
    }

    return ret;
}

unsigned long get_microtime()
{
    struct timeval tv1;
    struct timezone tz1;

    gettimeofday(&tv1,&tz1);

    return (unsigned long)(tv1.tv_sec * 1000 + tv1.tv_usec / 1000) ;
}


time_t get_file_date( const char *file )
{
    CppDir::File f( file );

    if( !f )
	return 0;

    return f.get_date();
}

void touch_file( const std::string &file )
{
    std::ofstream out_( file.c_str() );

    if( !out_ )
	return;

    out_ << 1;
}

std::string ps2kw( const std::string &ps )
{
    return x2s( static_cast<int>( ceil( s2x<double>( ps ) * FAKTOR_PS_KW ) ) );		
}

std::string kw2ps( const std::string &kw )
{
     return x2s( static_cast<int>( ceil( s2x<double>( kw ) / FAKTOR_PS_KW ) ) );
}

void store( std::string &s, const std::string &what )
{
  VOUT(0)( "<< %s\n", what );
  s += bin_encode( what ) + '|';
}

void load( std::vector<std::string>::iterator & end, std::vector<std::string>::iterator & it, std::string &what )
{
    if( end == it )
    {
	VOUT(0)( "warning: end reached!\n" );
	return;
    }

    what = bin_decode( *it );
    VOUT(0)( "decoded: %s\n", what );
    VOUT(0)( ">> %s\n", what );
    it++;
}

std::string convtime_day( time_t t )
{
    struct tm *tm;

    tm = localtime( &t );

    std::string ret;

    ret = format("%02d", tm->tm_mday ) + '/';
    ret += format("%02d", tm->tm_mon + 1 ) + '/';
    ret += x2s( tm->tm_year + 1900 ).substr( 2 );

    return ret;
}

std::string local_prepand(std::string str, std::string what, std::string prefix)
{
    std::string::size_type pos = 0, start1 = 0, start2 = 0, start = 0, start3 = 0;

    for (;;) {
        pos = str.find(what, pos);

        if (pos == std::string::npos)
            break;

        start1 = str.rfind('\"', pos);
        start2 = str.rfind('=', pos);
        start3 = str.rfind("\n", pos);
        
        if( start1 != std::string::npos &&
            start2 != std::string::npos )
        {
            if( start3 > start1 ||
                start3 > start2 ) {
                pos += what.size();
                continue;
            }
        }
        
        if( start1 != std::string::npos && start1+1 == str.find("http",start1) )
        {
            pos += what.size();
            continue;
        }          

        if (start1 == std::string::npos && start2 != std::string::npos)
            start = start2;
        else if (start2 == std::string::npos && start1 != std::string::npos)
            start = start1;
        else if (start1 == std::string::npos && start2 == std::string::npos)
            break;
        else if (start1 > start2)
            start = start1;
        else
            start = start2;

        str.insert(start + 1, prefix);
        pos += prefix.size();
        pos++;
    }
    
    return str;
}

 
