#include <cstdlib>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <regex>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace Json;

// Function to check if a package is installed using yay
bool isPackageInstalled(const std::string &packageName) {
  std::string command = "yay -Qs " + packageName + " > /dev/null 2>&1";
  int result = std::system(command.c_str());
  return result == 0;
}

// Function to install a package using yay
void installPackage(const std::string &packageName) {
  std::string command = "yay -S --noconfirm " + packageName;
  int result = std::system(command.c_str());
  if (result != 0) {
    throw std::runtime_error("Failed to install package: " + packageName);
  }
}

// Function to remove comments from JSONC content
std::string removeComments(const std::string &jsoncContent) {
  std::string result = jsoncContent;

  // Remove single-line comments
  std::regex singleLineCommentPattern("//.*");
  result = std::regex_replace(result, singleLineCommentPattern, "");

  // Remove multi-line comments
  std::regex multiLineCommentPattern("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/");
  result = std::regex_replace(result, multiLineCommentPattern, "");

  return result;
}

// Function to get package file path
std::string getPackageFilePath() {
  const char *homeDir = getenv("HOME");
  if (!homeDir) {
    throw std::runtime_error("Failed to get home directory");
  }

  // Default paths
  std::string jsonPath = std::string(homeDir) + "/.config/confix/packages.json";
  std::string jsoncPath =
      std::string(homeDir) + "/.config/confix/packages.jsonc";

  // Check if the .json file exists
  struct stat buffer;
  if (stat(jsonPath.c_str(), &buffer) == 0) {
    return jsonPath;
  }

  // Check if the .jsonc file exists
  if (stat(jsoncPath.c_str(), &buffer) == 0) {
    return jsoncPath;
  }

  throw std::runtime_error(
      "Neither packages.json nor packages.jsonc file exists");
}

// Function to read package file content
std::string readFileContent(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open packages file: " + filePath);
  }
  std::string fileContent((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
  file.close();
  return fileContent;
}

// Function to parse package file
std::vector<std::string> parsePackageFile(const std::string &fileContent,
                                          bool isJsonc) {
  std::string jsonContent = isJsonc ? removeComments(fileContent) : fileContent;

  Value packageData;
  CharReaderBuilder reader;
  std::string errs;
  std::istringstream stream(jsonContent);
  if (!parseFromStream(reader, stream, &packageData, &errs)) {
    throw std::runtime_error("Failed to parse packages file: " + errs);
  }

  std::vector<std::string> packages;
  if (packageData.isMember("packages") && packageData["packages"].isArray()) {
    for (Value::ArrayIndex i = 0; i < packageData["packages"].size(); ++i) {
      packages.push_back(packageData["packages"][i].asString());
    }
  } else {
    throw std::runtime_error("Invalid packages file format");
  }
  return packages;
}

// Function to handle packages
void handlePackages(const std::vector<std::string> &packages) {
  for (const std::string &package : packages) {
    if (!isPackageInstalled(package)) {
      std::cout << "Installing package: " << package << std::endl;
      installPackage(package);
    } else {
      std::cout << "Package " << package << " is already installed."
                << std::endl;
    }
  }
}

int main() {
  try {
    std::string packageFilePath = getPackageFilePath();

    // Determine if the file is JSON or JSONC
    bool isJsonc = packageFilePath.find(".jsonc") != std::string::npos;

    std::string fileContent = readFileContent(packageFilePath);
    std::vector<std::string> packages = parsePackageFile(fileContent, isJsonc);
    handlePackages(packages);

    std::cout << "All packages are installed." << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
