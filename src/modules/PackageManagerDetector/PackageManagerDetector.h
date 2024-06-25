#ifndef PACKAGEMANAGERDETECTOR_H
#define PACKAGEMANAGERDETECTOR_H

#include <string>

bool commandExists(const std::string &command);
std::string detectPackageManager();

#endif
