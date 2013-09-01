#include "song_info.h"
#include <cgicc/Cgicc.h>
#include "setup.h"
#include "play_music.h"

SongInfo::SongInfo()
: Page("song_info"),
  PageCommon("play_music"), 
  value(),
  current_song(),
  current_modul()
{           
    for( cgicc::const_form_iterator it = SETUP.cgi->getElements().begin();
            it != SETUP.cgi->getElements().end();
            it++ )
    {                
        VOUT(0)( "%s=%s\n", it->getName(), it->getValue() );    
        
        if( it->getName() == "value" )
        {
            value = it->getValue();
        }
    }
}


std::string SongInfo::display()
{
    PlayMusic::read_outfile(outfile, current_song, current_modul );
    
    VOUT(0)( "song: %s modul: %s\n", current_song, current_modul );
    
    std::string ret = "Content-Type: text/html\n\n";
    
    if( value=="song") {        
        ret += current_song;
        return ret;
    }
    
    return ret + current_modul;
}

