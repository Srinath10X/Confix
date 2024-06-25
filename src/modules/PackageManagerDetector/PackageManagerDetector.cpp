#include "PackageManagerDetector.h"
#include <cstdio>

bool commandExists(const std::string &command) {
  std::string cmd = "command -v " + command + " 2>&1";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    return false;

  char buffer[128];
  std::string result = "";
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);

  return !result.empty();
}

std::string detectPackageManager() {
  if (commandExists("yay")) {
    return "yay";
  } else if (commandExists("pacman")) {
    return "pacman";
  } else if (commandExists("apt")) {
    return "apt";
  } else if (commandExists("dnf")) {
    return "dnf";
  } else if (commandExists("yum")) {
    return "yum";
  } else if (commandExists("zypper")) {
    return "zypper";
  } else if (commandExists("brew")) {
    return "brew";
  } else if (commandExists("port")) {
    return "port";
  } else if (commandExists("pkg")) {
    return "pkg";
  } else {
    return "unknown";
  }
}
