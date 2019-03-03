#include "gtest/gtest.h"

#include <memory>

#include "RR32Can/util/BufferManager.h"

class BufferManagerFixture : public ::testing::Test {
 protected:
  void SetUp() {
    memset(buffer, 0, bufferSize);
    mgr = std::make_unique<RR32Can::BufferManager>(buffer, bufferSize);
  }

  constexpr static const uint8_t bufferSize = 16;
  char buffer[bufferSize];

  std::unique_ptr<RR32Can::BufferManager> mgr;
};

class BufferManagerFixtureWithData : public BufferManagerFixture {
 protected:
  void SetUp() {
    BufferManagerFixture::SetUp();
    mgr->set("0123456789");
    EXPECT_EQ(10, mgr->length());

    mgr2 = std::make_unique<RR32Can::BufferManager>(buffer2, bufferSize);
    mgr2->set("abcdefghij");
    EXPECT_EQ(10, mgr2->length());
  }

  char buffer2[bufferSize];

  std::unique_ptr<RR32Can::BufferManager> mgr2;
};

TEST_F(BufferManagerFixture, initialized) {
  EXPECT_TRUE(mgr->empty());
  EXPECT_FALSE(mgr->full());
  EXPECT_EQ(bufferSize, mgr->capacity());
  EXPECT_EQ(bufferSize, mgr->capacity_remaining());
  EXPECT_EQ(0, mgr->length());
  EXPECT_EQ(buffer, mgr->data());
}

TEST_F(BufferManagerFixture, set_and_get) {
  const char* testString = "Testdaten";
  const std::size_t stringLength = strlen(testString);
  ASSERT_EQ(9, stringLength);

  mgr->set(testString);

  EXPECT_NE(testString, mgr->data());
  EXPECT_STREQ(testString, mgr->data());

  EXPECT_EQ(stringLength, mgr->length());
  EXPECT_EQ(7, mgr->capacity_remaining());
  EXPECT_EQ(bufferSize, mgr->capacity());
  EXPECT_FALSE(mgr->empty());
  EXPECT_FALSE(mgr->full());
}

TEST_F(BufferManagerFixture, set_and_get_15) {
  const char* testString = "TestdatenTestda";
  const std::size_t stringLength = strlen(testString);
  ASSERT_EQ(15, stringLength);

  mgr->set(testString);

  EXPECT_NE(testString, mgr->data());
  EXPECT_STREQ(testString, mgr->data());

  EXPECT_EQ(stringLength, mgr->length());
  EXPECT_EQ(1, mgr->capacity_remaining());
  EXPECT_EQ(bufferSize, mgr->capacity());
  EXPECT_FALSE(mgr->empty());
  EXPECT_FALSE(mgr->full());
}

TEST_F(BufferManagerFixture, set_and_get_16) {
  const char* testString = "TestdatenTestdat";
  const std::size_t stringLength = strlen(testString);
  ASSERT_EQ(16, stringLength);

  mgr->set(testString);

  EXPECT_NE(testString, mgr->data());
  EXPECT_STRNE(testString, mgr->data());
  EXPECT_STREQ("TestdatenTestda", mgr->data());

  EXPECT_EQ(15, mgr->length());
  EXPECT_EQ(1, mgr->capacity_remaining());
  EXPECT_EQ(bufferSize, mgr->capacity());
  EXPECT_FALSE(mgr->empty());
  EXPECT_FALSE(mgr->full());
}

TEST_F(BufferManagerFixture, set_and_get_17) {
  const char* testString = "TestdatenTestdate";
  const std::size_t stringLength = strlen(testString);
  ASSERT_EQ(17, stringLength);

  mgr->set(testString);

  EXPECT_NE(testString, mgr->data());
  EXPECT_STRNE(testString, mgr->data());
  EXPECT_STREQ("TestdatenTestda", mgr->data());

  EXPECT_EQ(15, mgr->length());
  EXPECT_EQ(1, mgr->capacity_remaining());
  EXPECT_EQ(bufferSize, mgr->capacity());
  EXPECT_FALSE(mgr->empty());
  EXPECT_FALSE(mgr->full());
}

