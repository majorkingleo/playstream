/* 
 * File:   play_music.h
 * Author: martin
 *
 * Created on 25. August 2013, 20:00
 */

#ifndef PLAY_MUSIC_H
#define	PLAY_MUSIC_H

#include "page.h"

class PlayMusic : public Page
{
 private:

    std::string outfile;	 

 public:
    PlayMusic();

    std::string display();
    
 private:
        
    bool stop_or_play( const std::string & start_command, const std::string & stop_command, const std::string & check_command );
        
    std::string exec_command( const std::string & command );
    void fork_and_exec( const std::string & command );

};

#endif	/* PLAY_MUSIC_H */

