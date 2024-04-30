#include <iostream>

#include "PeecTestClient.hpp"
#include <PeecCommandParser.hpp>

#include <gtest/gtest.h>

int main(int argc, char** argv) 
{
	testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	getchar();

	return EXIT_SUCCESS;
}