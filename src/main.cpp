#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

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

int main() {
  try {
    // Get the home directory from the environment variable
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
      throw std::runtime_error("Failed to get home directory");
    }

    // Construct the full path to the packages.json file
    std::string package_file_path =
        std::string(home_dir) + "/.config/confix/packages.json";

    // Open and read the JSON file
    std::ifstream file(package_file_path);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open packages.json file");
    }

    json package_data;
    file >> package_data;
    file.close();

    // Extract package names
    std::vector<std::string> packages =
        package_data["packages"].get<std::vector<std::string>>();

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
