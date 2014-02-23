
#include "gtest/gtest.h"

#include "streams/counter.h"
#include "streams/iostream.h"
#include "streams/stream_with_observer.h"


namespace {

typedef ObservedStream<char> ObserverStream;
typedef CountObserver<char> LineCountObserver;

class StreamWithObserverTest : public ::testing::Test {
 protected:
  StreamWithObserverTest() : inner_stream_(NULL) {}

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

char const StreamWithObserverTest::kTestString[] =
  "This is a test.1\n"
  "This is a test.2\n"
  "This is a test.3\n"
  "This is a test.4\n"
  "This is a test.5\n"
  "This is a test.6\n"
  "This is a test.7\n"
  "This is a test.8\n";

TEST_F(StreamWithObserverTest, Basic) {
  ObserverStream line_counter(inner_stream_);
  EXPECT_EQ(NULL, line_counter.observer());

  LineCountObserver counter('\n');
  line_counter.set_observer(&counter);

  EXPECT_EQ(&counter, line_counter.observer());
}

TEST_F(StreamWithObserverTest, Count) {
  ObserverStream line_counter(inner_stream_);
  LineCountObserver counter('\n');

  line_counter.set_observer(&counter);

  size_t line_count = 0;
  while (!line_counter.IsEOS()) {
    EXPECT_EQ(line_count, counter.GetCount());
    if ('\n' == line_counter.Get())
      ++line_count;
  }
}

}  // namespace