// FileCrawler.h: plik dołączany dla standardowych systemowych plików dołączanych,
// lub pliki dołączane specyficzne dla projektu.

#pragma once

#include <iostream>
#include <filesystem>
namespace filesystem = std::filesystem;

// TODO: W tym miejscu przywołaj dodatkowe nagłówki wymagane przez program.
unsigned int countLinesInFile(filesystem::path path);