TEST_F(BufferManagerFixtureWithData, pop_front) {
  mgr->pop_front(3);
  EXPECT_EQ(7, mgr->length());
  EXPECT_STREQ("3456789", mgr->data());
}

TEST_F(BufferManagerFixtureWithData, pop_front_all) {
  mgr->pop_front(10);
  EXPECT_EQ(0, mgr->length());
  EXPECT_STREQ("", mgr->data());
}

TEST_F(BufferManagerFixtureWithData, pop_front_over_content) {
  mgr->pop_front(11);
  EXPECT_EQ(0, mgr->length());
  EXPECT_STREQ("", mgr->data());
}

TEST_F(BufferManagerFixtureWithData, pop_front_over_limit) {
  mgr->pop_front(20);
  EXPECT_EQ(0, mgr->length());
  EXPECT_STREQ("", mgr->data());
}

TEST_F(BufferManagerFixtureWithData, index_operator) {
  EXPECT_EQ('0', (*mgr)[0]);
  EXPECT_EQ('1', (*mgr)[1]);
  EXPECT_EQ('2', (*mgr)[2]);
  EXPECT_EQ('3', (*mgr)[3]);
  EXPECT_EQ('4', (*mgr)[4]);
  EXPECT_EQ('5', (*mgr)[5]);
  EXPECT_EQ('6', (*mgr)[6]);
  EXPECT_EQ('7', (*mgr)[7]);
  EXPECT_EQ('8', (*mgr)[8]);
  EXPECT_EQ('9', (*mgr)[9]);
}

TEST_F(BufferManagerFixtureWithData, at_read) {
  EXPECT_EQ('0', mgr->at(0));
  EXPECT_EQ('1', mgr->at(1));
  EXPECT_EQ('2', mgr->at(2));
  EXPECT_EQ('3', mgr->at(3));
  EXPECT_EQ('4', mgr->at(4));
  EXPECT_EQ('5', mgr->at(5));
  EXPECT_EQ('6', mgr->at(6));
  EXPECT_EQ('7', mgr->at(7));
  EXPECT_EQ('8', mgr->at(8));
  EXPECT_EQ('9', mgr->at(9));
}

TEST_F(BufferManagerFixtureWithData, at_write) {
  mgr->at(0) = 'a';
  EXPECT_STREQ("a123456789", mgr->data());
  EXPECT_EQ(10, mgr->length());

  mgr->at(5) = 'f';
  EXPECT_STREQ("a1234f6789", mgr->data());
  EXPECT_EQ(10, mgr->length());

  mgr->at(9) = 'j';
  EXPECT_STREQ("a1234f678j", mgr->data());
  EXPECT_EQ(10, mgr->length());

  mgr->at(10) = 'k';
  EXPECT_STREQ("a1234f678k", mgr->data());
  EXPECT_EQ(10, mgr->length());
}

TEST_F(BufferManagerFixtureWithData, push_back) {
  EXPECT_EQ(6, mgr->push_back(*mgr2));
  EXPECT_TRUE(mgr->full());
  EXPECT_EQ(16, mgr->length());

  EXPECT_EQ(0, memcmp("0123456789abcdefg", mgr->data(), 16));

  EXPECT_EQ(10, mgr2->length());
  EXPECT_STREQ("abcdefghij", mgr2->data());
}

TEST_F(BufferManagerFixtureWithData, push_back_self) {
  EXPECT_EQ(6, mgr->push_back(*mgr));
  EXPECT_TRUE(mgr->full());
  EXPECT_EQ(16, mgr->length());

  EXPECT_EQ(0, memcmp("01234567890123456", mgr->data(), 16));
}

TEST_F(BufferManagerFixtureWithData, move_back) {
  mgr->move_back(*mgr2);
  EXPECT_TRUE(mgr->full());
  EXPECT_EQ(16, mgr->length());

  EXPECT_EQ(0, memcmp("0123456789abcdef", mgr->data(), 16));

  EXPECT_EQ(4, mgr2->length());
  EXPECT_STREQ("ghij", mgr2->data());
}

