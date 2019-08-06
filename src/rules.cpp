#include "rules.hpp"

Rules::Rules(const std::string &path, bool verbose) : _path(path), _verbose(verbose)
{
  std::ifstream file(path);

  if (!file.is_open()) {
    throw MakefileException("Failed to open " + path);
  }
  try {
    file >> this->_rules;
  }
  catch (nlohmann::detail::parse_error e) {
    throw MakefileException(path + " is not a valid JSON file");
  }
  if (this->_verbose) {
    std::cout << std::setw(4) << this->_rules << std::endl;
  }
}

Rules::~Rules()
{}

int Rules::check(const Makefile &makefile)
{
  (void)makefile;
  return 0;
}
