#include <cstdlib>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace Json;

// Function to check if a package is installed using yay
bool is_package_installed(const std::string &package_name) {
  std::string command = "yay -Qs " + package_name + " > /dev/null 2>&1";
  int result = std::system(command.c_str());
  return result == 0;
}

// Function to install a package using yay
void install_package(const std::string &package_name) {
  std::string command = "yay -S --noconfirm " + package_name;
  int result = std::system(command.c_str());
  if (result != 0) {
    throw std::runtime_error("Failed to install package: " + package_name);
  }
}

// Function to remove comments from JSONC content
std::string remove_comments(const std::string &jsonc_content) {
  std::string result = jsonc_content;

  // Remove single-line comments
  std::regex single_line_comment_pattern("//.*");
  result = std::regex_replace(result, single_line_comment_pattern, "");

  // Remove multi-line comments
  std::regex multi_line_comment_pattern("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/");
  result = std::regex_replace(result, multi_line_comment_pattern, "");

  return result;
}

int main() {
  try {
    // Get the home directory from the environment variable
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
      throw std::runtime_error("Failed to get home directory");
    }

    // Get the CONFIX_PACKAGES_PATH environment variable
    const char *package_path_env = getenv("CONFIX_PACKAGES_FILE_PATH");
    std::string package_file_path;

    if (package_path_env) {
      package_file_path = package_path_env;
    } else {
      // Default package file path
      package_file_path =
          std::string(home_dir) + "/.config/confix/packages.jsonc";
    }

    // Open and read the JSON file
    std::ifstream file(package_file_path);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open packages.jsonc file");
    }

    // Read file content into a string (to preprocess for JSONC)
    std::string file_content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();

    // Remove comments from JSONC content
    std::string json_content = remove_comments(file_content);

    Value package_data;
    CharReaderBuilder reader;
    std::string errs;

    std::istringstream stream(json_content);
    if (!parseFromStream(reader, stream, &package_data, &errs)) {
      throw std::runtime_error("Failed to parse packages.jsonc file: " + errs);
    }

    // Extract package names
    std::vector<std::string> packages;
    if (package_data.isMember("packages") &&
        package_data["packages"].isArray()) {
      for (Value::ArrayIndex i = 0; i < package_data["packages"].size(); ++i) {
        packages.push_back(package_data["packages"][i].asString());
      }
    } else {
      throw std::runtime_error("Invalid packages.jsonc file format");
    }

    // Check and install each package
    for (const std::string &package : packages) {
      if (!is_package_installed(package)) {
        std::cout << "Installing package: " << package << std::endl;
        install_package(package);
      } else {
        std::cout << "Package " << package << " is already installed."
                  << std::endl;
      }
    }

    std::cout << "All packages are installed." << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
