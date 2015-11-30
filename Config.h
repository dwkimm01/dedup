#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

// Duplicate file finding configurtion
class DupesConfig
{
   bool m_exitEarly = false;
   std::string m_rmMatch;
   std::vector<std::string> m_pathsVec;
   int m_threadCount = 1;

public:
   // CTOR
   DupesConfig(int argc, char* argv[])
   {
      namespace po = boost::program_options;

      // Declare the supported options.
      po::options_description desc("Configuration settings");
      desc.add_options()
         ("help,h", "Produce help message and exit")
         ("rm-match,rmm", po::value<std::string>(), "String to match which duplicate to delete")
         ("path,p", po::value<std::vector<std::string>>(), "Path to add to list to search for dupes")
         ("threadCount,j", po::value<int>(), "Number of threads to use")
         ;

      // Parse into variables map
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);    

      // Process variables map into member variables
      if (vm.count("help")) 
      {
         std::cout << desc << "\n";
         m_exitEarly = true;
         return;
      }

      if (vm.count("rm-match")) 
      {
         m_rmMatch = vm["rm-match"].as<std::string>();
         std::cout << "Set rm-match to \'" << m_rmMatch << "\'\n";
      } 
      else 
      {
         std::cout << "Error, must set rm-match to something \n";
         m_exitEarly = true;
      }

      if (vm.count("path"))
      {
         m_pathsVec = vm["path"].as<std::vector<std::string>>();
         std::cout << "Set paths to, " << m_pathsVec.size() << "\n";
         for(const auto & p : m_pathsVec)
         {
            std::cout << "   \"" << p << "\"\n";
         }
      }
      else
      {
         std::cout << "No paths sepcified for processing\n";
         m_exitEarly = true;
      }

      if(vm.count("threadCount"))
      {
         m_threadCount = vm["threadCount"].as<int>();
         std::cout << "Set thread count to " << m_threadCount << "\n";
      }
      else
      {
         std::cout << "Thread count default, " << m_threadCount << "\n";
      }

   }


   bool exitEarly() const { return m_exitEarly; }
   const std::string& rmMatch() const { return m_rmMatch; }
   const std::vector<std::string>& pathsVec() const { return m_pathsVec; }

};
