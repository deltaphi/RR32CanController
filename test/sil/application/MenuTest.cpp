#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "RR32Can/RR32Can.h"

#include "MocksAndFixtures.h"

TEST_F(MainFixture, IdleScreen) {
  EXPECT_FALSE(masterControl.getDisplayModel().isCursorEnabled());
  EXPECT_STREQ(masterControl.getDisplayModel().getWritableBuffer(0),
               "No Engine");
  EXPECT_STREQ(masterControl.getDisplayModel().getWritableBuffer(1), "RR32Can");
}