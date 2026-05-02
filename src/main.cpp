#include <chrono>
#include <json.hpp>
#include <print>
#include <string_view>
using json = nlohmann::json;

#include <filesystem>
namespace fs = std::filesystem;
using iterateDir = std::filesystem::recursive_directory_iterator;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

// Windows specific api
#include <Windows.h>

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

// Parse json
json parseJson() {
  std::ifstream f(windowsRelativePath().parent_path().parent_path() /
                  "dir.json");
  return json::parse(f);
}

// Json iterator
void iterateJson(bool a) {}

int main(int argc, char *argv[]) {
  std::string userOptions = argv[1];

  std::string sourceDir{};
  std::string targetDir{};

  json jsonObject{parseJson()};
  for (auto &[key, val] : jsonObject.items()) {
    for (auto &[source, sourceVal] : val.items()) {
      // source/target level
      std::println("{} : ", source);
      // app/dir level
      if (source == "source") {
        for (auto &[app, dir] : sourceVal.items()) {
          std::cout << dir << '\n';
          sourceDir = dir;
        }
      }
    }
  }

  std::cout << '\n';
  std::cout << sourceDir;
  std::cout << targetDir;

  return 0;
}
