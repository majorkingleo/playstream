/* 
 * File:   play_music.h
 * Author: martin
 *
 * Created on 25. August 2013, 20:00
 */

#ifndef PLAY_MUSIC_H
#define	PLAY_MUSIC_H

#include "page_common.h"

class PlayMusic : public Page , PageCommon
{
 private:
    
    std::string read_mixer_command;
    std::string mixer_channel;
    std::string current_playing_modul;
    std::string current_playing_song;
    
 public:
    PlayMusic();

    std::string display();
    
 private:
        
    bool stop_or_play( const std::string & start_command, const std::string & stop_command, 
                       const std::string & check_command, const std::string & modul );
        

    std::string read_mixer( const std::string & read_command );
    
    void read_outfile();
    
  public:
    static void read_outfile( const std::string & outfile, std::string & current_song, std::string & current_modul );
    
    
};

#endif	/* PLAY_MUSIC_H */

