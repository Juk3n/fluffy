#include <gtest/gtest.h>

#include <game.hpp>

TEST(GameTest, Creation) {
  Game sut{0, "sut", "sut/path"};

  EXPECT_EQ(0, sut.getId());
  EXPECT_EQ("sut", sut.getName());
  EXPECT_EQ("sut/path", sut.getPath());
}