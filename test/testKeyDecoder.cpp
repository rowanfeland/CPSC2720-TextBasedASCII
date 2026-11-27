#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "Key.hpp"
#include "KeyDecoder.hpp"

namespace {

std::vector<Key> feedAll(KeyDecoder& d, const std::string& bytes) {
  std::vector<Key> out;
  for (std::string::size_type i = 0; i < bytes.size(); ++i) {
    std::vector<Key> keys = d.feed(bytes[i]);
    out.insert(out.end(), keys.begin(), keys.end());
  }
  return out;
}

}  // namespace

TEST(KeyDecoderTest, Arrows) {
  KeyDecoder d;
  std::vector<Key> keys = feedAll(d, "\033[A\033[B\033[C\033[D");
  ASSERT_EQ(4u, keys.size());
  EXPECT_EQ(KeyType::Up, keys[0].type);
  EXPECT_EQ(KeyType::Down, keys[1].type);
  EXPECT_EQ(KeyType::Right, keys[2].type);
  EXPECT_EQ(KeyType::Left, keys[3].type);
}

TEST(KeyDecoderTest, SplitEscape) {
  KeyDecoder d;
  EXPECT_TRUE(d.feed('\033').empty());
  EXPECT_TRUE(d.feed('[').empty());
  std::vector<Key> keys = d.feed('A');
  ASSERT_EQ(1u, keys.size());
  EXPECT_EQ(KeyType::Up, keys[0].type);
}

TEST(KeyDecoderTest, ModifiedArrow) {
  KeyDecoder d;
  std::vector<Key> keys = feedAll(d, "\033[1;5C");
  ASSERT_EQ(1u, keys.size());
  EXPECT_EQ(KeyType::Right, keys[0].type);
}

TEST(KeyDecoderTest, PrintableChars) {
  KeyDecoder d;
  std::vector<Key> keys = feedAll(d, "Hi ");
  ASSERT_EQ(3u, keys.size());
  EXPECT_EQ(KeyType::Char, keys[0].type);
  EXPECT_EQ('H', keys[0].ch);
  EXPECT_EQ(' ', keys[2].ch);
}

TEST(KeyDecoderTest, BackspaceEnterEof) {
  KeyDecoder d;
  std::vector<Key> keys = feedAll(d, std::string("\x7f\x08\r\n\x04", 5));
  ASSERT_EQ(5u, keys.size());
  EXPECT_EQ(KeyType::Backspace, keys[0].type);
  EXPECT_EQ(KeyType::Backspace, keys[1].type);
  EXPECT_EQ(KeyType::Enter, keys[2].type);
  EXPECT_EQ(KeyType::Enter, keys[3].type);
  EXPECT_EQ(KeyType::Eof, keys[4].type);
}

TEST(KeyDecoderTest, UnknownSequenceIsNone) {
  KeyDecoder d;
  std::vector<Key> keys = feedAll(d, "\033[Z");
  ASSERT_EQ(1u, keys.size());
  EXPECT_EQ(KeyType::None, keys[0].type);
}

TEST(KeyDecoderTest, LoneEscapeThenChar) {
  KeyDecoder d;
  std::vector<Key> keys = feedAll(d, "\033x");
  ASSERT_EQ(2u, keys.size());
  EXPECT_EQ(KeyType::None, keys[0].type);
  EXPECT_EQ(KeyType::Char, keys[1].type);
  EXPECT_EQ('x', keys[1].ch);
}

TEST(KeyDecoderTest, FlushPendingEscape) {
  KeyDecoder d;
  d.feed('\033');
  EXPECT_TRUE(d.pending());
  std::vector<Key> keys = d.flush();
  ASSERT_EQ(1u, keys.size());
  EXPECT_EQ(KeyType::None, keys[0].type);
  EXPECT_FALSE(d.pending());
  EXPECT_TRUE(d.flush().empty());
}

TEST(KeyDecoderTest, IgnoresOtherControls) {
  KeyDecoder d;
  EXPECT_TRUE(d.feed('\x01').empty());
}
