#include "set_volume.h"
#include <cgicc/Cgicc.h>
#include "setup.h"

SetVolume::SetVolume()
: Page("set_volume"),
  PageCommon("set_volume"),
  set_command(),
  value()
{
    set_command = CONFIG.load_config_entry("mixer", "set_command" );        
        
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


std::string SetVolume::display()
{
    exec_command( set_command + " " + value );
    
    return "";    
}
