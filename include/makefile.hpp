#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <list>
#include "json.hpp"

class MakefileException : public std::exception {
public:
  MakefileException(const std::string &what) : _what(what) {}
  ~MakefileException() = default;
  const char *what() const noexcept {
    return this->_what.c_str();
  }
private:
  std::string _what;
};

class Makefile {
public:
  Makefile(const std::string &makefilePath);
  ~Makefile() = default;
  const std::string getMakefile() const;
  const std::string getVariables() const;
  const std::string getRules() const;
private:
  struct Rule {
    std::string target;
    std::string deps;
    std::list<std::string> cmds; 
  };
  bool _isVariable(const std::string &line) const;
  bool _isRuleTarget(const std::string &line) const;
  bool _isRuleCommand(const std::string &line) const;
  void _cleanMakefile();
  void _extractVariables();
  void _extractRules();
  std::string _makefilePath;
  std::map<std::string, std::string> _variables;
  std::list<Rule> _rules; 
  std::list<std::string> _makefile;
};
