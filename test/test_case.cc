#include <iostream>
#include <gtest/gtest.h>
#include "stbwrap.h"
using namespace std;

TEST(UT, load)
{
    stbwrap::frame *f = stbwrap::frame_alloc();
    EXPECT_NE(f, nullptr);

    int ret = stbwrap::image_read("../test/res/xiyang.jpeg", f);
    EXPECT_EQ(ret, 0);

    ret = stbwrap::image_write("./xiyang.bmp", f);
    EXPECT_EQ(ret, 0);

    stbwrap::frame_free(f);
}