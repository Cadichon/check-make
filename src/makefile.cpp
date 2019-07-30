#include "makefile.hpp"
#include "utils.hpp"
#include <iostream>

Makefile::Makefile(const std::string &makefilePath) : _makefilePath(makefilePath)
{
  std::string line;
  std::ifstream file(makefilePath);

  if (!file.is_open()) {
    throw MakefileException("Failed to open " + makefilePath);
  }
  while (std::getline(file, line)) {
    this->_makefile.push_back(line);
  }
  this->_removeUselessLine();
#ifdef __DEBUG_MAKEFILE
  std::cout << "=== Makefile clean begin ===" << std::endl;
  std::cout << this->getMakefile() << std::endl;
  std::cout << "=== Makefile clean end ===" << std::endl;
#endif

  this->_extractVariables();
#ifdef __DEBUG_MAKEFILE
  std::cout << "=== Makefile variable begin ===" << std::endl;
  std::cout << this->getVariables() << std::endl;
  std::cout << "=== Makefile variable end ===" << std::endl;
#endif
 
  this->_extractRules();
#ifdef __DEBUG_MAKEFILE
  std::cout << "=== Makefile rules begin ===" << std::endl;
  std::cout << this->getRules() << std::endl;
  std::cout << "=== Makefile rules end ===" << std::endl;
#endif
}

bool Makefile::_isVariable(const std::string &line)
{
  int found = line.find_first_of("=:");

  if (found < 0)
    return false;
  return line[found] == '=' || (line[found] == ':' && line[found + 1] == '=');
}

bool Makefile::_isRuleTarget(const std::string &line)
{
  int foundDot = line.find_first_of(":");
  int foundEqual = line.find_first_of("=");
  
  if (this->_isVariable(line))
    return false;
  if (foundDot < 0)
    return false;
  if (foundEqual > 0 && foundEqual < foundDot)
    return false;
  if (!line.compare(0, 6, ".PHONY"))
    return false;
  return true;
}

void Makefile::_removeUselessLine() {
  std::string reconstituedLine;
  std::list<std::string> lineToReconstituate;
  bool previousLineIsBackslashEnded = false;
  auto it = this->_makefile.begin();

  while (it != this->_makefile.end()) {
    std::string &line = *it;

    if (line.empty() || starts_with(line, "#")) {
      erase(this->_makefile, it);
      continue;
    } 
    else if (ends_with(line, "\\")) {
      line.pop_back();
      lineToReconstituate.push_back(line);
      previousLineIsBackslashEnded = true;
      erase(this->_makefile, it);
      continue;
    }
    else if (!ends_with(line, "\\") && previousLineIsBackslashEnded) {
      lineToReconstituate.push_back(line);
      while (!lineToReconstituate.empty()) {
	reconstituedLine += lineToReconstituate.front();
	lineToReconstituate.pop_front();
      }
      epur(reconstituedLine);
      std::replace(it, std::next(it), line, reconstituedLine);
      previousLineIsBackslashEnded = false;
    }
    else if (!ends_with(line, "\\")) {
      epur(line);
    }
    it++;
  }
}

void Makefile::_extractVariables()
{
  for (auto it = this->_makefile.begin(); it != this->_makefile.end(); it++) {
    std::string &line = *it;
    int found = line.find_first_of("=:");

    if (found < 0)
      continue;
    if (this->_isVariable(line)) {
      std::string name;
      std::string content;
      int equalPos = (line[found] == '=' ? found : found + 1);
      
      std::copy(line.begin(), line.begin() + found, std::back_inserter(name));
      epur(name);
      std::copy(line.begin() + equalPos + 1, line.end(), std::back_inserter(content));
      epur(content);
      this->_variables[name] = content;
    }
  }
}

void Makefile::_extractRules()
{
  auto it = this->_makefile.begin();
  
  while (it != this->_makefile.end()) {
    if (starts_with(*it, ".PHONY")) {
      it++;
      continue;
    }
    if (this->_isRuleTarget(*it)) {
      std::string firstLine = *it;
      int found = firstLine.find_first_of(":");
      std::string deps;
      std::list<std::string> commands;
      Rule rule;
      
      std::copy(firstLine.begin(), firstLine.begin() + found, std::back_inserter(rule.target));
      epur(rule.target);      
      if (firstLine.begin() + found + 1 != firstLine.end()) {
	std::copy(firstLine.begin() + found + 1, firstLine.end(), std::back_inserter(rule.deps));
	epur(rule.deps);
      }
      if (!this->_isRuleTarget(*(std::next(it)))) {
	it++;
	while (!this->_isRuleTarget(*it) && !starts_with(*it, ".PHONY")) {
	  commands.push_back(*it);
	  it++;
	}
	it--;
	rule.cmds = commands;
      }
      this->_rules.push_back(rule);
    }
    it++;
  }
}

const std::string Makefile::getMakefile() const
{
  std::string out;
  
  for (auto it = this->_makefile.begin(); it != this->_makefile.end(); it++) {
    out += *it;
    if (std::next(it) != this->_makefile.end())
      out += "\n";
  }
  return out;
}

const std::string Makefile::getVariables() const
{
  std::string out;

  for (auto it = this->_variables.begin(); it != this->_variables.end(); it++) {
    out += ("[" + it->first + "] = '" + it->second + "'");
    if (std::next(it) != this->_variables.end())
      out += "\n";
  }
  return out;
}

const std::string Makefile::getRules() const
{
  std::string out;

  for (auto it = this->_rules.begin(); it != this->_rules.end(); it++) {
    out += ("target = '" + it->target + "'");
    if (!it->deps.empty())
      out += ("\ndeps = '" + it->deps+ "'");
    if (!it->cmds.empty()) {
      out += "\ncommands:\n";
      for (auto it2 = it->cmds.begin(); it2 != it->cmds.end(); it2++) {
	out += ("\t" + *it2);
	if (std::next(it2) != it->cmds.end())
	  out += "\n";
      }
    }
    if (std::next(it) != this->_rules.end())
      out += "\n\n";
  }
  return out;
}
