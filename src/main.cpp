#include <chrono>
#include <json.hpp>
#include <print>
#include <string_view>
#include <vector>
using json = nlohmann::json;

#include <filesystem>
namespace fs = std::filesystem;
using iterateDir = std::filesystem::recursive_directory_iterator;

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
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

// dir.json Directory
fs::path dirDirectory() {
  return windowsRelativePath().parent_path().parent_path() / L"tests" /
         L"dir.json";
}

// Parse json
json parseJson() {
  std::ifstream f(dirDirectory());
  return json::parse(f);
}

bool doExists(std::string s, std::vector<std::string> v) {
  for (int i = 0; i < v.size(); i++) {
    if (s.contains(v[i])) {
      return true;
    }
  }
  return false;
}

int main(int argc, char *argv[]) {
  std::string userOptions = argv[1];

  std::string sourceDir{};
  std::string targetDir{};
  std::vector<std::string> exclusion{};

  std::string zxc{"badabings"};

  json jsonObject{parseJson()};
  for (auto &[key, val] : jsonObject.items()) {
    for (auto &[from, fromVal] : val.items()) {
      std::println("{} : ", from);

      // source
      if (from == "source" && !fromVal.empty()) {
        for (auto &[app, dir] : fromVal.items()) {

          if (dir.is_object()) {
            for (auto &[innerKey, innerVal] : dir.items()) {
              if (innerKey == "exc") {
                exclusion.push_back(innerVal);
              }

              if (innerKey == "src") {
                // sourceDir = innerVal;
                for (auto &srcDir : fs::directory_iterator(innerVal)) {
                  if (!doExists(srcDir.path().string(), exclusion)) {
                    std::cout << srcDir.path().string() << '\n';
                  }
                }
              }
            }
          }

          // if (!dir.is_object()) {
          //   for (auto &juxtapose : fs::directory_iterator(dir)) {
          //     std::cout << juxtapose << '\n';
          //   }
          // }

          // if (sourceDir.back() != '/') {
          //   sourceDir += '/';
          // }
        }
      }
    }
  }

  // std::cout << '\n';
  // std::cout << sourceDir << '\n';
  // std::cout << targetDir;

  for (auto &juxtapose : exclusion) {
    std::cout << juxtapose << '\n';
  }

  // Working ofstream
  // std::ofstream outf{sourceDir + "sample.txt"};
  // outf << "badabings";

  return 0;
}
