const http = new easyHttp();

function test_Get_RGBIValues()
{
    http.get(ApiGetPort_RGBISync, dDisplay);
}
function test_Get_RGBWValues()
{
    http.get(ApiGetPort_RGBWAsync, dDisplay);
}
function test_Get_RGBWSingleValue()
{
    http.get(ApiGetPort_RGBWSingle, dDisplay);
}
function test_Get_IValues()
{
    http.get(ApiGetPort_IValues, dDisplay);
}

function test_Set_RGBIValues()
{
    http.post(ApiSetPort_RGBISync, '{}', dDisplay);
}
function test_Set_RGBWValues()
{
    http.post(ApiSetPort_RGBWAsync, '{}', dDisplay);
}
function test_Set_RGBWSingleValue()
{
    http.post(ApiSetPort_RGBWSingle, '{}', dDisplay);
}
function test_Set_IValues()
{
    http.post(ApiSetPort_IValues, '{}', dDisplay);
}




function test_Get_WiFiStatus()
{
    http.get(ApiGetStatus_WiFiStatus, dDisplay);
}
function test_Set_SaveToPage()
{
    http.post(ApiSaveToPage, '{}', dDisplay);
}
function test_Set_ResetProgram()
{
    http.post(ApiResetProgram, '{}', dDisplay);
}



function dDisplay(err, response) {
    if (err) {
        console.log(err);
    } else {
        console.log(response);
    }
}

