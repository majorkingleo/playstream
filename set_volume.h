/* 
 * File:   set_volume.h
 * Author: martin
 *
 * Created on 01. September 2013, 11:57
 */

#ifndef SET_VOLUME_H
#define	SET_VOLUME_H

#include "page_common.h"

class SetVolume : public Page, PageCommon
{
    std::string set_command;
    std::string value;
    
public:
    SetVolume();
    
    std::string display();
    
};
        


#endif	/* SET_VOLUME_H */

