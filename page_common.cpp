#include "page_common.h"
#include "util.h"
#include "local_debug.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "setup.h"
#include <stdlib.h>

PageCommon::PageCommon( const std::string & name )
: outfile()
{
    outfile = CONFIG.load_config_entry( name, "outfile", "/dev/null" );
    VOUT(0)( "outfile=%s\n", outfile );    
}

std::string PageCommon::exec_command( const std::string & command, bool silent )
{
    VOUT(0)( "exec: %s\n", command );
    
    FILE *file = popen( command.c_str(), "r");
    
    if( !file )
    {
        warning( format("cannot execute command: '%s' error: %s\n", command, strerror(errno)) );
        return "";
    }
    
    char acBuffer[1024];
    acBuffer[0]='\0';
    
    std::string res;
    
    do
    {
        fgets( acBuffer, sizeof(acBuffer)-1, file );            
        
        acBuffer[sizeof(acBuffer)-1]='\0';
        res += acBuffer;        
    } while( !feof(file) );
    
    VOUT(0)( res + "\n");
    
    int ret = pclose( file );
    
    
    if( ret != 0 ) {
        if( !silent )
                warning( format("return value of command '%s' was '%d'", command, ret) );
    }
    
    return res;
}

void PageCommon::fork_and_exec( const std::string & command )
{
    std::vector<std::string> sl = split_and_strip_simple(command);   
    
    if( fork() == 0 )
    {
        std::vector<char*> argv_array;

        argv_array.resize(sl.size() + 1);
        argv_array[sl.size()] = 0;

        for (unsigned i = 0; i < sl.size(); i++)
            argv_array[i] = strdup(sl[i].c_str());
        
        
        freopen( outfile.c_str(), "a+", stdout );
        freopen( outfile.c_str(), "a+", stderr );

        if( execv( sl[0].c_str(), &argv_array[0] ) == -1 )
        {
            VOUT(0)(format("error: %s", strerror(errno)));
            exit(1);
        }               
    }
}

void PageCommon::fork_and_exec( const std::string & command, const std::string & name )
{
    std::vector<std::string> sl = split_and_strip_simple(command);   
    
    if( fork() == 0 )
    {
        std::vector<char*> argv_array;

        argv_array.resize(sl.size() + 1);
        argv_array[sl.size()] = 0;

        for (unsigned i = 0; i < sl.size(); i++)
            argv_array[i] = strdup(sl[i].c_str());
        
        std::ofstream xout( outfile.c_str(), std::ios_base::trunc );
        
        xout << name << std::endl;
        xout.close();
        
        freopen( outfile.c_str(), "a+", stdout );
        freopen( outfile.c_str(), "a+", stderr );                
        

        if( execv( sl[0].c_str(), &argv_array[0] ) == -1 )
        {
            VOUT(0)(format("error: %s", strerror(errno)));
            exit(1);
        }               
    }
}