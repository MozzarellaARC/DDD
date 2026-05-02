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

// Check if a substring exists in a vector<string>
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

  std::vector<std::string> localDirExc{};
  std::vector<std::string> localDirIncl{};
  std::vector<std::string> remoteDir{};
  bool hasInclusion{false};
  std::vector<std::string> exclusion{};
  std::vector<std::string> inclusion{};

  json jsonObject{parseJson()};
  for (auto &[key, val] : jsonObject.items()) {
    for (auto &[from, fromVal] : val.items()) {
      std::println("{} : ", from);

      // local
      if (from == "local" && !fromVal.empty()) {
        for (auto &[app, dir] : fromVal.items()) {
          exclusion.clear();
          inclusion.clear();
          hasInclusion = false;

          if (dir.is_object()) {

            // Pass 1: collect filters
            for (auto &[innerKey, innerVal] : dir.items()) {
              if (innerKey == "exc" && !doExists(innerVal, exclusion))
                exclusion.push_back(innerVal);
              if (innerKey == "incl") {
                hasInclusion = true;
                if (!doExists(innerVal, inclusion))
                  inclusion.push_back(innerVal);
              }
            }

            // Pass 2: scan source directories
            for (auto &[innerKey, innerVal] : dir.items()) {
              if (innerKey == "src") {
                // localDir = innerVal;
                if (!fs::is_empty(innerVal)) {
                  std::cout << app << '\n';
                }
                for (auto &srcDir : fs::directory_iterator(innerVal)) {
                  bool check_inclusion =
                      doExists(srcDir.path().string(), inclusion);

                  bool check_exclusion =
                      doExists(srcDir.path().string(), exclusion);

                  if (hasInclusion) {
                    // inclusion mode: only print files in inclusion list
                    if (check_inclusion) {
                      std::cout << srcDir << '\n';
                      localDirIncl.push_back(srcDir.path().string());
                    }
                  } else {
                    // exclusion mode: print everything except excluded files
                    if (!check_exclusion) {
                      std::cout << srcDir << '\n';
                      localDirExc.push_back(srcDir.path().string());
                    }
                  }
                }
              }
            }
          }
        }
      }

      // remote
      if (from == "remote" && !fromVal.empty()) {
        for (auto &[app, dir] : fromVal.items()) {
          if (!fs::is_empty(dir)) {
            std::cout << app << '\n';
          }
          for (auto &[innerKey, innerVal] : dir.items()) {
            for (auto &srcDir : fs::directory_iterator(innerVal)) {
              std::cout << srcDir << '\n';
              remoteDir.push_back(srcDir.path().string());
            }
          }
        }
      }
    }
  }

  // std::cout << '\n';
  // std::cout << localDir << '\n';
  // std::cout << remoteDir;

  // for (auto &juxtapose : localDirExc) {
  //   std::cout << juxtapose << '\n';
  // }
  //
  // for (auto &juxtapose : localDirIncl) {
  //   std::cout << juxtapose << '\n';
  // }

  std::cout << '\n';

  // for (auto &juxtapose : remoteDir) {
  //   std::cout << juxtapose << '\n';
  // }
  //
  // std::cout << '\n';

  // for (auto &juxtapose : exclusion) {
  //   std::cout << juxtapose << '\n';
  // }
  //
  // std::cout << '\n';
  //
  // for (auto &juxtapose : inclusion) {
  //   std::cout << juxtapose << '\n';
  // }

  // Working ofstream
  // std::ofstream outf{localDir + "sample.txt"};
  // outf << "badabings";

  return 0;
}
