#include "gtest/gtest.h"

#include "parser/parser.h"
#include "parser/parse_node.h"
#include "parser/stream_set.h"

namespace {

class ParserTest : public ::testing::Test {
};

TEST_F(ParserTest, ParseImmediateValue) {
  StreamSet set;
  BuildStream("HIGH LOW TRUE FALSE", &set);
  ParserEntries entries;

  EXPECT_TRUE(entries.ParseImmediateValue(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseImmediateValue(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseImmediateValue(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseImmediateValue(*set.buffered_token_stream_));

  EXPECT_FALSE(entries.ParseImmediateValue(*set.buffered_token_stream_));
}

TEST_F(ParserTest, ParseIdentifierDefinition) {
  StreamSet set;
  BuildStream("some_identifier some_identifier[10] some_identifier[0xF] some_identifier[0b11]", &set);

  ParserEntries entries;
  EXPECT_TRUE(entries.ParseIdentifierDefinition(*set.buffered_token_stream_));
  {
    ParseNode* node = entries.node();
    EXPECT_EQ(ParseNode::SINGLE_PIN_DEFINITION, node->type());
    EXPECT_TRUE(node->terminals()[0].first.value() == "some_identifier");
  }

  EXPECT_TRUE(entries.ParseIdentifierDefinition(*set.buffered_token_stream_));
  {
    ParseNode* node = entries.node();
    EXPECT_EQ(ParseNode::IDENTIFIER_DEFINITION, node->type());
    EXPECT_TRUE(node->terminals()[0].first.value() == "some_identifier");
    EXPECT_TRUE(node->terminals()[2].first.value() == "10");
  }

  EXPECT_TRUE(entries.ParseIdentifierDefinition(*set.buffered_token_stream_));
  {
    ParseNode* node = entries.node();
    EXPECT_EQ(ParseNode::IDENTIFIER_DEFINITION, node->type());
    EXPECT_TRUE(node->terminals()[0].first.value() == "some_identifier");
    EXPECT_TRUE(node->terminals()[2].first.value() == "0xF");
  }

  EXPECT_TRUE(entries.ParseIdentifierDefinition(*set.buffered_token_stream_));
  {
    ParseNode* node = entries.node();
    EXPECT_EQ(ParseNode::IDENTIFIER_DEFINITION, node->type());
    EXPECT_TRUE(node->terminals()[0].first.value() == "some_identifier");
    EXPECT_TRUE(node->terminals()[2].first.value() == "0b11");
  }

  EXPECT_FALSE(entries.ParseIdentifierDefinition(*set.buffered_token_stream_));
}

TEST_F(ParserTest, ParseNumberOrRange) {
  StreamSet set;
  BuildStream("0xF 0..10", &set);

  ParserEntries entries;
  EXPECT_TRUE(entries.ParseNumberOrRange(*set.buffered_token_stream_));
  {
    ParseNode* node = entries.node();
    EXPECT_EQ(ParseNode::NUMBER, node->type());
    EXPECT_TRUE(node->terminals()[0].first.value() == "0xF");
  }

  EXPECT_TRUE(entries.ParseNumberOrRange(*set.buffered_token_stream_));
  {
    ParseNode* node = entries.node();
    EXPECT_EQ(ParseNode::NUMBER_RANGE, node->type());
    EXPECT_TRUE(node->terminals()[0].first.value() == "0");
    EXPECT_TRUE(node->terminals()[2].first.value() == "10");
  }

  EXPECT_FALSE(entries.ParseNumberOrRange(*set.buffered_token_stream_));
}

TEST_F(ParserTest, ParseNumberCollection) {
  StreamSet set;
  BuildStream("10 1,2 3..4,5..6", &set);

  ParserEntries entries;
  EXPECT_TRUE(entries.ParseNumberCollection(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseNumberCollection(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseNumberCollection(*set.buffered_token_stream_));

  // TODO:  Assert parse tree structure.
  EXPECT_FALSE(entries.ParseNumberCollection(*set.buffered_token_stream_));
}

TEST_F(ParserTest, ParseIdentifierReference) {
  StreamSet set;
  BuildStream("name name[10] name.pin name[10].pin name.pin[20] name[10].pin[10]", &set);

  ParserEntries entries;

  EXPECT_TRUE(entries.ParseIdentifierReference(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseIdentifierReference(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseIdentifierReference(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseIdentifierReference(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseIdentifierReference(*set.buffered_token_stream_));
  EXPECT_TRUE(entries.ParseIdentifierReference(*set.buffered_token_stream_));

  // TODO: Assert parse tree structure.

  EXPECT_FALSE(entries.ParseIdentifierReference(*set.buffered_token_stream_));
}

TEST_F(ParserTest, ParseChipIdentifierReference) {
}

}  // unnamed namespace
