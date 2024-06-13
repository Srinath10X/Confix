#include <argparse/argparse.hpp>
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

const std::string VERSION = "0.1.0";

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
std::string getPackageFilePath(const std::string &customPath = "") {
  const char *envPath = getenv("CONFIX_PACKAGES_PATH");
  if (envPath && strlen(envPath) > 0) {
    return std::string(envPath);
  }

  const char *homeDir = getenv("HOME");
  if (!homeDir) {
    throw std::runtime_error("Failed to get home directory");
  }

  if (!customPath.empty()) {
    return customPath;
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

// Function to update package file
void updatePackageFile(const std::string &filePath,
                       const std::vector<std::string> &packages, bool isJsonc) {
  Value packageData;
  packageData["packages"] = Json::arrayValue;

  for (const std::string &package : packages) {
    packageData["packages"].append(package);
  }

  std::ofstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open packages file for writing: " +
                             filePath);
  }

  if (isJsonc) {
    // Writing JSONC content requires preserving comments, which is not
    // straightforward. Here we just write the JSON part.
    file << packageData.toStyledString();
  } else {
    file << packageData.toStyledString();
  }

  file.close();
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

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("confix", VERSION);

  program.add_argument("-f", "--file")
      .help("specify a custom package file path")
      .default_value(std::string(""));

  program.add_argument("-c", "--check")
      .help("check if a specific package is installed")
      .default_value(std::string(""));

  program.add_argument("-i", "--install")
      .help("install a specific package")
      .default_value(std::string(""));

  try {
    program.parse_args(argc, argv);

    std::string customFilePath = program.get<std::string>("--file");
    std::string checkPackage = program.get<std::string>("--check");
    std::string installPackageName = program.get<std::string>("--install");

    if (!checkPackage.empty()) {
      if (isPackageInstalled(checkPackage)) {
        std::cout << "Package " << checkPackage << " is installed."
                  << std::endl;
      } else {
        std::cout << "Package " << checkPackage << " is not installed."
                  << std::endl;
      }
      return 0;
    }

    std::string packageFilePath = getPackageFilePath(customFilePath);
    bool isJsonc = packageFilePath.find(".jsonc") != std::string::npos;
    std::string fileContent = readFileContent(packageFilePath);
    std::vector<std::string> packages = parsePackageFile(fileContent, isJsonc);

    if (!installPackageName.empty()) {
      if (!isPackageInstalled(installPackageName)) {
        installPackage(installPackageName);
        std::cout << "Package " << installPackageName << " has been installed."
                  << std::endl;

        packages.push_back(installPackageName);
        updatePackageFile(packageFilePath, packages, isJsonc);
      } else {
        std::cout << "Package " << installPackageName
                  << " is already installed." << std::endl;
      }
      return 0;
    }

    handlePackages(packages);
    std::cout << "All packages are installed." << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
