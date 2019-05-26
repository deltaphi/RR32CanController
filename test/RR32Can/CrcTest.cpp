#include "gtest/gtest.h"

#include "RR32Can/util/Crc.h"

namespace RR32Can {

TEST(Crc, empty) {
  RR32Can::Crc crc;
  crc.reset();
  EXPECT_TRUE(crc.isCrcValid());
  EXPECT_EQ(0xFFFF, crc.getCrc());
}

TEST(Crc, loadReference) {
  RR32Can::Crc crc;
  crc.reset();
  crc.loadReference(5);
  EXPECT_FALSE(crc.isCrcValid());
  EXPECT_EQ(0xFFFF, crc.getCrc());

  crc.reset();
  EXPECT_TRUE(crc.isCrcValid());
  EXPECT_EQ(0xFFFF, crc.getCrc());
}

TEST(Crc, reset) {
  RR32Can::Crc crc;
  crc.reset();
  crc.updateCrc(5);
  EXPECT_FALSE(crc.isCrcValid());
  EXPECT_NE(0xFFFF, crc.getCrc());

  crc.reset();
  EXPECT_TRUE(crc.isCrcValid());
  EXPECT_EQ(0xFFFF, crc.getCrc());
}

TEST(Crc, data1) {
  uint16_t streamCRC = 0xDEC2;
  uint16_t streamLength = 88;

  uint8_t streamData[11][8] = {
      {0x5B, 0x6C, 0x6F, 0x6B, 0x5D, 0x0A, 0x20, 0x20},
      {0x20, 0x2E, 0x6E, 0x61, 0x6D, 0x65, 0x3D, 0x39},
      {0x38, 0x20, 0x31, 0x30, 0x30, 0x31, 0x0A, 0x20},
      {0x5B, 0x6C, 0x6F, 0x6B, 0x5D, 0x0A, 0x20, 0x20},
      {0x20, 0x2E, 0x6E, 0x61, 0x6D, 0x65, 0x3D, 0x42},
      {0x52, 0x20, 0x38, 0x36, 0x20, 0x35, 0x38, 0x32},
      {0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
      {0x5B, 0x6E, 0x75, 0x6D, 0x6C, 0x6F, 0x6B, 0x73},
      {0x5D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
      {0x20, 0x2E, 0x77, 0x65, 0x72, 0x74, 0x3D, 0x31},
      {0x32, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}};

  RR32Can::Crc crc;
  crc.reset();
  crc.loadReference(0xDEC2);

  for (uint8_t i = 0; i < 11; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      crc.updateCrc(streamData[i][j]);

      EXPECT_TRUE(crc.isCrcValid() == (i == 10 && j == 7));
    }
  }

  EXPECT_TRUE(crc.isCrcValid());
}

} /* namespace RR32Can */