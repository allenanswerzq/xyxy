#include "debug.h"

namespace qian {

string Strip(const string& str) {
  int n = str.size();
  int i = 0;
  while (i < n && (str[i]  == '\n' || str[i] == ' ')) {
    i++;
  }
  int j = n - 1;
  while (j > i && (str[j] == '\n' || str[j] == ' ')) {
    j--;
  }
  return str.substr(i, j - i + 1);
}

void DumpStack(Stack<Value, STACK_SIZE>* stk) {
  // TODO(zq7): dump stack content.
}

} // namespace qian
