/*
 * main.cpp
 * Copyright (C) 2014 wangyongliang <wangyongliang.wyl@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "../3rdparty/include/gtest/gtest.h"
#include "../3rdparty/include/gmock/gmock.h"



int main(int argc, char **argv) {
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}


