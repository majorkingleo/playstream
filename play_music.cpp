#include <cgicc/FormEntry.h>

#include "play_music.h"
#include "setup.h"
#include "format.h"
#include "string_utils.h"
#include "setup.h"
#include <cgicc/Cgicc.h>
#include "util.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

PlayMusic::PlayMusic()
    : Page( "play_music" )     
{
    prefix = "page_music_";
    skin = "music.htm";
    
    bool found_something = false;
    
    for( cgicc::const_form_iterator it = SETUP.cgi->getElements().begin();
            it != SETUP.cgi->getElements().end();
            it++ )
    {
        VOUT(0)( "%s=%s\n", it->getName(), it->getValue() );                
        
        std::string start_command = CONFIG.load_config_entry(it->getName(), "start_command");
        std::string stop_command = CONFIG.load_config_entry(it->getName(), "stop_command");
        std::string check_command = CONFIG.load_config_entry(it->getName(), "check_command");
        
        if( start_command.empty() &&
                stop_command.empty() &&
                check_command.empty())
        {                    
                    continue;
        }       
        
        stop_or_play( start_command, stop_command, check_command );
        
        found_something = true;
    }        
    
    if (!found_something )
    {
        warning( "no commands found in ini section" );
    }
}

std::string PlayMusic::display()
{
    std::string ret = header() + load_skin( skin );

    return ret;
}


bool PlayMusic::stop_or_play( const std::string & start_command, const std::string & stop_command, const std::string & check_command )
{
    std::string check = exec_command( check_command );
    
    // if( !check.empty() )
        exec_command( stop_command );
    
    fork_and_exec( start_command );
    
    return true; 
}

std::string PlayMusic::exec_command( const std::string & command )
{
    VOUT(0)( "exec: %s", command );
    
    FILE *file = popen( command.c_str(), "r");
    
    if( !file )
    {
        warning( format("cannot execute command: '%s' error: %s", command, strerror(errno)) );
        return "";
    }
    
    char acBuffer[1024];
    acBuffer[0]='\0';
    
    std::string res;
    
    while( !feof(file) )
    {
        size_t nbytes = fread( acBuffer, sizeof(acBuffer)-1, 1, file );
        
        if( nbytes > 0 )
        {
            acBuffer[nbytes] = '\0';
                
            res += acBuffer;
        }
    }
    
    VOUT(0)( res );
    
    int ret = pclose( file );
    
    /*
    if( ret != 0 ) {
        warning( format("return value of command '%s' was '%d'", command, ret) );
    }*/
    
    return res;
}

void PlayMusic::fork_and_exec( const std::string & command )
{
    std::vector<std::string> sl = split_and_strip_simple(command);   
    
    if( fork() == 0 )
{
        std::vector<char*> argv_array;

        argv_array.resize(sl.size() + 1);
        argv_array[sl.size()] = 0;

        for (unsigned i = 0; i < sl.size(); i++)
            argv_array[i] = strdup(sl[i].c_str());
        
        
        freopen( "/dev/null", "a+", stdout );
        freopen( "/dev/null", "a+", stderr );

        /*
        freopen( "/tmp/exec.out", "a+", stdout );
        freopen( "/tmp/exec.out", "a+", stderr );
        */
        
        if( execv( sl[0].c_str(), &argv_array[0] ) == -1 )
        {
            VOUT(0)(format("error: %s", strerror(errno)));
            exit(1);
        }               
    }
}