/* 
 * File:   song_info.h
 * Author: martin
 *
 * Created on 01. September 2013, 16:09
 */

#ifndef SONG_INFO_H
#define	SONG_INFO_H

#include "page_common.h"

class SongInfo : public Page, PageCommon
{    
    std::string value;
    std::string current_song;
    std::string current_modul;
    
public:
    SongInfo();
    
    std::string display();
    
};
        

#endif	/* SONG_INFO_H */

