#pragma once
#include "Utilities/XAssert.h"

namespace Eks
{
template <typename Node, typename Derived> class IntrusiveLinkedListBase
  {
public:
  /// Find the next node from prev
  static Node *getNext(Node *prev)
    {
    xAssert(prev);
    Node **location = Derived::getNextLocation(prev);

    xAssert(location);
    return *location;
    }

  /// Append the node [val] to the list at [start] and store the [index]
  /// \returns the node prior to the inserted node
  static Node *append(Node **start, Node *val, xsize *index)
    {
    xAssert(!Derived::getNext(val));
    Node *node = nullptr;
    Node **location = start;

    *index = 0;
    while(*location)
      {
      node = *location;

      location = Derived::getNextLocation(node);
      xAssert(*location != val);

      ++*index;
      }

    appendAt(location, val);
    return node;
    }

  /// Insert [val] at [index] in the list starting at [start].
  /// \returns the node before the inserted node
  static Node* insert(Node** start, Node *val, xsize index)
    {
    xAssert(val);

    Node *node = nullptr;
    Node **location = start;

    xsize count = 0;
    while(*location && index != count)
      {
      ++count;
      node = *location;
      location = Derived::getNextLocation(node);
      }

    // failed to find insert point.
    if(index != count)
      {
      xAssertFail();
      return nullptr;
      }

    appendAt(location, val);
    return node;
    }

  /// Append the node [val] at [location]
  static void appendAt(Node **location, Node *val)
    {
    xAssert(location);
    xAssert(val);

    xAssert(!Derived::getNext(val));

    // insert this prop into the list
    *Derived::getNextLocation(val) = *location;

    *location = val;
    }

  /// Remove the node [val] from the list [start]
  /// \returns the node that used to be previous to [val]
  static Node *remove(Node **start, Node *val)
    {
    Node *node = nullptr;
    Node **location = start;
    while(*location)
      {
      xAssert(*location != Derived::getNext(*location));

      if((*location) == val)
        {
        (*location) = Derived::getNext(*location);
        break;
        }

      node = *location;
      location = Derived::getNextLocation(node);
      }

    // Node isn't in the list.
    xAssertFail();
    // It seems wrong to change [val] in this case?
    // Maybe don't do this.
    *Derived::getNextLocation(val) = nullptr;
    return node;
    }

  /// Find if the list [start] contains [val].
  static bool contains(const Node *const *start, const Node *val)
    {
    const Node *const *op = start;
    while(*op)
      {
      if(*op == val)
        {
        return true;
        }
      op = Derived::getNextLocation(*op);
      }

    return false;
    }
  };

template <typename Node,
          Node* (Node::*Next)>
          class IntrusiveLinkedListMember
    : public IntrusiveLinkedListBase<Node, IntrusiveLinkedListMember<Node, Next>>
  {
public:
  static Node **getNextLocation(Node *prev)
    {
    return &(prev->*Next);
    }
  static const Node *const *getNextLocation(const Node *prev)
    {
    return &(prev->*Next);
    }
  };
}