TEST_F(BufferManagerFixtureWithData, erase) {
  mgr->erase();
  EXPECT_TRUE(mgr->empty());
  EXPECT_EQ(0, mgr->length());

  EXPECT_EQ(0, memcmp("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", mgr->data(), 16));
  EXPECT_EQ(buffer, mgr->data());
}

TEST_F(BufferManagerFixtureWithData, find_first_of) {
  EXPECT_EQ(0, mgr->findFirstOf('0'));
  EXPECT_EQ(1, mgr->findFirstOf('1'));
  EXPECT_EQ(2, mgr->findFirstOf('2'));
  EXPECT_EQ(3, mgr->findFirstOf('3'));
  EXPECT_EQ(4, mgr->findFirstOf('4'));
  EXPECT_EQ(5, mgr->findFirstOf('5'));
  EXPECT_EQ(6, mgr->findFirstOf('6'));
  EXPECT_EQ(7, mgr->findFirstOf('7'));
  EXPECT_EQ(8, mgr->findFirstOf('8'));
  EXPECT_EQ(9, mgr->findFirstOf('9'));
}

TEST_F(BufferManagerFixtureWithData, find_first_of_zero) {
  EXPECT_EQ(0, mgr->findFirstOf('0', 0));
  EXPECT_EQ(1, mgr->findFirstOf('1', 0));
  EXPECT_EQ(2, mgr->findFirstOf('2', 0));
  EXPECT_EQ(3, mgr->findFirstOf('3', 0));
  EXPECT_EQ(4, mgr->findFirstOf('4', 0));
  EXPECT_EQ(5, mgr->findFirstOf('5', 0));
  EXPECT_EQ(6, mgr->findFirstOf('6', 0));
  EXPECT_EQ(7, mgr->findFirstOf('7', 0));
  EXPECT_EQ(8, mgr->findFirstOf('8', 0));
  EXPECT_EQ(9, mgr->findFirstOf('9', 0));
}

TEST_F(BufferManagerFixtureWithData, find_first_of_middle) {
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('0', 5));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('1', 5));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('2', 5));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('3', 5));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('4', 5));
  EXPECT_EQ(5, mgr->findFirstOf('5', 5));
  EXPECT_EQ(6, mgr->findFirstOf('6', 5));
  EXPECT_EQ(7, mgr->findFirstOf('7', 5));
  EXPECT_EQ(8, mgr->findFirstOf('8', 5));
  EXPECT_EQ(9, mgr->findFirstOf('9', 5));
}

