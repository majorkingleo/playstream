#include <cgicc/FormEntry.h>

#include "play_music.h"
#include "setup.h"
#include "format.h"
#include "string_utils.h"
#include "setup.h"
#include <cgicc/Cgicc.h>
#include "util.h"
#include <xml.h>

using namespace Tools;


PlayMusic::PlayMusic()
    : Page( "play_music" ),
      PageCommon( "play_music" ),      
      read_mixer_command(),
      mixer_channel()
{
    prefix = "page_music_";
    skin = "music.htm";
    
    bool found_something = false;
    bool done_something = false;
    
    for( cgicc::const_form_iterator it = SETUP.cgi->getElements().begin();
            it != SETUP.cgi->getElements().end();
            it++ )
    {
        done_something = true;
        
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
        
        stop_or_play( start_command, stop_command, check_command, it->getName() );
        
        found_something = true;
    }               
    
    read_mixer_command = CONFIG.load_config_entry("mixer", "read_command" );            
    mixer_channel = CONFIG.load_config_entry("mixer", "channel" );     
    
    
    if (!found_something && done_something )
    {
        VOUT(0)( "no commands found in ini section" );
    }
}

std::string PlayMusic::display()
{
    read_outfile();
    
    std::string ret = header() + load_skin( skin );

    std::string mixer = read_mixer( read_mixer_command );
    
    ret = substitude(ret, "MIXER", mixer );
    ret = substitude(ret, "CURRENT_SONG", current_playing_song );
    ret = substitude(ret, "CURRENT_MODUL", current_playing_modul );
    
    return ret;
}


bool PlayMusic::stop_or_play( const std::string & start_command, const std::string & stop_command, 
                              const std::string & check_command, const std::string & modul )
{
    std::string check = exec_command( check_command, true );
    
    // if( !check.empty() )
        exec_command( stop_command, true );
    
    fork_and_exec( start_command, modul );
    
    return true; 
}



std::string PlayMusic::read_mixer( const std::string & read_command )
{        
    std::string ret = exec_command( read_command );         
    
    std::vector<std::string> sl = split_simple( ret, "\n" );
    
    std::string channel;
    std::string level;        
    
    std::string res;
    
    for( unsigned i = 0; i < sl.size(); i++ )
    {
        const std::string & line = sl[i];
        
        if( line.find( "control") != std::string::npos )
        {
            std::string::size_type start = line.find('\'');
            std::string::size_type end = line.rfind('\'');
            
            if( start != std::string::npos && 
                end != std::string::npos )
            {
                channel = line.substr( start+1, end - start - 1);
                level.clear();
            }
        }
        
        if( line.find("Playback" ) != std::string::npos )
        {
            std::string::size_type start = line.find('[');
            std::string::size_type end = line.find('%', start);
            
            if( start != std::string::npos && 
                end != std::string::npos )
            {
                level = line.substr( start+1, end - start - 1);                
                
                if( channel == mixer_channel )
                {
                    return level;
                }
                else
                {
                        res = format("%s=%s ", channel, level);                
                }
            }
        }
    }
    
    if( res.empty() ) {
        if( sl.size() > 0 ) {
            res =strip(sl[0]);
        } else {
           res = strip(ret);
        }
    }
    
    return res;    
}


void PlayMusic::read_outfile()
{
    read_outfile( outfile, current_playing_song, current_playing_modul);
}

void PlayMusic::read_outfile( const std::string & outfile, std::string & current_song, std::string & current_modul )
{
    std::string data;
    
    if( !XML::read_file( outfile, data ) ) {
        VOUT(0)("Cannot read " + outfile + "\n");
        return;
    }
    
    std::vector<std::string> sl = split_simple( data, "\n");
    
    if( sl.empty() ) {
        VOUT(0)("sl is empty\n");
        return;
    }
    
    current_modul = sl[0];
    
    std::string line = *sl.rbegin();
    
    VOUT(0)("last line: %s\n", line);
    
    std::string::size_type start = line.find('\'');
    std::string::size_type end = line.find('\'', start+1);
    
    if( start != std::string::npos && end != std::string::npos ) {
        current_song = line.substr( start+1, end - start - 1);
    }
}
