#pragma once

#include <fstream>
#include <boost/filesystem.hpp>
#include "MD5Sum.h"

class Utils
{
public:

   // Given a file path return the md5 checksum
   static std::string calcFileMD5(const std::string & fileName)
   {
      common::MD5Sum md5Sum;
      const size_t buffLength = 2048;
      char buff[buffLength];
      // TODO, clean this up
      std::ifstream ifs;
      ifs.open(fileName.c_str(), std::ios::in | std::ios::binary);
      if(ifs.is_open() && ifs.good())
      {
         while(!ifs.eof())
         {
            const size_t bytesRead = ifs.read(buff, buffLength).gcount();
            md5Sum.update(buff, bytesRead);
         }
      }

      const auto digest = md5Sum.getHexStringDigest();
      return digest;
   }

   
   // Given a directory, recursively find files
   static void findFiles(const boost::filesystem::path & root, const std::function<bool(const boost::filesystem::path & path)> & callback)
   {
      using namespace boost::filesystem;
      if(!exists(root) || !is_directory(root)) 
      {
         return;
      }

      recursive_directory_iterator it(root);
      recursive_directory_iterator endit;

      while(it != endit)
      {
         if(is_regular_file(*it)) // TODO, allow filtering by extension using regex: && it->path().extension() == ext)
         {
            callback(it->path());
         }
         ++it;
      }
   }


   static uintmax_t fileSize(const boost::filesystem::path & path)
   {
      return boost::filesystem::file_size(path);
   }

};

