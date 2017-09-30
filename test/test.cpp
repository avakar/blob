#include <avakar/blob.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("blobs", "")
{
	avakar::blob b;
	REQUIRE(b.empty());
}

TEST_CASE("from string literal", "")
{
	avakar::blob b = "test";
	REQUIRE(!b.empty());
	REQUIRE(b == "test");
}

TEST_CASE("rope", "")
{
	avakar::rope r = "test";

	r = r | "ahoj" | "cau";
	REQUIRE(r.str() == "testahojcau");
}
