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

bool IndexesAreEqual(const ApplyIndexes_t * expected, const ApplyIndexes_t * actual);


TEST_CASE("Parse no significant data -> Nor Errors", "[ApplyParse]") {
    ApplyIndexes_t const zeroIdx = {0, {RgbChannel::None_Error,0,0}, {0,0,0,0,0,}};
    ApplyIndexes_t idx = {0, {RgbChannel::None_Error, 0,0}, {0,0,0,0,0,}};
    char input[128];

    uint32_t errors;
    SECTION("Zero-Pointer") {
        errors = ParseApplyToString(nullptr, &idx);
        CHECK( errors == 1 );
    }
    SECTION("Parse empty string") {
        strcpy(input, "");
        errors = ParseApplyToString(input, &idx);
        CHECK( errors == 0 );
    }
    SECTION("only zeros or negative numbers") {
        strcpy(input, "0,-1,-2,0");
        errors = ParseApplyToString(input, &idx);
        CHECK( errors == 4 );
    }
    SECTION("only zeros or negative numbers with whitespaces") {
        strcpy(input, " 0,-1 , -4, -2,0  ");
        errors = ParseApplyToString(input, &idx);
        CHECK( errors == 5 );
    }
    SECTION("too big numbers") {
        strcpy(input, " 0,161,8000");
        errors = ParseApplyToString(input, &idx);
        CHECK( errors == 3 );
    }

    CHECK( IndexesAreEqual(&zeroIdx, &idx) );
    CHECK( idx.Items == 0 );
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
    ApplyIndexes_t idx = {0, {RgbChannel::None_Error, 0,0}, {0,0,0,0,0,}};
    char input[128];
    uint32_t errors;
    SECTION("Single Value") {
        strcpy(input, "1");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp1 = {0, {RgbChannel::None_Error, 0,0}, {0x00000001, 0, 0, 0, 0}};
        compIdx = &comp1;
        CHECK( idx.Items == 1 );
    }
    SECTION("Single Channel Value") {
        strcpy(input, "1.1");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {1, 0, 0, 0, 0}};
        compIdx = &comp2;
        CHECK( idx.Items == 1 );
    }
    SECTION("invalid single Value") {
        strcpy(input, "-1");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0, 0, 0, 0, 0}};
        compIdx = &comp2;
        CHECK( errors == 1 );
    }

    CHECK( IndexesAreEqual(&idx, compIdx) );
}

TEST_CASE("Simple Comma seperated Lists -> Nor Error", "[ApplyParse]") {
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx = {0, {RgbChannel::None_Error, 0,0}, {0,0,0,0,0,}};
    char input[128];
    uint32_t errors;

    SECTION("simple list") {
        SECTION("simple list in tight definition") {
            strcpy(input, "1,2,3,32");
            errors = ParseApplyToString(input, &idx);
            CHECK( idx.Items == 4 );
        }
        SECTION("simple list with whitespaces") {
            strcpy(input, " 1,2 ,   3,32   ");
            errors = ParseApplyToString(input, &idx);
            CHECK( idx.Items == 4 );
        }
        static ApplyIndexes_t comp1 = {0, {RgbChannel::None_Error, 0,0}, {0x80000007, 0, 0, 0, 0}};
        compIdx = &comp1;
    }
    SECTION("inverted order") {
        SECTION("inverted order in tight definition") {
            strcpy(input, "31,6,5,4");
            errors = ParseApplyToString(input, &idx);
            CHECK( idx.Items == 4 );
        }
        SECTION("inverted order with whitespaces") {
            strcpy(input, "31 ,  6, 5  ,   4 ");
            errors = ParseApplyToString(input, &idx);
            CHECK( idx.Items == 4 );
        }
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0x40000038, 0, 0, 0, 0}};
        compIdx = &comp2;
    }
    SECTION("First bits of channels") {
        strcpy(input, "1,33,65,97,129");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {1, 1, 1, 1, 1}};
        compIdx = &comp2;
        CHECK( idx.Items == 5 );
    }
    SECTION("Last bits of channels") {
        strcpy(input, "32,64,96,128,160");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}};
        compIdx = &comp2;
        CHECK( idx.Items == 5 );
    }
    int result = memcmp(&idx, compIdx, sizeof(ApplyIndexes_t));
    REQUIRE( IndexesAreEqual(compIdx, &idx) );
    CHECK( errors == 0 );
}

