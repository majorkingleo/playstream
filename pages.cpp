#include "pages.h"
#include "setup.h"
#include "page_default.h"
#include "play_music.h"

Ref<Page> load_page()
{    
    if( SETUP.page == "PLAY_MUSIC" || SETUP.page.empty() )
	return new PlayMusic();       
    
    VOUT(0)( "page not found: %s\n", SETUP.page ); 

    return new PageDefault();    
}
