#ifndef __MAKEFILE_HPP_
#define __MAKEFILE_HPP_

#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <exception>
#include <map>
#include <list>
#include "utils.hpp"

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
  const std::string getReceipes() const;
private:
  struct Receipe {
    std::string target;
    std::string deps;
    std::list<std::string> cmds; 
  };
  bool _isVariable(const std::string &line) const;
  bool _isVariableModifier(const std::string &line) const;
  bool _isReceipeTarget(const std::string &line) const;
  bool _isReceipeCommand(const std::string &line) const;
  void _cleanMakefile();
  void _extractVariables();
  void _extractVariableModifiers();
  void _extractReceipes();
  void _extractPhony();
  std::string _makefilePath;
  bool _verbose;
  std::map<std::string, std::string> _variables;
  std::list<Receipe> _receipes;
  std::list<std::string> _makefile;
  std::string _phony;
};

#endif
