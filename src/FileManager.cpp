#include "FileManager.hpp"

void FileManager::clear() {
#ifdef NDEBUG
  std::cout << "Clearing the core path\n";
#endif
  corePath.clear();
}

void FileManager::clear(std::string newPath) {
#ifdef NDEBUG
  std::cout << "Clearing file manager with new path " + newPath + "\n";
#endif
  corePath = newPath;
}

void FileManager::info() {
  std::cout << "Current path is set to: " << corePath << "\n";
}

FileManager::file_info FileManager::make_file_info(std::string filename, std::string relative_filename, bool is_dir) {
  // f.name is the absolute name
  // f.rname is name of the folder/file
  // f.is_dir is set to true if it's a directory, else false
  file_info f;
  f.name = filename;
  f.rname = relative_filename;
  f.is_dir = is_dir;
  return f;
}

std::vector<FileManager::file_info> FileManager::list_files() {
  // This returns the list of files present in the folder: corePath
  // TODO: Add tests, check if corePath is not empty
  // Converting #ifdef NDEBUG and #endif to a macro
  
  std::vector<file_info> list_files;
  std::string base_name;

  if(*corePath.rbegin() != '/') base_name = corePath + "/";
  else base_name = corePath;
#ifdef NDEBUG
  std::cout << "Listing files and dirs in: " << corePath << "\n";
#endif
  DIR* dir;
  struct dirent *ent;
  bool is_dir;
  if((dir = opendir(base_name.c_str())) != NULL) {
    while((ent = readdir(dir)) != NULL) {
      std::string relative_filename = ent->d_name;
      if(relative_filename == "." || relative_filename == "..") {
        continue;
      }
      std::string filename = base_name + relative_filename;
      if(opendir(filename.c_str()) == NULL) {
#ifdef NDEBUG
        std::cout << filename << " is not a directory" << std::endl;
#endif
        is_dir = false;
      }
      else {
#ifdef NDEBUG
        std::cout << filename << " is a directory" << std::endl;
#endif
        is_dir = true;
      }
      list_files.push_back(make_file_info(filename, relative_filename, is_dir));
    }
    closedir(dir);
  } else {
    std::cout << "Could not open directory " << base_name.c_str() << std::endl;
  }
  return list_files;
}

std::string spaces(int s) {
  std::string out;
  for (size_t i = 0; i < s; i++) {
    out += "    ";
  }
  return out;
}

void FileManager::writeToFileIterated(FileManager f, std::ofstream& file, int depth, std::vector<std::string> ignore_dirs) {
  std::vector<file_info> out_dir = f.list_files();
  ++depth;
  for (auto const& iterating_entry: out_dir) {
    if (!iterating_entry.is_dir) {
      // it's a file
      file << spaces(depth) + "|-- " + iterating_entry.rname + "\n";
    } else {
      // It's a directory
      file << spaces(depth) + "|-- " + iterating_entry.rname + "\n";
      if (dirTobeIgnored(iterating_entry.rname, ignore_dirs)) {
#ifdef NDEBUG
        std::cout << "Ignoring dir: " << iterating_entry.name << "\n";
#endif
        continue;
      }
      f.clear(iterating_entry.name);
      f.writeToFileIterated(f, file, depth, ignore_dirs);
    }
  }   
}

bool FileManager::dirTobeIgnored(std::string dir_name, std::vector<std::string> dirs_list) {
  if (dirs_list.size() == 0) {
#ifdef NDEBUG
    std::cout << "Nothing passed in ignore_dirs, returning false by default then." << std::endl;
#endif
    return false;
  }

  for(auto const& dir: dirs_list) {
    if (dir == dir_name) {
      return true;
    }
  }
  return false;
}

void FileManager::writeToFile(FileManager f, std::vector<std::string> ignore_dirs = {}) { 
  std::vector<file_info> out = f.list_files();
  if (out.size() == 0) {
#ifdef NDEBUG
    std::cout << "We got no files in the folder, enable NDEBUG flag to see what happened.\n";
#endif
    return;
  }
  std::ofstream file;
  file.open("sample_out.txt");
  if (!file.is_open()) {
    std::cout << "Unable to open the file, please check\n";
    return;
  }
  int depth = 0;
  for (auto const& entry: out) {
    if (!entry.is_dir) {
      // entry is a file
      file << "|-- " + entry.rname + "\n";
    } else {
      // entry is a directory
      file << "|-- " + entry.rname + "\n";
#ifdef NDEBUG
      std::cout << "Checking " << entry.rname << " against build" << std::endl; 
#endif
      if (dirTobeIgnored(entry.rname, ignore_dirs)) {
#ifdef NDEBUG
        std::cout << "Ignoring dir: " << entry.name << "\n";
#endif
        continue;
      } else {
        f.clear(entry.name);
        f.writeToFileIterated(f, file, depth, ignore_dirs);
      }
    }
  }
#ifdef NDEBUG
  std::cout << "Done!\n";
#endif
  file.close();
}