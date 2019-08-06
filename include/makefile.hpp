#ifndef __MAKEFILE_HPP_
#define __MAKEFILE_HPP_

#include <string>
#include <fstream>
#include <exception>
#include <map>
#include <list>

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
  Makefile(const std::string &makefilePath, bool verbose = false);
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
  bool _isVariableModifier(const std::string &line) const;
  bool _isRuleTarget(const std::string &line) const;
  bool _isRuleCommand(const std::string &line) const;
  void _cleanMakefile();
  void _extractVariables();
  void _extractVariableModifiers();
  void _extractRules();
  void _extractPhony();
  std::string _makefilePath;
  bool _verbose;
  std::map<std::string, std::string> _variables;
  std::list<Rule> _rules; 
  std::list<std::string> _makefile;
  std::string _phony;
};

#endif
