#include "gtest/gtest.h"

#include "streams/buffered_stream.h"
#include "streams/iostream.h"

namespace {

class BufferedStreamTest : public ::testing::Test {
 protected:
  typedef BufferedStream<char> BufferedStreamType;

  BufferedStreamTest() : inner_stream_(NULL) {}

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

char const BufferedStreamTest::kTestString[] = "This is not a test.";

TEST_F(BufferedStreamTest, Construction) {
  BufferedStreamType buffered_stream(inner_stream_);

  EXPECT_FALSE(buffered_stream.IsEOS());
}

TEST_F(BufferedStreamTest, Unget) {
  BufferedStreamType buffered_stream(inner_stream_);

  buffered_stream.Unget('4');
  EXPECT_FALSE(buffered_stream.IsEOS());

  buffered_stream.Unget('2');
  EXPECT_FALSE(buffered_stream.IsEOS());

  EXPECT_TRUE('2' == buffered_stream.Get());
  EXPECT_FALSE(buffered_stream.IsEOS());

  EXPECT_TRUE('4' == buffered_stream.Get());

  std::string extracted_string;
  while (!buffered_stream.IsEOS())
    extracted_string += buffered_stream.Get();

  EXPECT_TRUE(kTestString == extracted_string);
  EXPECT_TRUE(buffered_stream.IsEOS());
}

TEST_F(BufferedStreamTest, Peek) {
  BufferedStreamType buffered_stream(inner_stream_);
  EXPECT_FALSE(buffered_stream.IsEOS());

  EXPECT_TRUE('T' == buffered_stream.Peek());
  EXPECT_TRUE('T' == buffered_stream.Peek());

  char const * string_iter = &kTestString[0];
  while (!buffered_stream.IsEOS()) {
    EXPECT_TRUE(*string_iter == buffered_stream.Peek());
    EXPECT_TRUE(*string_iter == buffered_stream.Get());
    ++string_iter;
  }
}

}  // namespace
