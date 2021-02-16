#include "TestUtils.h"

TEST_CASE("Construction of DUT", "[Generic]") {
    REQUIRE (true);
}

static bool stringIsEqual(const char * expected, const char * actual);
void appendExtension( char * filepath)
{
    if (strchr(filepath, '.') == NULL)
        strcpy(&filepath[strlen(filepath)], ".html"); 
}

TEST_CASE("Check if missing line endign is correctly recognized", "[strings]")
{
    char buffer[32];

    SECTION( "Without file extension" ) {
        strcpy(buffer, "SomeRoute");
        appendExtension(buffer);
        CHECK(stringIsEqual(buffer, "SomeRoute.html"));
    }
    SECTION( "File extension" ) {
        strcpy(buffer, "SomeFile.txt");
        appendExtension(buffer);
        CHECK(stringIsEqual(buffer, "SomeFile.txt"));
    }
    SECTION( "FilePath in folder" ) {
        strcpy(buffer, "/server/SomeFile.txt");
        appendExtension(buffer);
        CHECK(stringIsEqual(buffer, "/server/SomeFile.txt"));
    }
}

TEST_CASE("Simple cJSON test in Module", "[cJson]") 
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", 42);
    const char *sys_info = cJSON_PrintUnformatted(root);
    REQUIRE( stringIsEqual("{\"raw\":42}", sys_info) );
    free((void *)sys_info);
    cJSON_Delete(root);
}

TEST_CASE("Structured cJSON test in Module", "[cJson]")
{
    const char * input = "{ \"StartupMode\": \"RunDemo\", \"DisplayMode\": \"ExpertView\", \"Outputs\": [ \
        { \"Type\": \"SyncLedCh\", \"Strip\": { \"LedCount\": 6, \"Intens\": 16, \"Channel\": \"RGB\" }, \"Color\": [0,0,0,0] }, \
        { \"Type\": \"AsyncLedCh\", \"Strip\": { \"LedCount\": 24, \"Intens\": 16, \"Channel\": \"RGBW\" }, \"Color\": [0,0,0,0] } ] }";


    // cJSON *root = cJSON_Parse(input);
    // const char *startup = cJSON_GetObjectItem(root, "StartupMode")->valuestring;
    // const char *display = cJSON_GetObjectItem(root, "DisplayMode")->valuestring;
    // const char *output = cJSON_GetObjectItem(root, "Outputs");
}


static bool stringIsEqual(const char * expected, const char * actual)
{
    if(strcmp(expected, actual) == 0)
        return true;

    UNSCOPED_INFO("Expected " << expected);
    UNSCOPED_INFO("Actual " << actual);
    return false;
}