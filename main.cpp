#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <regex>
#include "Config.h"
#include "Utils.h"
#include "FileTable.h"

using namespace std;
//using namespace boost;
//using namespace boost::filesystem;


// --------------------------------------------------------
// Main
int main(int argc, char* argv[])
{
   // Config parsing
   const DupesConfig config(argc, argv);
   if(config.exitEarly())
   {
      std::cout << "Exiting early" << std::endl;
      return 0;
   }

   const std::regex rmMatchRegex(config.rmMatch());

   // Accumulator of file paths, file sizes, and md5sums
   file_set fs;
   int id = 0;

   // Search through directories
   for(const auto & path : config.pathsVec())
   {
      Utils::findFiles(path, [&](const boost::filesystem::path & path)
         {
            fs.insert(FileRecord(id++, path.string(), Utils::fileSize(path)));
            
            const auto mod = id % 1000;
            if(0 == mod)
            {
               std::cout << " ... found " << id << " files" << std::endl;
            }

            return true; 
         } );
   }

   std::cout << "Found " << id << " files" << std::endl;


   //std::cout<<"by filePath"<<std::endl;
   //print_out_by<filePath>(fs);
   //std::cout<<std::endl;
  
   //std::cout<<"by byteLength"<<std::endl;
   //print_out_by<byteLength>(fs);
   //std::cout<<std::endl;

   // Output script to delete files
   std::ofstream ofs("out.txt", std::ofstream::out);

   // Find files of the same length to calc md5sum on
   std::cout << "Looking for duplicate sizes" << std::endl;
   size_t matchIndexCount = 0;
   size_t matchingFiles = 0;
   auto & lengthIndex = fs.get<byteLength>();
   for(auto lengthItr = lengthIndex.begin(); lengthItr != lengthIndex.end(); ++lengthItr) 
   {
      ++matchIndexCount;
      if(matchIndexCount % 1000 == 0)
      {
         std::cout << " ... checked " << matchIndexCount << " files" << std::endl;
      }
      // std::cout << "L> " << lengthItr->filePath << std::endl;

      // Get next iteration in list if it's not the end
      auto lengthItrPP = lengthItr;
      ++lengthItrPP;
      // if(lengthItrPP != lengthIndex.end())
      for(;lengthItrPP != lengthIndex.end(); ++lengthItrPP)
      {
         // std::cout << "   next: " << lengthItrPP->filePath << std::endl;
         //if(lengthItr->byteLength == lengthItrPP->byteLength)
         //{
         //   std::cout << "  Same size: " << lengthItr->filePath << ", " <<  lengthItrPP->filePath << std::endl;
         //}
         
         if(lengthItr->isEqual(*lengthItrPP))
         {
            //std::cout << "File match " << lengthItr->filePath << " and " << lengthItrPP->filePath << "\n"; // std::endl;

            ++matchingFiles;

            // Determine which file to delete
            std::smatch m;
            if(std::regex_search (lengthItr->filePath, m, rmMatchRegex))
            {
               lengthItr->setShouldDelete(true);
            }
            else if(std::regex_search (lengthItrPP->filePath, m, rmMatchRegex))
            {
               lengthItrPP->setShouldDelete(true);
            }
            else
            {
               ofs << "del failed, could not match either" << "\n"
                   << "  " << lengthItr->filePath << " md5 " << lengthItr->getMD5Sum() << "\n"
                   << "  " << lengthItrPP->filePath << " md5 " << lengthItrPP->getMD5Sum() << "\n";
            }
         }
      }
   }

   // Write out all deleted files
   auto & idIndex = fs.get<::id>();
   for(auto itr = idIndex.begin(); itr != idIndex.end(); ++itr)
   {
      if(itr->shouldDelete())
      {
         ofs << "DEL " << itr->filePath << "\n";
      }
   }


   std::cout << "Found " << matchingFiles << " matching Files" << std::endl;


   return 0;
}
