#include <getopt.h>
#include <string>
#include <iostream>
#include "argument.hpp"

static const option long_opts[] = {
  {"makefile", required_argument, nullptr, 'm'},
  {"rules", required_argument, nullptr, 'r'},
  {"recursive", no_argument, nullptr, 'R'},
  {"verbose", no_argument, nullptr, 'v'},
  {nullptr, no_argument, nullptr, 0}
};

static const char *short_opts = "m:r:Rv";

Argument::Argument(char argc, char **argv) : _isGood(true), _recursive(false), _verbose(false), _makefilePath("./Makefile"), _rulesPath("./RULES")
{
  int opt;

  while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
    switch (opt) {
      case 'm':
	this->_makefilePath = optarg;
	break;
      case 'r':
	this->_rulesPath = optarg;
	break;
      case 'R':
	this->_recursive = true;
	break;
      case 'v':
	this->_verbose = true;
	break;
      default:
	std::cout << "usage: " << std::endl;
	std::cout << "\t" << argv[0] << " [-m|--makefile m-path] [-r|--rules r-path] [-v|--verbose] [-R|--recursive]" << std::endl;
	std::cout << "\t\t" << "m-path: path to a makefile (default to \"./Makefile\")" << std::endl;
	std::cout << "\t\t" << "m-path: path to a RULES config file (default to \"./RULES\")" << std::endl;
	this->_isGood = false;
    }
  }
}

bool Argument::isRecursive() const
{
  return this->_recursive;
}

bool Argument::isVerbose() const
{
  return this->_verbose;
}

const std::string &Argument::getMakefilePath() const
{
  return this->_makefilePath;
}

const std::string &Argument::getRulesPath() const
{
  return this->_rulesPath;
}

bool Argument::operator==(bool test) const
{
  return this->_isGood == test;
}

bool Argument::operator!() const
{
  return !this->_isGood;
}
