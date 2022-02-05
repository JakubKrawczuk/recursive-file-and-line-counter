# CMakeList.txt: projekt CMake dla elementu FileCrawler, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (FileCrawler "FileCrawler.cpp" "FileCrawler.h")

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.
enable_testing()

add_executable(
  FCTest
  "FCTest.cc"
)
target_link_libraries(
  FCTest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(FCTest)