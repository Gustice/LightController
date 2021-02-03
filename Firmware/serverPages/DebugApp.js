const http = new easyHttp();

function test_Get_RGBIValues()
{
    http.get('/api/Values/RGBISync', dDisplay);
}
function test_Get_RGBWValues()
{
    http.get('/api/Values/RGBWAsync', dDisplay);
}
function test_Get_RGBWSingleValue()
{
    http.get('/api/Values/RGBWSingle', dDisplay);
}
function test_Get_IValues()
{
    http.get('/api/Values/IValues', dDisplay);
}
function test_Get_WiFiStatus()
{
    http.get('/api/Status/WiFiStatus', dDisplay);
}

function dDisplay(err, response) {
    if (err) {
        console.log(err);
    } else {
        console.log(response);
    }
}

function test_Set_RGBIValues()
{
    http.post('/api/Port/RGBISync', '{}', dDisplay);
}
function test_Set_RGBWValues()
{
    http.post('/api/Port/RGBWAsync', '{}', dDisplay);
}
function test_Set_RGBWSingleValue()
{
    http.post('/api/Port/RGBWSingle', '{}', dDisplay);
}
function test_Set_IValues()
{
    http.post('/api/Port/IValues', '{}', dDisplay);
}
function test_Set_SaveToPage()
{
    http.post('/api/SaveToPage', '{}', dDisplay);
}
function test_Set_ResetProgram()
{
    http.post('/api/ResetProgram', '{}', dDisplay);
}

