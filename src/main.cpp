#include <json.hpp>
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
json parseJson(std::string x) {
  std::ifstream f(windowsRelativePath().parent_path() / "dir.json");
  return json::parse(f)[x];
}

// Exclude json
json excludeJson(std::string x) {
  std::ifstream f(windowsRelativePath().parent_path() / "exclude.json");
  return json::parse(f)[x];
}

// Mumbo jumbo
std::string matchExclude() {
  const char *a{"test"};
  std::string s = a;
  return static_cast<std::string_view>(a);
}

// Json iterator
void iterateJson(bool a) {
  json fromJson = a ? parseJson("source") : parseJson("target");
  json toJson = a ? parseJson("target") : parseJson("source");

  for (auto &[key, fromPath] : fromJson.items()) {
    auto &toPath = toJson[key];

    fs::path from = fromPath.get<std::string>();
    fs::path to = toPath.get<std::string>();

    if (!fs::exists(from)) {
      std::cout << from << " -> " << "source directory doesn't exists\n";
    } else if (fs::is_empty(from)) {
      std::cout << from << " -> " << "source directory is empty\n";
    } else if (!fs::exists(to)) {
      std::cout << to << " -> " << "target directory not exists\n";
    } else {
      for (auto &entry : fs::directory_iterator(from)) {
        // std::cout << fs::path(entry).generic_string() << '\n';
        std::string genericPath = from.generic_string();
        // auto exclusion[];

        // if (fs::path(entry.path()) == "abc2") {
        //   std::cout << "badabings";
        // }
      }

      // std::cout << from << " -> " << to << '\n';
    }
  }
}

// Let's go (Interface)
int main() {
  std::cout << "Would you like to Retrieve or Deploy dotfiles \n"
            << "0: Retrieve - 1: Deploy\n";

  int answer;
  do {
    std::cin >> answer;
    if (answer == 0) {
      std::cout << "Retrieving user data\n";
      iterateJson(true);
    } else if (answer == 1) {
      std::cout << "Deploying user data\n";
      iterateJson(false);
    } else {
      std::cout << "Please enter 0 or 1\n";
    }
  } while (answer != 0 && answer != 1);

  return 0;
}
