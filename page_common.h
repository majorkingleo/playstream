/* 
 * File:   page_common.h
 * Author: martin
 *
 * Created on 01. September 2013, 11:58
 */

#ifndef PAGE_COMMON_H
#define	PAGE_COMMON_H

#include "page.h"

class PageCommon
{
protected:
    std::string outfile;	
    
public:
    PageCommon( const std::string & name );
    
    std::string exec_command( const std::string & command, bool silent = false );
    void fork_and_exec( const std::string & command );
    void fork_and_exec( const std::string & command, const std::string & name );
};

#endif	/* PAGE_COMMON_H */

