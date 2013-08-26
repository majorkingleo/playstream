#include "page_default.h"
#include "setup.h"
#include "format.h"
#include "string_utils.h"

PageDefault::PageDefault()
    : Page( "page_default" )     
{
    prefix = "page_suchen_";
    skin = "default.htm";
}

std::string PageDefault::display()
{
    std::string ret = header() + load_skin( skin );

    return ret;
}

