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

  fs::path currentPath{windowsRelativePath().parent_path().parent_path()};

  std::filesystem::create_directory("deploy");
  std::filesystem::create_directory("deploy/local");
  std::filesystem::create_directory("deploy/remote");

  std::filesystem::create_directory("deploy/local/localDir 1");
  std::filesystem::create_directory("deploy/local/localDir 2");
  std::filesystem::create_directory("deploy/local/localDir 3");

  std::filesystem::create_directory("deploy/remote/remoteDir 1");
  std::filesystem::create_directory("deploy/remote/remoteDir 2");
  std::filesystem::create_directory("deploy/remote/remoteDir 3");

  std::ofstream outf1{"deploy/local/localDir 1/sample1.txt"};
  std::ofstream outf2{"deploy/local/localDir 1/sample2.txt"};
  std::ofstream outf3{"deploy/local/localDir 1/sample3.txt"};
  std::ofstream outf1a{"deploy/local/localDir 1/sample4.txt"};
  std::ofstream outf2b{"deploy/local/localDir 1/sample5.txt"};
  std::ofstream outf3c{"deploy/local/localDir 1/sample6.txt"};
  std::ofstream outf3d{"deploy/local/localDir 1/sample7.txt"};
  std::ofstream outf1e{"deploy/local/localDir 1/sample8.txt"};
  std::ofstream outf2f{"deploy/local/localDir 1/sample9.txt"};
  std::ofstream outf3g{"deploy/local/localDir 1/sample10.txt"};

  std::ofstream outf4{"deploy/local/localDir 2/sample1.txt"};
  std::ofstream outf5{"deploy/local/localDir 2/sample2.txt"};
  std::ofstream outf6{"deploy/local/localDir 2/sample3.txt"};
  std::ofstream outf4a{"deploy/local/localDir 2/sample4.txt"};
  std::ofstream outf5b{"deploy/local/localDir 2/sample5.txt"};
  std::ofstream outf6c{"deploy/local/localDir 2/sample6.txt"};
  std::ofstream outf6d{"deploy/local/localDir 2/sample7.txt"};
  std::ofstream outf4e{"deploy/local/localDir 2/sample8.txt"};
  std::ofstream outf5f{"deploy/local/localDir 2/sample9.txt"};
  std::ofstream outf6g{"deploy/local/localDir 2/sample10.txt"};

  std::ofstream outf7{"deploy/local/localDir 3/sample1.txt"};
  std::ofstream outf8{"deploy/local/localDir 3/sample2.txt"};
  std::ofstream outf9{"deploy/local/localDir 3/sample3.txt"};
  std::ofstream outf7a{"deploy/local/localDir 3/sample4.txt"};
  std::ofstream outf8b{"deploy/local/localDir 3/sample5.txt"};
  std::ofstream outf9c{"deploy/local/localDir 3/sample6.txt"};
  std::ofstream outf9d{"deploy/local/localDir 3/sample7.txt"};
  std::ofstream outf7e{"deploy/local/localDir 3/sample8.txt"};
  std::ofstream outf8f{"deploy/local/localDir 3/sample9.txt"};
  std::ofstream outf9g{"deploy/local/localDir 3/sample10.txt"};
  // outf << "badabings";

  for (const auto &juxtapose : std::filesystem::directory_iterator(
           currentPath / "deploy/local/localDir 1")) {
    std::cout << juxtapose << '\n';
  }

  return 0;
}
