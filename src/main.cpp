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

struct ManagedObject {
  static inline std::vector<std::string> jsonNeedleIncl{};
  static inline std::vector<std::string> jsonNeedleExcl{};

  static inline std::vector<std::string> localSourceDir{};
  static inline std::vector<std::string> remoteSourceDir{};

  static inline bool hasIncl{true};
};

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
         L"dirTests.json";
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

// Source - https://stackoverflow.com/a/20303915
// Posted by masoud, modified by community. See post 'Timeline' for change
// history Retrieved 2026-05-03, License - CC BY-SA 4.0
// <
#include <algorithm>
#include <vector>

bool in_array(const std::string &value,
              const std::vector<std::string> &needle) {
  return std::any_of(needle.begin(), needle.end(), [&](const std::string &n) {
    return value.find(n) != std::string::npos;
  });
}
// >

void jsonObjectManager(ManagedObject &data) {
  json jsonObject{parseJson()};
  // dir pass
  for (auto &[baseKey, baseVal] : jsonObject.items()) {
    for (auto &[sourceKey, sourceVal] : baseVal.items()) {
      std::cout << sourceKey << '\n';
      // local
      if (sourceKey == "local") {
        for (auto &[appKey, appVal] : sourceVal.items()) {
          std::cout << appKey << '\n';
          data.jsonNeedleIncl.clear();
          data.jsonNeedleExcl.clear();

          if (appVal.is_object()) {
            // filter pass
            for (auto &[innerKey, innerVal] : appVal.items()) {
              if (innerKey == "incl") {
                for (auto inclusionElement : innerVal) {
                  data.jsonNeedleIncl.push_back(inclusionElement);
                  data.hasIncl = true;
                }
              }
              if (innerKey == "exc") {
                for (auto exclusionElement : innerVal) {
                  data.jsonNeedleExcl.push_back(exclusionElement);
                  data.hasIncl = false;
                }
              }
            }

            // directory pass
            for (auto &[innerKey, innerVal] : appVal.items()) {
              if (innerKey == "src") {
                for (auto &srcDir : fs::directory_iterator(innerVal)) {
                  if (in_array(srcDir.path().string(), data.jsonNeedleIncl) &&
                      data.hasIncl) {
                    std::cout << srcDir << '\n';
                    data.localSourceDir.push_back(srcDir.path().string());
                  }

                  if (!in_array(srcDir.path().string(), data.jsonNeedleExcl) &&
                      !data.hasIncl) {
                    std::cout << srcDir << '\n';
                    data.localSourceDir.push_back(srcDir.path().string());
                  }
                }
              }
            }
          }

          // pure dir
          if (!appVal.is_object()) {
            for (auto &srcDir : fs::directory_iterator(appVal)) {
              std::cout << srcDir << '\n';
              data.localSourceDir.push_back(srcDir.path().string());
            }
          }
        }
      }

      // remote
      if (sourceKey == "remote") {
        for (auto &[appKey, appVal] : sourceVal.items()) {
          std::cout << appKey << '\n';
          std::cout << appVal << '\n';
          data.remoteSourceDir.push_back(appVal);
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  std::string userOptions = argv[1];

  // std::string input{};
  // std::cin >> input;

  // ManagedObject data{};
  // jsonObjectManager(data);

  std::vector<std::vector<int>> matrix = {
      {1, 2},
      {4, 5},
      {7, 8},
  };

  std::cout << matrix[1][1];
  matrix.push_back({});
  matrix[3][1] = 13;
  std::cout << matrix[3][1];

  // const auto copyOptions =
  //     fs::copy_options::overwrite_existing | fs::copy_options::recursive;
  //
  // if (input == "retrieve") {
  //   for (auto &remoteElement : data.remoteSourceDir) {
  //     for (auto &localElement : data.localSourceDir) {
  //       std::println("{} \t-> \t{}", localElement, remoteElement);
  //       fs::copy(localElement, remoteElement, copyOptions);
  //     }
  //   }
  // }

  return 0;
}
