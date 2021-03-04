#include "TestUtils.h"
#include "RotatingIndex.h"

TEST_CASE("Instantiate object without any trouble", "[RotatinIndex]") {
    RotatingIndex dut(4);
    REQUIRE(true);
}

TEST_CASE("Instance starts with 0", "[RotatinIndex]") {
    RotatingIndex dut(4);
    REQUIRE( dut.GetIndex() == 0 );
}

TEST_CASE("Get with inkrement increments internal index for next get", "[RotatinIndex]") {
    RotatingIndex dut(4);
    REQUIRE( dut.GetIndexAndInkrement() == 0 );
    REQUIRE( dut.GetIndex() == 1 );
}

TEST_CASE("Multiple Get doesn't change internal index ", "[RotatinIndex]") {
    RotatingIndex dut(4);
    CHECK( dut.GetIndex() == 0 );
    CHECK( dut.GetIndex() == 0 );

    REQUIRE( dut.GetIndexAndInkrement() == 0 );
    REQUIRE( dut.GetIndex() == 1 );
    REQUIRE( dut.GetIndex() == 1 );
}

TEST_CASE("Index rotates to start after last increment ", "[RotatinIndex]") {

    SECTION( "Works for arbitrary Limit" ) {
        RotatingIndex dut(4);
        CHECK( dut.GetIndexAndInkrement() == 0 );
        CHECK( dut.GetIndexAndInkrement() == 1 );
        CHECK( dut.GetIndexAndInkrement() == 2 );
        CHECK( dut.GetIndexAndInkrement() == 3 );
        CHECK( dut.GetIndexAndInkrement() == 0 );
    }
    SECTION( "Works for other arbitrary Limit" ) {
        RotatingIndex dut(6);
        CHECK( dut.GetIndexAndInkrement() == 0 );
        CHECK( dut.GetIndexAndInkrement() == 1 );
        CHECK( dut.GetIndexAndInkrement() == 2 );
        CHECK( dut.GetIndexAndInkrement() == 3 );
        CHECK( dut.GetIndexAndInkrement() == 4 );
        CHECK( dut.GetIndexAndInkrement() == 5 );
        CHECK( dut.GetIndexAndInkrement() == 0 );
    }
}

TEST_CASE("PeekRevolution switches to true if next increment will rotate index ", "[RotatinIndex]") {

    SECTION( "Works for arbitrary Limit" ) {
        RotatingIndex dut(4);
        CHECK( dut.PeekRevolution() == false );

        dut.GetIndexAndInkrement();
        CHECK( dut.PeekRevolution() == false );
        dut.GetIndexAndInkrement();
        CHECK( dut.PeekRevolution() == false );
        dut.GetIndexAndInkrement();
        CHECK( dut.PeekRevolution() == true );
    }
    SECTION( "Works for other arbitrary Limit" ) {
        RotatingIndex dut(2);
        CHECK( dut.PeekRevolution() == false );

        dut.GetIndexAndInkrement();
        dut.GetIndexAndInkrement();
        dut.GetIndexAndInkrement();
        dut.GetIndexAndInkrement();
        CHECK( dut.PeekRevolution() == false );
        dut.GetIndexAndInkrement();
        CHECK( dut.PeekRevolution() == true );
    }
}