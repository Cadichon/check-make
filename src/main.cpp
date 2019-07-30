#include <iostream>
#include "argument.hpp"
#include "makefile.hpp"

int main(int argc, char **argv)
{
  Argument arg(argc, argv);

  if (!arg)
    return (-1);
  if (arg.isVerbose()) {
    std::cout << "Makefile path = " << arg.getMakefilePath() << std::endl;
    std::cout << "Rules path = " << arg.getRulesPath() << std::endl;
    std::cout << "Recursive is " << (arg.isRecursive() ? "on" : "off") << std::endl;
    std::cout << "Verbose is " << (arg.isVerbose() ? "on" : "off") << std::endl;
  }
  Makefile makefile(arg.getMakefilePath());

  return (0);
}
