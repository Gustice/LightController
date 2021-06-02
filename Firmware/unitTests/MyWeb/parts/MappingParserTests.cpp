#include "TestUtils.h"
#include "MappingParser.h"


TEST_CASE( "Split Path to Type and index", "[Parser]" ) {
    char input[128];
    SECTION( "Simple Path" ) {
        sprintf(input, "SomeType/123");
        char * type;
        char * index;
        REQUIRE( ESP_OK == RgbTargetPathSplitter(input, &type, &index));

        INFO(type)
        INFO(index)
        CHECK( 0 == strcmp("SomeType", type));
        CHECK( 0 == strcmp("123", index));
    }
    SECTION( "Wrong Path" ) {
        sprintf(input, "SomeType123");
        char * type;
        char * index;
        REQUIRE( ESP_FAIL == RgbTargetPathSplitter(input, &type, &index));
    }
}
