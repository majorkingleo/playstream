#include "pages.h"
#include "setup.h"
#include "page_default.h"
#include "play_music.h"
#include "set_volume.h"
#include "song_info.h"

Ref<Page> load_page()
{    
    if( SETUP.page == "PLAY_MUSIC" || SETUP.page.empty() )
	return new PlayMusic();       
    
    if( SETUP.page == "SET_VOLUME"  )
	return new SetVolume();       
    
    if( SETUP.page == "CURRENT_SONG"  )
	return new SongInfo();           
    
    VOUT(0)( "page not found: %s\n", SETUP.page ); 

    return new PageDefault();    
}