TEST_CASE("Complex Comma seperated Lists -> Nor Error", "[ApplyParse]") {
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx = {0, {RgbChannel::None_Error, 0,0}, {0,0,0,0,0,}};
    char input[128];
    uint32_t errors;

    SECTION("simple list") {
        SECTION("simple list in tight definition") {
            strcpy(input, "1.1,1.2,2.3");
            errors = ParseApplyToString(input, &idx);
            CHECK( idx.Items == 3 );
        }
        SECTION("simple list with whitespaces") {
            strcpy(input, " 1.1 ,1 . 2, 2 . 3 ");
            errors = ParseApplyToString(input, &idx);
            CHECK( idx.Items == 3 );
        }
        static ApplyIndexes_t comp1 = {0, {RgbChannel::None_Error, 0,0}, {0x00000003, 0x00000004, 0, 0, 0}};
        compIdx = &comp1;
    }
    SECTION("First bits of channels") {
        strcpy(input, "1.1,2.1,3.1,4.1,5.1");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {1, 1, 1, 1, 1}};
        compIdx = &comp2;
        CHECK( idx.Items == 5 );
    }
    SECTION("Last bits of channels") {
        strcpy(input, "1.32,2.32,3.32,4.32,5.32");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp3 = {0, {RgbChannel::None_Error, 0,0}, {0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}};
        compIdx = &comp3;
        CHECK( idx.Items == 5 );
    }
    int result = memcmp(&idx, compIdx, sizeof(ApplyIndexes_t));
    REQUIRE( IndexesAreEqual(compIdx, &idx) );
    CHECK( errors == 0 );
}

TEST_CASE("Span-Values -> Nor Error", "[ApplyParse]") {
    ApplyIndexes_t *compIdx;
    ApplyIndexes_t idx = {0, {RgbChannel::None_Error, 0,0}, {0,0,0,0,0,}};
    char input[128];
    uint32_t errors;

    SECTION("All Values") {
        strcpy(input, "*");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp1 = {32*5, {RgbChannel::None_Error, 1,1}, {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};
        compIdx = &comp1;
        CHECK( idx.Items == 160 );
    }
    SECTION("Single Channel Values") {
        strcpy(input, "1, 3-6, 15, 24-16, 30-32");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0xE000403D, 0, 0, 0, 0}};
        compIdx = &comp2;
        CHECK( idx.Items == 9 );
        CHECK( errors == 1 );
    }
    SECTION("One Range definition with Single Channel Value") {
        strcpy(input, "3-6");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0x0000003C, 0, 0, 0, 0}};
        compIdx = &comp2;
        CHECK( idx.Items == 4 );
        CHECK( errors == 0 );
    }
    SECTION("Structured Channel Values") {
        strcpy(input, "1.1, 1.3-1.6, 1.15, 1.24-1.16, 1.30-32");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0xE000403D, 0, 0, 0, 0}};
        compIdx = &comp2;
        CHECK( idx.Items == 9 );
        CHECK( errors == 1 );
    }
    SECTION("One structured Range definition with Single Channel Value") {
        strcpy(input, "1.3-1.6");
        errors = ParseApplyToString(input, &idx);
        static ApplyIndexes_t comp2 = {0, {RgbChannel::None_Error, 0,0}, {0x0000003C, 0, 0, 0, 0}};
        compIdx = &comp2;
        CHECK( idx.Items == 4 );
        CHECK( errors == 0 );
    }

    CHECK( IndexesAreEqual(&idx, compIdx) );
}

bool IndexesAreEqual(const ApplyIndexes_t * expected, const ApplyIndexes_t * actual)
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