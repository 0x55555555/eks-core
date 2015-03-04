#include <gtest/gtest.h>
#include "Containers/XIntrusiveLinkedList.h"

class Item
  {
public:
  Item *next = nullptr;
  };

using List = Eks::IntrusiveLinkedListMember<Item, &Item::next>;

TEST (ContainerTest, IntrusiveList) {
  Item *start = nullptr;
  Item a, b, c;
  
  EXPECT_FALSE(List::contains(&start, &a));
  EXPECT_FALSE(List::contains(&start, &b));
  EXPECT_FALSE(List::contains(&start, &c));
  
  size_t index = 0;
  EXPECT_EQ(nullptr, List::append(&start, &a, &index));
  EXPECT_TRUE(List::contains(&start, &a));
  EXPECT_FALSE(List::contains(&start, &b));
  EXPECT_FALSE(List::contains(&start, &c));
  
  EXPECT_EQ(nullptr, List::insert(&start, &b, 0));
  EXPECT_TRUE(List::contains(&start, &a));
  EXPECT_TRUE(List::contains(&start, &b));
  EXPECT_FALSE(List::contains(&start, &c));

  EXPECT_EQ(&a, List::append(&start, &c, &index));
  EXPECT_TRUE(List::contains(&start, &a));
  EXPECT_TRUE(List::contains(&start, &b));
  EXPECT_TRUE(List::contains(&start, &c));
  
  EXPECT_EQ(&b, List::remove(&start, &a));
  EXPECT_FALSE(List::contains(&start, &a));
  EXPECT_TRUE(List::contains(&start, &b));
  EXPECT_TRUE(List::contains(&start, &c));
  
  EXPECT_EQ(nullptr, List::remove(&start, &b));
  EXPECT_FALSE(List::contains(&start, &a));
  EXPECT_FALSE(List::contains(&start, &b));
  EXPECT_TRUE(List::contains(&start, &c));
  
  EXPECT_EQ(nullptr, List::remove(&start, &c));
  EXPECT_FALSE(List::contains(&start, &a));
  EXPECT_FALSE(List::contains(&start, &b));
  EXPECT_FALSE(List::contains(&start, &c));
}