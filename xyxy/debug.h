#ifndef XYXY_DEBUG_H_
#define XYXY_DEBUG_H_

#include "base.h"
#include "logging.h"
#include "stack.h"
#include "type.h"

namespace xyxy {

string Strip(const string& str);

void DumpStack(Stack<Value, STACK_SIZE>* stk);

}  // namespace xyxy

#endif  // XYXY_DEBUG_H_
