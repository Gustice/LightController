// #include "Apa102.h"
#include "TestUtils.h"
//#include "cJSON.h"

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
    const char * woExt = "SomeRoute";
    const char * wExt = "SomeFile.txt";

    char buffer[32];
    strcpy(buffer, woExt);
    appendExtension(buffer);
    CHECK(stringIsEqual(buffer, "SomeRoute.html"));

    strcpy(buffer, wExt);
    appendExtension(buffer);
    CHECK(stringIsEqual(buffer, "SomeFile.txt"));
}

TEST_CASE("Validating Test with linked cJSON in Module", "[cJson]") 
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", 42);
    const char *sys_info = cJSON_PrintUnformatted(root);
    REQUIRE( stringIsEqual("{\"raw\":42}", sys_info) );
    free((void *)sys_info);
    cJSON_Delete(root);
}

static bool stringIsEqual(const char * expected, const char * actual)
{
    if(strcmp(expected, actual) == 0)
        return true;

    UNSCOPED_INFO("Expected " << expected);
    UNSCOPED_INFO("Actual " << actual);
    return false;
}