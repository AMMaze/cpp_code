#include "../shop.h"
#include "../product.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <iostream>
#include <string>

class A : public IProduct {
public:
    A(double p): IProduct(p, "A") {};
};

TEST(AttachTest, AddInactiveItem) {
    std::shared_ptr<IShop> shopPtr(new IShop("SHOP"));
    std::shared_ptr<IShop> shopPtr_test(new IShop("SHOP"));
    A item(10);
    testing::internal::CaptureStdout();
    item.Attach(shopPtr);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "");
    testing::internal::CaptureStdout();
    shopPtr->ListItems();
    std::string list1 = testing::internal::GetCapturedStdout();
    testing::internal::CaptureStdout();
    shopPtr_test->ListItems();
    std::string list2 = testing::internal::GetCapturedStdout();
    ASSERT_EQ(list1, list2);
};

TEST(AttachTest, AddActiveItem) {
    std::shared_ptr<IShop> shopPtr(new IShop("SHOP"));
    A item(10);
    item.StartSales();
    testing::internal::CaptureStdout();
    item.Attach(shopPtr);
    std::string output = testing::internal::GetCapturedStdout();
    testing::internal::CaptureStdout();
    shopPtr->ListItems();
    std::string list1 = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "In shop \'SHOP\'\nProduct A is now on sale for 10 !\n\n");
    ASSERT_EQ(list1, "#In shop \'SHOP\'\nItems on sale:\nA for 10\n\n");
};

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};
