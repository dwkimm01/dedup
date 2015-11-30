#pragma once

//#if !defined(NDEBUG)
//#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
//#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
//#endif

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include "Utils.h"

using boost::multi_index_container;
using namespace boost::multi_index;

// Entry for a single file
struct FileRecord 
{
  int         id;
  std::string filePath;
  int         byteLength;

  FileRecord(int id_,std::string filePath_,int byteLength_):id(id_),filePath(filePath_),byteLength(byteLength_){}


  // TODO, have HEADER, MID, and TAIL md5 sums to make things faster?
  const std::string& getMD5Sum() const
  {
     // Lazy init md5sum
     if(md5sum.empty())
     {
        md5sum = Utils::calcFileMD5(filePath);
     }

     return md5sum;
  }

  bool isEqual(const FileRecord & rhs) const
  {
     if(byteLength != rhs.byteLength)
     {
        return false;
     }

     if(getMD5Sum() != rhs.getMD5Sum())
     {
        return false;
     }

     return true;
  }


  friend std::ostream& operator<<(std::ostream& os,const FileRecord& e)
  {
    os<<e.id<<" "<<e.filePath<<" "<<e.byteLength<<std::endl;
    return os;
  }

  bool shouldDelete() const { return m_shouldDelete; }
  void setShouldDelete(const bool val) const { m_shouldDelete = val; }

private:
  mutable std::string md5sum;
  mutable bool m_shouldDelete = false;
};

// tags for accessing the corresponding indices of file_set

struct id{};
struct filePath{};
struct byteLength{};

// See Compiler specifics: Use of member_offset for info on
//  BOOST_MULTI_INDEX_MEMBER
 
// Define a multi_index_container of FileRecords with following indices:
//   - a unique index sorted by FileRecord::int,
//   - a non-unique index sorted by FileRecord::name,
//   - a non-unique index sorted by FileRecord::byteLength.


typedef multi_index_container<
  FileRecord,
  indexed_by<
    ordered_unique<
      tag<id>,  BOOST_MULTI_INDEX_MEMBER(FileRecord,int,id)>,
    ordered_non_unique<
      tag<filePath>,BOOST_MULTI_INDEX_MEMBER(FileRecord,std::string,filePath)>,
    ordered_non_unique<
      tag<byteLength>, BOOST_MULTI_INDEX_MEMBER(FileRecord,int,byteLength)> >
> file_set;

template<typename Tag,typename MultiIndexContainer>
void print_out_by(const MultiIndexContainer& s)
{
  // obtain a reference to the index tagged by Tag 
  const typename boost::multi_index::index<MultiIndexContainer,Tag>::type& i=
    get<Tag>(s);

  typedef typename MultiIndexContainer::value_type value_type;

  // dump the elements of the index to cout 
  std::copy(i.begin(),i.end(),std::ostream_iterator<value_type>(std::cout));
}

