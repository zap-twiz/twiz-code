
#include "parser/parse_node.h"

#include <assert.h>

void ParseNode::VisitChildrenRightToLeft(Visitor * visitor) const {
  if (!visitor)
    return;

  if (terminals_.empty() && non_terminals_.empty())
    return;

  TerminalArray::const_reverse_iterator terminal_iter(terminals_.rbegin()),
      terminal_end(terminals_.rend());

  NonTerminalArray::const_reverse_iterator
      non_terminal_iter(non_terminals_.rbegin()),
      non_terminal_end(non_terminals_.rend());

  size_t offset = size() - 1;
  for (; offset != 0; --offset) {
    if (terminal_end != terminal_iter
        && offset == terminal_iter->second) {
      visitor->VisitTerminal(terminal_iter->first, offset);
      terminal_iter++;
      continue;
    }

    if (non_terminal_end != non_terminal_iter
        && offset == non_terminal_iter->second) {
      visitor->VisitNonTerminal(non_terminal_iter->first, offset);
      non_terminal_iter++;
      continue;
    }

    assert(false);
  }
}

void ParseNode::VisitChildrenLeftToRight(Visitor * visitor) const {
  if (!visitor)
    return;
}

