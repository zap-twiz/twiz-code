#include "gtest/gtest.h"

#include "streams/iostream.h"

#include <fstream>
#include <sstream>

namespace {

class IoStreamTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(IoStreamTest, Construction) {
  std::ifstream bad_stream("");
  IoStream stream(&bad_stream);

  EXPECT_TRUE(stream.IsEOS());
  EXPECT_TRUE(stream.IsEOS());
}

TEST_F(IoStreamTest, BasicInput) {
  const std::string value("This is a test.");
  std::stringstream string_stream(value);

  std::string extracted_string;
  IoStream stream(&string_stream);
  while (!stream.IsEOS())
    extracted_string += stream.Get();

  EXPECT_TRUE(value == extracted_string);
}

}  // namespace
