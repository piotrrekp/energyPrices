#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "databaseWriter.h"

TEST(simple, create) {
	EXPECT_ANY_THROW(databaseWriter{"not_existing/path"});
}
