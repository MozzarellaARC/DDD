#include <chrono>
#include <json.hpp>
#include <ostream>
#include <print>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <filesystem>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>

// Windows specific api
#include <Windows.h>

using json       = nlohmann::json;

namespace fs     = std::filesystem;
using iterateDir = std::filesystem::recursive_directory_iterator;

struct Dir {
  std::vector<fs::path> dirVal{};
};

struct ManagedObject {
  static inline std::vector<std::string> jsonNeedleIncl{};
  static inline std::vector<std::string> jsonNeedleExcl{};

  static inline std::unordered_map<std::string, Dir> localDirMap{};
  static inline std::unordered_map<std::string, fs::path> flattenedLocalDirMap{};
  static inline std::unordered_map<std::string, Dir> remoteDirMap{};

  static inline bool hasIncl{true};
};

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
  return windowsRelativePath().parent_path().parent_path() / L"tests" / L"dirTests.json";
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

bool in_array(const std::string& value, const std::vector<std::string>& needle) {
  return std::any_of(needle.begin(), needle.end(),
                     [&](const std::string& n) { return value.find(n) != std::string::npos; });
}
// >

void jsonObjectManager(ManagedObject& data, Dir& dir) {
  json jsonObject{parseJson()};
  // dir pass
  for (auto& [baseKey, baseVal] : jsonObject.items()) {
    for (auto& [sourceKey, sourceVal] : baseVal.items()) {
      // std::cout << sourceKey << '\n';
      // local
      if (sourceKey == "local") {
        for (auto& [appKey, appVal] : sourceVal.items()) {
          // std::cout << appKey << '\n';
          data.localDirMap.emplace(appKey, dir);
          data.jsonNeedleIncl.clear();
          data.jsonNeedleExcl.clear();

          if (appVal.is_object()) {
            // filter pass
            for (auto& [innerKey, innerVal] : appVal.items()) {
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
            for (auto& [innerKey, innerVal] : appVal.items()) {
              if (innerKey == "src") {

                data.flattenedLocalDirMap[appKey] = innerVal.get<fs::path>();

                // dir content
                for (auto& srcDir : fs::directory_iterator(innerVal)) {
                  if (in_array(srcDir.path().string(), data.jsonNeedleIncl) && data.hasIncl) {
                    // std::cout << srcDir << '\n';
                    data.localDirMap[appKey].dirVal.push_back(srcDir.path());
                  }

                  if (!in_array(srcDir.path().string(), data.jsonNeedleExcl) && !data.hasIncl) {
                    // std::cout << srcDir << '\n';
                    data.localDirMap[appKey].dirVal.push_back(srcDir.path());
                  }
                }
              }
            }
          }

          // pure dir
          if (!appVal.is_object()) {
            data.flattenedLocalDirMap[appKey] = appVal.get<fs::path>();
            for (auto& srcDir : fs::directory_iterator(appVal)) {
              // std::cout << srcDir << '\n';
              data.localDirMap[appKey].dirVal.push_back(srcDir.path());
            }
          }
        }
      }

      // remote
      if (sourceKey == "remote") {
        for (auto& [appKey, appVal] : sourceVal.items()) {
          data.remoteDirMap.emplace(appKey, dir);
          // std::cout << appKey << '\n';
          data.remoteDirMap.emplace(appKey, dir);
          // std::cout << appVal << '\n';
          data.remoteDirMap[appKey].dirVal.push_back(appVal);
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Add retrieve or deploy" << std::endl;
    return 0;
  }

  if (argc < 2) {
    std::cout << "Too many arguments" << std::endl;
  }

  std::string userOptions = argv[1];

  ManagedObject data{};
  Dir dir{};
  jsonObjectManager(data, dir);

  for (const auto& [key, value] : data.localDirMap) {
    std::cout << '\n' << "local " << key << " : " << '\n';
    for (auto& v : value.dirVal) {
      std::cout << v.string() << '\n';
    }
  }

  for (const auto& [key, value] : data.flattenedLocalDirMap) {
    std::cout << '\n' << "flattened local " << key << " : \n" << value.string() << '\n';
  }

  for (const auto& [key, value] : data.remoteDirMap) {
    std::cout << '\n' << "remote " << key << " : " << '\n';
    for (auto& v : value.dirVal) {
      std::cout << v.string() << '\n';
    }
  }

  std::cout << "Here mark the end of the debugging messeages";

  const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;

  if (userOptions == "retrieve" || userOptions == "get") {
    for (const auto& [localKey, localVal] : data.localDirMap) {
      std::cout << localKey << ": " << '\n';
      auto remoteIt         = data.remoteDirMap.find(localKey);
      const auto& remoteVal = remoteIt->second;

      for (const auto& remote : remoteVal.dirVal) {
        if (!fs::exists(remote)) {
          fs::create_directories(remote);
        }

        if (fs::exists(remote)) {
          fs::remove_all(remote);
          fs::create_directories(remote);
        }

        for (const auto& local : localVal.dirVal) {
          fs::copy(local, remote / local.filename(), copyOptions);
          std::cout << local << "\t -> \t" << remote << '\n';
        }
      }
    }

    return 0;
  }

  if (userOptions == "deploy" || userOptions == "set") {
    for (auto& [remoteKey, remoteVal] : data.remoteDirMap) {
      std::cout << remoteKey << ": " << '\n';
      auto localIt = data.flattenedLocalDirMap.at(remoteKey);

      for (auto& remote : remoteVal.dirVal) {
        fs::copy(remote, localIt, copyOptions);

        std::cout << remote << " -> " << localIt << '\n';
      }
    }
    return 0;
  }

  if (userOptions != "retrieve" || userOptions != "deploy") {
    std::cout << "There is only 2 options retrieve or deploy" << std::endl;
    return 0;
  }

  return 0;
}
