#ifndef QIAN_DEBUG_H_
#define QIAN_DEBUG_H_

#include "base.h"
#include "logging.h"
#include "stack.h"
#include "type.h"

namespace qian {

string Strip(const string& str);

void DumpStack(Stack<Value, STACK_SIZE>* stk);


} // namespace qian

#endif  // QIAN_DEBUG_H_
