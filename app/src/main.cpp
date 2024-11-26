#include "fluffyApplication.hpp"

auto main(int argc, char const *argv[]) -> int {
  try {
    auto app = FluffyApplication{argc, argv};
  }
  catch(...) {
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}