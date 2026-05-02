#include <filesystem>

#include <filesystem>
namespace fs = std::filesystem;
using iterateDir = std::filesystem::recursive_directory_iterator;

// Windows specific api
#include <Windows.h>

#include <filesystem>
namespace fs = std::filesystem;
using iterateDir = std::filesystem::recursive_directory_iterator;

#include <fstream>
#include <iostream>
#include <string>

// Windows relative path
fs::path windowsRelativePath() {
  wchar_t buffer[MAX_PATH];
  DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);

  if (length == 0) {
    std::cerr << "Failed to get executable path\n";
    return {};
  }
  return fs::path(buffer);
}

int main() {
  std::filesystem::current_path(
      windowsRelativePath().parent_path().parent_path());

  std::filesystem::create_directory("deploy");
  std::filesystem::create_directory("deploy/source");
  std::filesystem::create_directory("deploy/target");

  std::filesystem::create_directory("deploy/source/srcDir 1");
  std::filesystem::create_directory("deploy/source/srcDir 2");
  std::filesystem::create_directory("deploy/source/srcDir 3");

  std::filesystem::create_directory("deploy/target/tgtDir 1");
  std::filesystem::create_directory("deploy/target/tgtDir 2");
  std::filesystem::create_directory("deploy/target/tgtDir 3");

  std::ofstream outf1{"deploy/source/srcDir 1/sample1.txt"};
  std::ofstream outf2{"deploy/source/srcDir 1/sample2.txt"};
  std::ofstream outf3{"deploy/source/srcDir 1/sample3.txt"};

  std::ofstream outf4{"deploy/source/srcDir 2/sample1.txt"};
  std::ofstream outf5{"deploy/source/srcDir 2/sample2.txt"};
  std::ofstream outf6{"deploy/source/srcDir 2/sample3.txt"};

  std::ofstream outf7{"deploy/source/srcDir 3/sample1.txt"};
  std::ofstream outf8{"deploy/source/srcDir 3/sample2.txt"};
  std::ofstream outf9{"deploy/source/srcDir 3/sample3.txt"};
  // outf << "badabings";

  return 0;
}
