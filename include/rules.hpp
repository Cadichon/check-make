#ifndef __RULES_HPP
#define __RULES_HPP

#include <iomanip>
#include "makefile.hpp"
#include "json.hpp"

using json = nlohmann::json;

class Rules {
public:
  Rules(const std::string &path, bool verbose = false);
  ~Rules();
  int check(const Makefile &makefile);
private:
  std::string _path; 
  bool _verbose;
  json _rules;
};

#endif
