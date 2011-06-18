#include "gtest/gtest.h"
#include "lexer/trie.h"

namespace {

typedef TrieMap<char, std::string, std::string> StringTrie;

// The fixture for testing class TrieMap.
class TrieTest : public ::testing::Test {
 protected:
   TrieTest() {
     if (kNumTestStrings == 0) {
       while(kTestStrings[kNumTestStrings] != NULL)
         ++kNumTestStrings;
     }
   }

  void InsertBulk(StringTrie* trie) {
    for (int x = 0; x < kNumTestStrings; ++x) {
      trie->insert(kTestStrings[x], std::string(kTestStrings[x]));
    }
  }

  static char const * kTestStrings[];
  static int kNumTestStrings;
};

int TrieTest::kNumTestStrings = 0;
char const * TrieTest::kTestStrings[] = {
  "asafoetida",
  "bemedalled",
  "bimetallist",
  "bimetallistic",
  "calliper",
  "cancellable",
  "cancellous",
  "chiselling",
  "corbelling",
  "counsellee",
  "counsellorship",
  "crenellate",
  "crenellated",
  "crenellation",
  "crystalographical",
  "crystalographically",
  "cupellation",
  "cupeller",
  "disenthral",
  "disenthrals",
  "driveller",
  "enamellist",
  "frivolled",
  "frivoller",
  "frivolling",
  "gambolled",
  "gambolling",
  "gavelled",
  "gruelled",
  "hiccupped",
  "hiccupping",
  "hovelled",
  "judgemental",
  "kennelled",
  "kernelled",
  "labellable",
  "lapelled",
  "libellant",
  "libellee",
  "marshall",
  "marshalling",
  "marshalls",
  "medallist",
  "medallists",
  "metallization",
  "metal",
  "metallizations",
  "metallize",
  "metallized",
  "metallizes",
  "metallizing",
  "microcrystaline",
  "microcrystalinity",
  "miocrystaline",
  "mislabelled",
  "monometallism",
  "monometallist",
  "penciller",
  "perilled",
  "photolabelled",
  "photolabeller",
  "photolabelling",
  "precancellation",
  "prejudgement",
  "programmist",
  "programmistic",
  "programmistics",
  "programmists",
  "raveller",
  "ravelling",
  "semicrystaline",
  "snobsnivelling",
  "subtotalled",
  "subtotalling",
  "teazelling",
  "tendrilled",
  "trammelled",
  "trammelling",
  "trowelled",
  "trowelling",
  "unhouselled",
  "weevilled",
  "woollen",
  "woollenization",
  "woollenizations",
  "woollenize",
  "woollenizes",
  "woollens",
  NULL
};


// Tests that the Foo::Bar() method does Abc.
TEST_F(TrieTest, BasicConstruction) {
  StringTrie trie;
  EXPECT_EQ(0, trie.count());
  EXPECT_TRUE(trie.empty());

  trie.clear();
  EXPECT_EQ(0, trie.count());
  EXPECT_TRUE(trie.empty());
}

TEST_F(TrieTest, BasicInsert) {
  StringTrie trie;
  StringTrie::Iterator iterator(trie.find("foo"));
  EXPECT_TRUE(trie.end() == iterator);

  static const std::string kFooInserted = "foo_inserted";
  trie.insert("foo", kFooInserted);
  iterator = trie.find("foo");
  EXPECT_FALSE(trie.end() == iterator);
  EXPECT_TRUE(iterator.value() == kFooInserted);
  EXPECT_EQ(1, trie.count());
  EXPECT_FALSE(trie.empty());

  static const std::string kFooInsertedAgain = "foo_inserted_again";
  trie.insert("foo", kFooInsertedAgain);
  EXPECT_TRUE(iterator.value() == kFooInsertedAgain);
  EXPECT_EQ(1, trie.count());
  EXPECT_FALSE(trie.empty());
}

TEST_F(TrieTest, BasicErase) {
  StringTrie trie;
  trie.insert("foo", "foo_inserted");

  StringTrie::Iterator iterator(trie.find("foo"));
  trie.erase(iterator);

  EXPECT_EQ(0, trie.count());
  EXPECT_TRUE(trie.empty());

  iterator = trie.find("foo");
  EXPECT_TRUE(trie.end() == iterator);
}

TEST_F(TrieTest, Insert) {
  StringTrie trie;
  InsertBulk(&trie);

  EXPECT_FALSE(trie.empty());
  EXPECT_EQ(kNumTestStrings, trie.count());

  trie.clear();
  EXPECT_TRUE(trie.empty());
  EXPECT_EQ(0, trie.count());
}

TEST_F(TrieTest, Find) {
  StringTrie trie;
  InsertBulk(&trie);

  EXPECT_EQ(kNumTestStrings, trie.count());

  for (int x = 0; x < kNumTestStrings; ++x) {
    StringTrie::Iterator iter(trie.find(kTestStrings[x]));
    EXPECT_TRUE(trie.end() != iter);
  }
}

TEST_F(TrieTest, Erase) {
  StringTrie trie;
  InsertBulk(&trie);

  for (int x = 0; x < kNumTestStrings; ++x) {
    StringTrie::Iterator iter(trie.find(kTestStrings[x]));
    trie.erase(iter);

    EXPECT_EQ(kNumTestStrings - x - 1, trie.count());
    iter = trie.find(kTestStrings[x]);
    EXPECT_TRUE(trie.end() == iter);
  }

  EXPECT_TRUE(trie.empty());
}

TEST_F(TrieTest, Iterator) {
  StringTrie trie;
  InsertBulk(&trie);

  int loop_count = 0;
  StringTrie::Iterator iter(trie.begin()), end(trie.end());
  for (; iter != end; ++iter) {
    ++loop_count;
  }

  EXPECT_EQ(trie.count(), loop_count);
}

TEST_F(TrieTest, IteratorValue) {
  StringTrie trie;
  InsertBulk(&trie);

  int loop_count = 0;
  StringTrie::Iterator iter(trie.begin()), end(trie.end());
  for (; iter != end; ++iter) {
    std::string value = *iter;

    StringTrie::Iterator same_iter(trie.find(value));
    EXPECT_TRUE(same_iter == iter);

    bool found = false;
    for (int x = 0; x < kNumTestStrings; ++x) {
      if (value == kTestStrings[x]) {
        found = true;
        break;
      }
    }
    EXPECT_TRUE(found) << "Value missing: " << value;
  }
}

TEST_F(TrieTest, CopyConstruct) {
  StringTrie trie1, trie2;
  InsertBulk(&trie1);

  trie1 = trie1;
  EXPECT_FALSE(trie1.empty());

  trie2 = trie1;
  StringTrie trie3(trie2);

  EXPECT_EQ(trie1.count(), trie2.count());
  EXPECT_EQ(trie1.empty(), trie2.empty());

  EXPECT_EQ(trie1.count(), trie3.count());
  EXPECT_EQ(trie1.empty(), trie3.empty());

  StringTrie::Iterator iter1(trie1.begin()), iter2(trie2.begin()),
      iter3(trie3.begin()), end(trie1.end());
  for (; iter1 != end; ++iter1, ++iter2, ++iter3) {
    EXPECT_TRUE(*iter1 == *iter2);
    EXPECT_TRUE(*iter1 == *iter3);
  }

  trie2.clear();
  EXPECT_TRUE(trie2.empty());
  EXPECT_FALSE(trie1.empty());
}

TEST_F(TrieTest, CommonPrefix) {
  StringTrie trie;
  trie.insert("ABABA", "ABABA");
  trie.insert("ABA", "ABA");
  trie.insert("AB", "AB");
  trie.insert("", "");

  StringTrie::Iterator iterator(trie.find("ABABA"));
  EXPECT_TRUE(iterator != trie.end());

  iterator = trie.find("ABA");
  EXPECT_TRUE(iterator != trie.end());

  iterator = trie.find("AB");
  EXPECT_TRUE(iterator != trie.end());

  size_t offset = 0;
  iterator = trie.find("ABAB", &offset);
  EXPECT_TRUE(iterator != trie.end());
  EXPECT_TRUE(*iterator == "ABA");
  EXPECT_EQ(3, offset);

  trie.erase(iterator);
  iterator = trie.find("ABABA");
  EXPECT_TRUE(iterator != trie.end());

  iterator = trie.find("X", &offset);
  EXPECT_TRUE(trie.end() != iterator);  // We matched the empty string
  EXPECT_EQ(0, offset);

  iterator = trie.find("");
  EXPECT_TRUE(trie.end() != iterator);
  EXPECT_TRUE(*iterator == "");

  trie.erase(iterator);
  iterator = trie.find("X", &offset);
  EXPECT_TRUE(trie.end() == iterator);
  EXPECT_EQ(-1, offset);
}

TEST_F(TrieTest, IteratorNext) {
  StringTrie trie;
  InsertBulk(&trie);

  // Validate that all of the test strings can be reached via the
  // Iterator::next method.
  for (int x = 0; x < kNumTestStrings; ++x) {
    StringTrie::Iterator position(trie.root());

    std::string value(kTestStrings[x]);
    std::string::iterator siter(value.begin()), send(value.end());
    for (; siter != send; ++siter) {
      StringTrie::Iterator next = position.next(*siter);
      ASSERT_TRUE(trie.end() != next);
      position = next;
    }
    EXPECT_TRUE(position.exists());
  }
}

}  // namespace
