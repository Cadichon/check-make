#ifndef ARG_HPP
#define ARG_HPP

class Argument {
public:
  Argument(char argc, char **argv);
  ~Argument() = default;
  bool isRecursive() const;
  bool isVerbose() const;
  const std::string &getMakefilePath() const;
  const std::string &getRulesPath() const;
  bool operator==(bool test) const;
  bool operator!() const;
  //TOTO: make a getRules method;

private:
  bool _isGood;
  bool _recursive;
  bool _verbose;
  std::string _makefilePath;
  std::string _rulesPath;
  //TODO: add a Rules object
};

#endif
