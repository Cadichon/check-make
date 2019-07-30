#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

inline bool ends_with(const std::string &value, const std::string &ending)
{
  if (ending.size() > value.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline bool starts_with(const std::string &value, const std::string &starting)
{
  if (starting.size() > value.size())
    return false;
  return (value.find(starting) == 0);
}

inline void epur(std::string &s)
{
  bool space = false;
  auto p = s.begin();
  for (auto ch : s)
    if (std::isspace(ch)) {
      space = p != s.begin();
    } else {
      if (space) *p++ = ' ';
      *p++ = ch;
      space = false; }
  s.erase(p, s.end());
}

template <typename C, typename T>
void erase(C &container, T &it)
{
  auto tmpIt = std::next(it);
  container.erase(it);
  it = tmpIt;
}

#endif
