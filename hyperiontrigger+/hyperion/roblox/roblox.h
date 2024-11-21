#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <regex>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include "..\xorstr\xorstr.hpp"

long long GetRenderViewFromLog();
std::unique_ptr<std::ifstream> GetLog();
void ClearLog();