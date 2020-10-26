#include "TestUtils.h"
#include "Encoder.h"

TEST_CASE("Construction of Encoder object as precondition", "[encoder]") 
{
    InputPort pA;
    InputPort pB;

    QuadDecoder dut(pA, pB);
    REQUIRE (true);
}

TEST_CASE("Encoder starts always at zero after construction", "[encoder]") 
{
    InputPort pA;
    InputPort pB;

    uint16_t value = 0xFFFF;
    SECTION( "B-A = 0-0" ) {
        QuadDecoder dut(pA, pB);
        value = dut.EvalStepSync();
    }
    SECTION( "B-A = 0-1" ) {
        pA.Value = 1;
        QuadDecoder dut(pA, pB);
        value = dut.EvalStepSync();
    }
    SECTION( "B-A = 1-0" ) {
        pB.Value = 1;
        QuadDecoder dut(pA, pB);
        value = dut.EvalStepSync();
    }
    SECTION( "B-A = 1-1" ) {
        pA.Value = pB.Value = 1;
        QuadDecoder dut(pA, pB);
        value = dut.EvalStepSync();
    }

    REQUIRE (value == 0);
}

TEST_CASE("Encoder is not counting if inputs stay idle", "[encoder]")
{
    InputPort pA;
    InputPort pB;

    QuadDecoder dut(pA, pB);
    
    REQUIRE( dut.EvalStepSync() == 0 );
    REQUIRE( dut.EvalStepSync() == 0 );
    REQUIRE( dut.EvalStepSync() == 0 );
}


TEST_CASE("Encoder can count upwards if ports change accordingly ", "[encoder]")
{
    InputPort pA;
    InputPort pB;

    QuadDecoder dut(pA, pB);
    int16_t cnt = 0;

    SECTION( "Counting up" ) {
        REQUIRE( dut.EvalStepSync() == cnt++ );
        pA.Value = 1;
        REQUIRE( dut.EvalStepSync() == cnt++ );
        pB.Value = 1;
        REQUIRE( dut.EvalStepSync() == cnt++ );
        pA.Value = 0;
        REQUIRE( dut.EvalStepSync() == cnt++ );
        pB.Value = 0;
        REQUIRE( dut.EvalStepSync() == cnt++ );
    }
    SECTION( "Counting down" ) {
        REQUIRE( dut.EvalStepSync() == cnt-- );
        pB.Value = 1;
        REQUIRE( dut.EvalStepSync() == cnt-- );
        pA.Value = 1;
        REQUIRE( dut.EvalStepSync() == cnt-- );
        pB.Value = 0;
        REQUIRE( dut.EvalStepSync() == cnt-- );
        pA.Value = 0;
        REQUIRE( dut.EvalStepSync() == cnt-- );
    }
}