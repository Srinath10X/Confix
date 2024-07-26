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

const std::string VERSION = "confix v0.1.1";

// Function to check if a package is available in AUR
bool isPackageAvailableInAUR(const std::string &packageName) {
  std::string command = "yay -Si " + packageName + " > /dev/null 2>&1";
  int result = std::system(command.c_str());
  return result == 0;
}

// Function to check if a package is installed using yay
bool isPackageInstalled(const std::string &packageName) {
  std::string command = "yay -Q " + packageName + " > /dev/null 2>&1";
  int result = std::system(command.c_str());
  return result == 0;
}

// Function to install a package using yay
void installPackage(const std::string &packageName) {
  // Check if package is available in AUR
  if (!isPackageAvailableInAUR(packageName)) {
    throw std::runtime_error("Package " + packageName + " not found in AUR.");
  }

  std::string command = "yay -Sy --noconfirm " + packageName;
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

void handlePackages(const std::vector<std::string> &packages) {
  std::string command = "yay -Qs ";
  for (const std::string &package : packages) {
    command += package + " ";
  }
  int result = std::system(command.c_str());

  std::vector<std::string> packagesToInstall;
  if (result != 0) {
    for (const std::string &package : packages) {
      if (!isPackageInstalled(package)) {
        packagesToInstall.push_back(package);
      } else {
        std::cout << "Package " << package << " is already installed."
                  << std::endl;
      }
    }
  } else {
    std::cout << "All packages are installed." << std::endl;
  }

  for (const std::string &package : packagesToInstall) {
    std::cout << "Installing package: " << package << std::endl;
    installPackage(package);
  }
}

std::string execCommand(const std::string &command) {
  char buffer[128];
  std::string result = "";
  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("Failed to execute command: " + command);
  }
  try {
    while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL)
        result += buffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return result;
}

void sortAndUpdatePackagesInFile(const std::string &filePath, bool isJsonc) {
  // Read the file content
  std::string fileContent = readFileContent(filePath);

  // Parse the package file
  std::vector<std::string> packages = parsePackageFile(fileContent, isJsonc);

  // Sort the packages
  std::sort(packages.begin(), packages.end());

  // Update the package file
  updatePackageFile(filePath, packages, isJsonc);

  // Display sorted packages
  std::cout << "Packages have been sorted and updated in the file:\n";
}

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("confix", VERSION);

  program.add_argument("-f", "--file")
      .help("specify a custom package file path")
      .default_value(std::string(""));

  program.add_argument("-c", "--check")
      .help("check if a specific package is installed")
      .default_value(std::string(""))
      .metavar("PACKAGE");

  program.add_argument("-i", "--install")
      .help("install specific packages")
      .nargs(argparse::nargs_pattern::at_least_one)
      .default_value(std::vector<std::string>{})
      .metavar("PACKAGE");

  program.add_argument("-s", "--sort")
      .help("sort packages alphabetically in the package file")
      .default_value(false)
      .implicit_value(true);

  try {
    program.parse_args(argc, argv);

    std::string customFilePath = program.get<std::string>("--file");
    std::string checkPackage = program.get<std::string>("--check");
    std::vector<std::string> installPackages =
        program.get<std::vector<std::string>>("--install");
    bool sortPackages = program.get<bool>("--sort");

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

    if (sortPackages) {
      sortAndUpdatePackagesInFile(packageFilePath, isJsonc);
      return 0;
    }

    std::string fileContent = readFileContent(packageFilePath);
    std::vector<std::string> packages = parsePackageFile(fileContent, isJsonc);

    if (!installPackages.empty()) {
      for (const std::string &installPackageName : installPackages) {
        try {
          installPackage(installPackageName);
          std::cout << "Package " << installPackageName
                    << " has been installed." << std::endl;

          // Check for duplication before adding
          if (std::find(packages.begin(), packages.end(), installPackageName) ==
              packages.end()) {
            packages.push_back(installPackageName);
            updatePackageFile(packageFilePath, packages, isJsonc);
          }
        } catch (const std::runtime_error &ex) {
          std::cerr << "Error: " << ex.what() << std::endl;
        }
      }
      return 0;
    }

    handlePackages(packages);
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