TEST_F(BufferManagerFixtureWithData, find_first_of_end) {
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('0', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('1', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('2', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('3', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('4', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('5', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('6', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('7', 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('8', 9));
  EXPECT_EQ(9, mgr->findFirstOf('9', 9));
}

TEST_F(BufferManagerFixtureWithData, find_first_of_pasttheend) {
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('0', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('1', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('2', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('3', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('4', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('5', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('6', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('7', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('8', 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf('9', 10));
}

TEST_F(BufferManagerFixtureWithData, multi_find_first_of) {
  EXPECT_EQ(0, mgr->findFirstOf("0123"));
  EXPECT_EQ(1, mgr->findFirstOf("123"));
  EXPECT_EQ(0, mgr->findFirstOf("3210"));

  EXPECT_EQ(4, mgr->findFirstOf("4"));

  EXPECT_EQ(4, mgr->findFirstOf("4567"));
  EXPECT_EQ(5, mgr->findFirstOf("567"));
  EXPECT_EQ(4, mgr->findFirstOf("7645"));
}

TEST_F(BufferManagerFixtureWithData, multi_find_first_of_zero) {
  EXPECT_EQ(0, mgr->findFirstOf("0123", 0));
  EXPECT_EQ(1, mgr->findFirstOf("123", 0));
  EXPECT_EQ(0, mgr->findFirstOf("3210", 0));

  EXPECT_EQ(4, mgr->findFirstOf("4", 0));

  EXPECT_EQ(4, mgr->findFirstOf("4567", 0));
  EXPECT_EQ(5, mgr->findFirstOf("567", 0));
  EXPECT_EQ(4, mgr->findFirstOf("7645", 0));
}

TEST_F(BufferManagerFixtureWithData, multi_find_first_of_middle) {
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("0123", 4));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("123", 4));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("3210", 4));

  EXPECT_EQ(4, mgr->findFirstOf("4", 4));
}

TEST_F(BufferManagerFixtureWithData, multi_find_first_of_end) {
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("0", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("1", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("2", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("3", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("4", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("5", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("6", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("7", 9));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("8", 9));
  EXPECT_EQ(9, mgr->findFirstOf("9", 9));
}

TEST_F(BufferManagerFixtureWithData, multi_find_first_of_pasttheend) {
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("0", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("1", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("2", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("3", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("4", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("5", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("6", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("7", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("8", 10));
  EXPECT_EQ(RR32Can::BufferManager::npos, mgr->findFirstOf("9", 10));
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_0) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(0);
  EXPECT_EQ(subMgr.data(), mgr->data());
  EXPECT_EQ(subMgr.length(), mgr->length());
  EXPECT_EQ(subMgr.capacity(), mgr->capacity());
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_1) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(1);
  EXPECT_EQ(subMgr.data(), mgr->data() + 1);
  EXPECT_EQ(subMgr.length(), 9);
  EXPECT_EQ(subMgr.capacity(), 15);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_5) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(5);
  EXPECT_EQ(subMgr.data(), mgr->data() + 5);
  EXPECT_EQ(subMgr.length(), 5);
  EXPECT_EQ(subMgr.capacity(), 11);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_9) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(9);
  EXPECT_EQ(subMgr.data(), mgr->data() + 9);
  EXPECT_EQ(subMgr.length(), 1);
  EXPECT_EQ(subMgr.capacity(), 7);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_10) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(10);
  EXPECT_EQ(subMgr.data(), mgr->data() + 10);
  EXPECT_EQ(subMgr.length(), 0);
  EXPECT_EQ(subMgr.capacity(), 6);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_11) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(11);
  EXPECT_EQ(subMgr.data(), mgr->data() + 11);
  EXPECT_EQ(subMgr.length(), 0);
  EXPECT_EQ(subMgr.capacity(), 5);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_15) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(15);
  EXPECT_EQ(subMgr.data(), mgr->data() + 15);
  EXPECT_EQ(subMgr.length(), 0);
  EXPECT_EQ(subMgr.capacity(), 1);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_16) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(16);
  EXPECT_EQ(subMgr.data(), mgr->data() + 16);
  EXPECT_EQ(subMgr.length(), 0);
  EXPECT_EQ(subMgr.capacity(), 0);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_17) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(17);
  EXPECT_EQ(subMgr.data(), mgr->data() + 16);
  EXPECT_EQ(subMgr.length(), 0);
  EXPECT_EQ(subMgr.capacity(), 0);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_5_len_3) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(5, 8);
  EXPECT_EQ(subMgr.data(), mgr->data() + 5);
  EXPECT_EQ(subMgr.length(), 3);
  EXPECT_EQ(subMgr.capacity(), 11);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_5_len_5) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(5, 10);
  EXPECT_EQ(subMgr.data(), mgr->data() + 5);
  EXPECT_EQ(subMgr.length(), 5);
  EXPECT_EQ(subMgr.capacity(), 11);
}

TEST_F(BufferManagerFixtureWithData, subBuffer_offset_5_len_negative) {
  RR32Can::BufferManager subMgr = mgr->subBufferManager(5, 3);
  EXPECT_EQ(subMgr.data(), mgr->data() + 5);
  EXPECT_EQ(subMgr.length(), 0);
  EXPECT_EQ(subMgr.capacity(), 11);
}