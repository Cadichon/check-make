#include "makefile.hpp"

Makefile::Makefile(const std::string &makefilePath, bool verbose) : _makefilePath(makefilePath), _verbose(verbose)
{
  std::string line;
  std::ifstream file(makefilePath);

  if (!file.is_open()) {
    throw MakefileException("Failed to open " + makefilePath);
  }
  while (std::getline(file, line)) {
    this->_makefile.push_back(line);
  }

  this->_cleanMakefile();
  this->_extractVariables();
  this->_extractVariableModifiers();
  this->_extractRules();
  this->_extractPhony();
  if (this->_verbose) {
    std::cout << "=== Makefile variable begin ===" << std::endl;
    std::cout << this->getVariables() << std::endl;
    std::cout << "=== Makefile variable end ===" << std::endl;
    std::cout << "=== Makefile rules begin ===" << std::endl;
    std::cout << this->getRules() << std::endl;
    std::cout << "=== Makefile rules end ===" << std::endl;
    if (!this->_phony.empty()) {
      std::cout << "=== Makefile .PHONY begin ===" << std::endl;
      std::cout << this->_phony << std::endl;
      std::cout << "=== Makefile .PHONY end ===" << std::endl;
    }
  }
}

bool Makefile::_isVariable(const std::string &line) const
{
  int found = line.find_first_of("=:+");

  if (this->_isRuleCommand(line))
    return false;
  if (found < 0)
    return false;
  if (line[found] == '+')
    return false;
  return line[found] == '=' || (line[found] == ':' && line[found + 1] == '=');
}

bool Makefile::_isVariableModifier(const std::string &line) const 
{
  int found = line.find("+=");
  int foundFirst = line.find_first_of("=:");

  if (this->_isRuleCommand(line))
    return false;
  if (found < 0)
    return false;
  if (line[foundFirst] == ':')
    return false;
  return true;
}

bool Makefile::_isRuleTarget(const std::string &line) const
{
  int found = line.find_first_of("=:");

  if (this->_isVariable(line))
    return false;
  if (line[found] != ':')
    return false;
  return true;
}

bool Makefile::_isRuleCommand(const std::string &line) const
{
  std::string recipePrefix;

  try {
    recipePrefix = this->_variables.at(".RECIPEPREFIX");
  }
  catch (std::out_of_range) {
    recipePrefix = "\t";
  }
  return starts_with(line, recipePrefix);
}

void Makefile::_cleanMakefile() {
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
      std::replace(it, std::next(it), line, reconstituedLine);
      previousLineIsBackslashEnded = false;
    }
    it++;
  }
}

void Makefile::_extractVariables()
{
  for (const std::string &line: this->_makefile) {
    if (this->_isVariable(line)) {
      int found = line.find_first_of("=:");
      int equalPos = (line[found] == '=' ? found : found + 1);
      std::string name(line, 0, found);
      std::string content(line, equalPos + 1);

      epur(name);
      epur(content);
      this->_variables[name] = content;
    }
  }
}

void Makefile::_extractVariableModifiers()
{
  for (const std::string &line: this->_makefile) {
    if (this->_isVariableModifier(line)) {
      int found = line.find("+=");
      int equalPos = found + 1;
      std::string name(line, 0, found);
      std::string addedContent(line, equalPos + 1);

      epur(name);
      epur(addedContent);
      this->_variables[name] += addedContent;
    }
  }
}


void Makefile::_extractRules()
{
  for (auto it = this->_makefile.begin(); it != this->_makefile.end(); it++) {
    if (this->_isRuleTarget(*it)) {
      int foundColon = it->find(":");
      int foundSemicolon = it->find(";");
      Rule rule;
      
      rule.target = std::string(*it, 0, foundColon);
      epur(rule.target);
      if (it->begin() + foundColon + 1 != it->end()) {
	if (foundSemicolon != -1)
	  rule.deps = std::string(*it, foundColon + 1, foundSemicolon - foundColon - 1);
	else
	  rule.deps = std::string(*it, foundColon + 1);
	epur(rule.deps);
      }
      if (foundSemicolon != -1) {
	rule.cmds.push_back(std::string(*it, foundSemicolon + 1));
	epur(rule.cmds.back());
      }
      if (std::next(it) != this->_makefile.end() && this->_isRuleCommand(*std::next(it))) {
	it++;
	while (this->_isRuleCommand(*it)) {
	  rule.cmds.push_back(*it);
	  epur(rule.cmds.back());
	  it++;
	}
	it--;
      }
      this->_rules.push_back(rule);
    }
  }
}

void Makefile::_extractPhony()
{
  auto phony = std::find_if(this->_rules.begin(), this->_rules.end(), [](const Rule &r) -> bool { return r.target == ".PHONY";});

  if (phony == this->_rules.end() && phony->target != ".PHONY")
    return;
  this->_phony = phony->deps;
  erase(this->_rules, phony);
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
	out += (" - '" + *it2 + "'");
	if (std::next(it2) != it->cmds.end())
	  out += "\n";
      }
    }
    if (std::next(it) != this->_rules.end())
      out += "\n\n";
  }
  return out;
}
