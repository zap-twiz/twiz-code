
#include "gtest/gtest.h"

#include "streams/iostream.h"
#include "streams/nested_stream.h"

#include <sstream>

namespace {

class NestedStreamTest : public ::testing::Test {
 protected:
  typedef NestedStream<char> NestedStreamType;

  NestedStreamTest() : inner_stream_(NULL) {}

  virtual void SetUp() {
    buffer_ = kTestString;
    string_stream_.str(buffer_);
    inner_stream_.set_stream(&string_stream_);
  }

  static char const kTestString[];

  std::string buffer_;
  std::stringstream string_stream_;
  IoStream inner_stream_;
};

char const NestedStreamTest::kTestString[] = "This is a test.";

TEST_F(NestedStreamTest, Basic) {
  NestedStreamType nested(inner_stream_);
  EXPECT_FALSE(nested.IsEOS());
}

TEST_F(NestedStreamTest, BasicCompare) {
  NestedStreamType nested(inner_stream_);

  std::string::const_iterator string_iter(buffer_.begin()), end(buffer_.end());
  while (!nested.IsEOS()) {
    ASSERT_TRUE(!inner_stream_.IsEOS());

    EXPECT_TRUE(nested.Get() ==  *string_iter);
    ++string_iter;

    EXPECT_EQ(nested.IsEOS(), inner_stream_.IsEOS());
  }
  EXPECT_TRUE(string_iter == end);
}

}  // namespace
