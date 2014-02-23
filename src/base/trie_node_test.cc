#include "gtest/gtest.h"
#include "base/trie_node.h"


namespace {

typedef TrieNode<char, bool> BasicNode;
typedef TrieNode<char, int> CountedNode;

class TrieNodeTest : public ::testing::Test {
};

TEST_F(TrieNodeTest, BasicConstruction) {
  BasicNode node(NULL);

  EXPECT_EQ(NULL, node.parent());
  EXPECT_FALSE(node.exists());
  EXPECT_EQ(NULL, node.value());

  node.ClearValue();
  EXPECT_EQ(NULL, node.value());

  EXPECT_TRUE(node.children()->empty());
}

TEST_F(TrieNodeTest, Value) {
  BasicNode node(NULL);

  EXPECT_EQ(NULL, node.value());

  node.set_value(true);
  EXPECT_FALSE(NULL == node.value());
  EXPECT_TRUE(node.value());
  EXPECT_TRUE(node.exists());

  node.ClearValue();
  EXPECT_EQ(NULL, node.value());
  EXPECT_FALSE(node.exists());
}

TEST_F(TrieNodeTest, Parent) {
  CountedNode root(NULL);
  CountedNode first_child(NULL);
  CountedNode second_child(NULL);

  root.AddChild('a', &first_child);
  EXPECT_EQ(&root, first_child.parent());
  EXPECT_FALSE(root.children()->empty());
  EXPECT_EQ(root.Next('a'), &first_child);

  root.AddChild('b', &second_child);
  EXPECT_EQ(&root, second_child.parent());
  EXPECT_EQ(root.Next('b'), &second_child);
}

TEST_F(TrieNodeTest, NextNode) {
  CountedNode root(NULL);
  CountedNode first_child(NULL);
  CountedNode second_child(NULL);

  root.AddChild('a', &first_child);
  root.AddChild('b', &second_child);

  EXPECT_EQ(&first_child, root.NextNode());
  EXPECT_EQ(&second_child, first_child.NextNode());
  EXPECT_EQ(NULL, second_child.NextNode());

  EXPECT_EQ(NULL, root.NextExistingNode());
}

TEST_F(TrieNodeTest, NextExistingNode) {
  CountedNode root(NULL);
  CountedNode first_child(NULL);
  CountedNode second_child(NULL);
  CountedNode third_child(NULL);

  root.AddChild('a', &first_child);
  root.AddChild('b', &second_child);
  root.AddChild('c', &third_child);

  second_child.set_value(42);
  EXPECT_EQ(&second_child, root.NextExistingNode());
  EXPECT_EQ(NULL, second_child.NextExistingNode());

  second_child.ClearValue();
  EXPECT_EQ(NULL, root.NextExistingNode());
}

TEST_F(TrieNodeTest, RemoveChild) {
  CountedNode root(NULL);
  CountedNode first_child(NULL);
  CountedNode second_child(NULL);
  CountedNode third_child(NULL);

  root.AddChild('a', &first_child);
  root.AddChild('b', &second_child);
  root.AddChild('c', &third_child);

  root.RemoveChild(&first_child);
  EXPECT_EQ(NULL, first_child.parent());
  EXPECT_EQ(NULL, root.Next('a'));
  EXPECT_EQ(&second_child, root.NextNode());
}

TEST_F(TrieNodeTest, Clone) {
  CountedNode root(NULL);
  CountedNode first_child(NULL);
  CountedNode second_child(NULL);
  CountedNode third_child(NULL);

  root.set_value(0);

  root.AddChild('a', &first_child);
  first_child.set_value(1);
  root.AddChild('b', &second_child);
  second_child.set_value(2);
  root.AddChild('c', &third_child);
  third_child.set_value(3);

  CountedNode* cloned = root.Clone();

  EXPECT_EQ(cloned->children()->size(), root.children()->size());

  CountedNode* current_node = &root;
  CountedNode* cloned_node = cloned;
  while (current_node) {
    EXPECT_EQ(*current_node->value(), *cloned_node->value());
    EXPECT_EQ(current_node->children()->size(),
              cloned_node->children()->size());
    current_node = current_node->NextNode();
    cloned_node = cloned_node->NextNode();
  }
  EXPECT_EQ(NULL, cloned_node);

  DeleteTrieDescendents(cloned);
}

}  // namespace
