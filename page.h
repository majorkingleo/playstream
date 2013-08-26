#ifndef PAGE_H
#define PAGE_H

#include <string>
#include "local_debug.h"

class Page
{
 protected:
    std::string name;
    std::string skin_dir;
    std::string skin;
    std::string skin_index;
    std::string host_url;
    std::string host_dir;
    std::string sendmail;
    std::string convert;
    std::string image_dir;
    std::string image_url;
    std::string prefix;

 public:
    Page( const std::string &name );
    virtual ~Page();

    virtual std::string display() = 0;

    std::string header();

 protected:
 
    std::string load_simple_skin( const std::string &skin_name );
    std::string load_skin( const std::string &skin_name );
};

#endif
