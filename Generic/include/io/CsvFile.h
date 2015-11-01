/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pierrevr@mindgoo.be> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Pierre Vorhagen
 * ----------------------------------------------------------------------------
 */

#ifndef CSVFILE_HPP
#define CSVFILE_HPP

#include <list>
#include <vector>
#include <string>

namespace yiqiding{ namespace io{

class CsvFile {
public:
    bool load(const char * path);
    bool save(const char * path) const;

    bool add(const std::vector< std::string > & row);
    bool remove(unsigned int n);

    std::list< std::vector< std::string > > get() const;

private:
    std::list< std::vector< std::string > > _data;
};

}}

#endif
