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
  std::vector<std::string> localDirExc{};
  std::vector<std::string> localDirIncl{};
  std::vector<std::string> remoteDir{};

  bool hasInclusion{false};
  std::vector<std::string> exclusion{};
  std::vector<std::string> inclusion{};
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

void jsonObjectManager(ManagedObject &data) {
  json jsonObject{parseJson()};
  for (auto &[key, val] : jsonObject.items()) {
    for (auto &[from, fromVal] : val.items()) {
      std::println("{} : ", from);

      // local
      if (from == "local" && !fromVal.empty()) {
        for (auto &[app, dir] : fromVal.items()) {
          data.exclusion.clear();
          data.inclusion.clear();
          data.hasInclusion = false;

          if (dir.is_object()) {

            // Pass 1: collect filters
            for (auto &[innerKey, innerVal] : dir.items()) {
              if (innerKey == "exc" && !doExists(innerVal, data.exclusion))
                data.exclusion.push_back(innerVal);
              if (innerKey == "incl") {
                data.hasInclusion = true;
                if (!doExists(innerVal, data.inclusion))
                  data.inclusion.push_back(innerVal);
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
                      doExists(srcDir.path().string(), data.inclusion);

                  bool check_exclusion =
                      doExists(srcDir.path().string(), data.exclusion);

                  if (data.hasInclusion) {
                    // data.inclusion mode: only print files in inclusion list
                    if (check_inclusion) {
                      std::cout << srcDir << '\n';
                      data.localDirIncl.push_back(srcDir.path().string());
                    }
                  } else {
                    // data.exclusion mode: print everything except excluded
                    // files
                    if (!check_exclusion) {
                      std::cout << srcDir << '\n';
                      data.localDirExc.push_back(srcDir.path().string());
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
              data.remoteDir.push_back(srcDir.path().string());
            }
          }
        }
      }
    }
  }
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

int main(int argc, char *argv[]) {
  std::string userOptions = argv[1];

  std::vector<std::string> needle{"sample1", "sample2"};
  std::vector<std::string> jsonNeedleIncl{};
  std::vector<std::string> jsonNeedleExcl{};

  std::vector<std::string> localSourceDir{};
  std::vector<std::string> remoteSourceDir{};

  // ManagedObject data{};
  // jsonObjectManager(data);

  json jsonObject{parseJson()};

  // needle pass
  for (auto &[baseKey, baseVal] : jsonObject.items()) {
    for (auto &[sourceKey, sourceVal] : baseVal.items()) {
      for (auto &[appKey, appVal] : sourceVal.items()) {
        if (appVal.is_object()) {
          for (auto &[innerKey, innerVal] : appVal.items()) {
            if (innerKey == "incl") {
              for (auto inclusionElement : innerVal) {
                jsonNeedleIncl.push_back(inclusionElement);
              }
            }
            if (innerKey == "exc") {
              for (auto exclusionElement : innerVal) {
                jsonNeedleExcl.push_back(exclusionElement);
              }
            }
          }
        }
      }
    }
  }

  // badabings
  for (auto &[baseKey, baseVal] : jsonObject.items()) {
    for (auto &[sourceKey, sourceVal] : baseVal.items()) {
      if (sourceKey == "local") {
        for (auto &[appKey, appVal] : sourceVal.items()) {
          for (auto &[innerKey, innerVal] : appVal.items()) {
            if (innerKey == "src") {
              for (auto &srcDir : fs::directory_iterator(innerVal)) {
                if (in_array(srcDir.path().string(), needle)) {
                  localSourceDir.push_back(srcDir.path().string());
                }
              }
            }
          }
        }
      }
    }
  }

  // movement from remote doesn't need exclusive check
  for (auto &[baseKey, baseVal] : jsonObject.items()) {
    for (auto &[sourceKey, sourceVal] : baseVal.items()) {
      if (sourceKey == "remote") {
        for (auto &[appKey, appVal] : sourceVal.items()) {
          for (auto &[innerKey, innerVal] : appVal.items()) {
            for (auto &srcDir : fs::directory_iterator(innerVal)) {
              if (in_array(srcDir.path().string(), needle)) {
                std::cout << srcDir << '\n';
              }
            }
          }
        }
      }
    }
  }

  //
  // for (auto &juxtapose : localSourceDir) {
  //   std::cout << juxtapose << '\n';
  // }

  std::cout << "needle inlcude\n";
  for (auto &juxtapose : jsonNeedleIncl) {
    std::cout << juxtapose << '\n';
  }

  std::cout << "needle exclude\n";
  for (auto &juxtapose : jsonNeedleExcl) {
    std::cout << juxtapose << '\n';
  }

  return 0;
}
