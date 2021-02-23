#include "TestUtils.h"
#include "ProcessHandler.h"

TEST_CASE("Evaluate String Functions", "[Basics]") {
    CHECK(2 == std::stoi("2"));
    CHECK(3 == std::stoi(" 3"));
    CHECK(4 == std::stoi("4 "));

    CHECK(2 == std::stoi("2.0"));
    CHECK(3 == std::stoi(" 3  . 1"));
    CHECK(4 == std::stoi("4 .3   "));
}

bool IndexesAreEqual(ApplyIndexes_t * expected, ApplyIndexes_t * actual);


TEST_CASE("Parse no significant data -> Nor Errors", "[ApplyParse]") {
    ApplyIndexes_t const zeroIdx = {{0,0,0,0,0,}};
    ApplyIndexes_t idx = {{0,0,0,0,0,}};
    char input[128];

    SECTION("Zero-Pointer") {
        ParseApplyToString(nullptr, &idx);
    }
    SECTION("Parse empty string") {
        strcpy(input, "");
        ParseApplyToString(input, &idx);
    }
    SECTION("only zeros or negative numbers") {
        strcpy(input, "0,-1,-2,0");
        ParseApplyToString(input, &idx);
    }
    SECTION("only zeros or negative numbers with whitespaces") {
        strcpy(input, " 0,-1 ,  -2,0  ");
        ParseApplyToString(input, &idx);
    }
    SECTION("too big numbers") {
        strcpy(input, " 0,161,8000");
        ParseApplyToString(input, &idx);
    }
    int result = memcmp(&idx, &zeroIdx, sizeof(ApplyIndexes_t));
    REQUIRE(result == 0);
}

TEST_CASE("Wrong input in different format -> Exceptions", "[ApplyParse]")
{
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx;
    char input[128];

    SECTION("blank string") {
        strcpy(input, "       ");
        REQUIRE_THROWS(ParseApplyToString(input, &idx));
    }
    SECTION("wrong character") {
        strcpy(input, "a");
        REQUIRE_THROWS(ParseApplyToString(input, &idx));
    }
    SECTION("nested wrong character") {
        strcpy(input, "0,a,1");
        REQUIRE_THROWS(ParseApplyToString(input, &idx));
    }
}

TEST_CASE("Single Values -> Nor Error", "[ApplyParse]") {
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx = {{0,0,0,0,0,}};
    char input[128];

    SECTION("Single Value") {
        strcpy(input, "1");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp1 = {{0x00000001, 0, 0, 0, 0}};
        compIdx = &comp1;
    }
    SECTION("Single Channel Value") {
        strcpy(input, "1.1");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {{1, 0, 0, 0, 0}};
        compIdx = &comp2;
    }
    SECTION("invalid single Value") {
        strcpy(input, "-1");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {{0, 0, 0, 0, 0}};
        compIdx = &comp2;
    }
    int result = memcmp(&idx, compIdx, sizeof(ApplyIndexes_t));
    REQUIRE( IndexesAreEqual(compIdx, &idx) );
}

TEST_CASE("Simple Comma seperated Lists -> Nor Error", "[ApplyParse]") {
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx = {{0,0,0,0,0,}};
    char input[128];

    SECTION("simple list") {
        SECTION("simple list in tight definition") {
            strcpy(input, "1,2,3,32");
            ParseApplyToString(input, &idx);
        }
        SECTION("simple list with whitespaces") {
            strcpy(input, " 1,2 ,   3,32   ");
            ParseApplyToString(input, &idx);
        }
        static ApplyIndexes_t comp1 = {{0x80000007, 0, 0, 0, 0}};
        compIdx = &comp1;
    }
    SECTION("inverted order") {
        SECTION("inverted order in tight definition") {
            strcpy(input, "31,6,5,4");
            ParseApplyToString(input, &idx);
        }
        SECTION("inverted order with whitespaces") {
            strcpy(input, "31 ,  6, 5  ,   4 ");
            ParseApplyToString(input, &idx);
        }
        static ApplyIndexes_t comp2 = {{0x40000038, 0, 0, 0, 0}};
        compIdx = &comp2;
    }
    SECTION("First bits of channels") {
        strcpy(input, "1,33,65,97,129");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {{1, 1, 1, 1, 1}};
        compIdx = &comp2;
    }
    SECTION("Last bits of channels") {
        strcpy(input, "32,64,96,128,160");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {{0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}};
        compIdx = &comp2;
    }
    int result = memcmp(&idx, compIdx, sizeof(ApplyIndexes_t));
    REQUIRE( IndexesAreEqual(compIdx, &idx) );
}

TEST_CASE("Complex Comma seperated Lists -> Nor Error", "[ApplyParse]") {
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx = {{0,0,0,0,0,}};
    char input[128];

    SECTION("simple list") {
        SECTION("simple list in tight definition") {
            strcpy(input, "1.1,1.2,2.3");
            ParseApplyToString(input, &idx);
        }
        SECTION("simple list with whitespaces") {
            strcpy(input, " 1.1 ,1 . 2, 2 . 3 ");
            ParseApplyToString(input, &idx);
        }
        static ApplyIndexes_t comp1 = {{0x00000003, 0x00000004, 0, 0, 0}};
        compIdx = &comp1;
    }
    SECTION("First bits of channels") {
        strcpy(input, "1.1,2.1,3.1,4.1,5.1");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {{1, 1, 1, 1, 1}};
        compIdx = &comp2;
    }
    SECTION("Last bits of channels") {
        strcpy(input, "1.32,2.32,3.32,4.32,5.32");
        ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {{0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}};
        compIdx = &comp2;
    }
    int result = memcmp(&idx, compIdx, sizeof(ApplyIndexes_t));
    REQUIRE( IndexesAreEqual(compIdx, &idx) );
}

bool IndexesAreEqual(ApplyIndexes_t * expected, ApplyIndexes_t * actual)
{
    char buffer[128];
    for (size_t i = 0; i < ApplyToTargetChannels; i++) {
        if (expected->ApplyTo[i] != actual->ApplyTo[i]) {
            UNSCOPED_INFO("Error in element " << i);
            sprintf(buffer, "0x%08X != 0x%08X", expected->ApplyTo[i], actual->ApplyTo[i] );
            UNSCOPED_INFO("    " <<  buffer );
            return false;
        }
    }
    return true;
}