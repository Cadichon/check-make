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
  this->_extractReceipes();
  this->_extractPhony();
  if (this->_verbose) {
    std::cout << "=== Makefile variable begin ===" << std::endl;
    std::cout << this->getVariables() << std::endl;
    std::cout << "=== Makefile variable end ===" << std::endl;
    std::cout << "=== Makefile receipes begin ===" << std::endl;
    std::cout << this->getReceipes() << std::endl;
    std::cout << "=== Makefile receipes end ===" << std::endl;
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

  if (this->_isReceipeCommand(line))
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

  if (this->_isReceipeCommand(line))
    return false;
  if (found < 0)
    return false;
  if (line[foundFirst] == ':')
    return false;
  return true;
}

bool Makefile::_isReceipeTarget(const std::string &line) const
{
  int found = line.find_first_of("=:");

  if (this->_isVariable(line))
    return false;
  if (line[found] != ':')
    return false;
  return true;
}

bool Makefile::_isReceipeCommand(const std::string &line) const
{
  auto found = this->_variables.find(".RECIPEPREFIX");
  std::string recipePrefix;

  if (found != this->_variables.end()) {
    recipePrefix = this->_variables.at(".RECIPEPREFIX");
  }
  else {
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


void Makefile::_extractReceipes()
{
  for (auto it = this->_makefile.begin(); it != this->_makefile.end(); it++) {
    if (this->_isReceipeTarget(*it)) {
      int foundColon = it->find(":");
      int foundSemicolon = it->find(";");
      Receipe receipe;
      
      receipe.target = std::string(*it, 0, foundColon);
      epur(receipe.target);
      if (it->begin() + foundColon + 1 != it->end()) {
        if (foundSemicolon != -1)
          receipe.deps = std::string(*it, foundColon + 1, foundSemicolon - foundColon - 1);
        else {
          receipe.deps = std::string(*it, foundColon + 1);
        }
        epur(receipe.deps);
      }
      if (foundSemicolon != -1) {
        receipe.cmds.push_back(std::string(*it, foundSemicolon + 1));
        epur(receipe.cmds.back());
      }
      if (std::next(it) != this->_makefile.end() && this->_isReceipeCommand(*(std::next(it)))) {
        it++;
        while (it != this->_makefile.end() && this->_isReceipeCommand(*it)) {
          receipe.cmds.push_back(*it);
          epur(receipe.cmds.back());
          it++;
        }
        it--;
      }
      this->_receipes.push_back(receipe);
    }
  }
}

void Makefile::_extractPhony()
{
  auto phony = std::find_if(this->_receipes.begin(), this->_receipes.end(), [](const Receipe &r) -> bool { return r.target == ".PHONY";});

  if (phony == this->_receipes.end() && phony->target != ".PHONY")
    return;
  this->_phony = phony->deps;
  erase(this->_receipes, phony);
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

const std::string Makefile::getReceipes() const
{
  std::string out;

  for (auto it = this->_receipes.begin(); it != this->_receipes.end(); it++) {
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
    if (std::next(it) != this->_receipes.end())
      out += "\n\n";
  }
  return out;
}
