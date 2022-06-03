// Integration tests driver.
// Soultatos Stefanos 2022

#include "gmock/gmock.h"
#include "gtest/gtest.h"

int main(int argc, char* argv[]) {
    try {
        ::testing::InitGoogleTest(&argc, argv);

        return RUN_ALL_TESTS();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';

        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "unexpected error\n";

        return EXIT_FAILURE;
    }
}