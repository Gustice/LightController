
const http = new easyHttp();

const PwdUi = (function () {
    const container = document.getElementById('WiFiSetup');
    const devConfigSpace = document.getElementById('showDeviceConfiguration');


    function display(err, response) {
        if (err) {
            console.log(err);
        } else {
            console.log(response);
        }
    }
    function evalWifiStatus(err, response) {
        if (err) {
            console.log(err);
        } else {
            console.log(response);
            if (response === 'WiFiParamSet') {
                container.innerHTML = configuredWiFiContent;
            } else {
                container.innerHTML = initWifiContent;
            }
        }
    }
    function evalDeviceConfig(err, response) {
        if (err) {
            console.log(err);
        } else {
            console.log(response);
            devConfigSpace.innerHTML = `<p>Content will be set accordingly</p>`
        }
    }

    function init() {
        http.get(ApiGetStatus_WiFiStatus, evalWifiStatus);
        http.get(ApiGetStatus_DeviceConfig, evalDeviceConfig);
    }
    function showWifiOptions() {
        container.innerHTML = queryWiFiContent;
    }
    function showMessage(message) {
        const basePoint = container;
        const parent = basePoint.parentElement;
                
        const messageObj = document.createElement('div');
        messageObj.className = 'row userNote';
        messageObj.appendChild(document.createTextNode(message));
        
        parent.insertBefore(messageObj, basePoint.nextSibling);
        setTimeout(function() {document.querySelector('.userNote').remove()}, 3000);
    }

    return {
        init: init,
        showWifiOptions: showWifiOptions,
        showMessage: showMessage
    }
})();

PwdUi.init();

function resetWifiConfig() {
    http.post(ApiSetDevice_ResetWiFi);
    PwdUi.init();
}

function setWifiConfig(form) {
    // http.post(ApiSetDevice_SetWiFi);
    const formData = new FormData(form).entries();
    let jsonObject = {};

    for (const [key, value]  of formData) {
        jsonObject[key] = value;
    }
    console.log(JSON.stringify(jsonObject));
    PwdUi.showMessage("WiFi Config sent");

    var xhr = new XMLHttpRequest();
    xhr.open("POST", ApiSetDevice_SetWiFi, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(jsonObject));
    return false; //don't submit
}

const initWifiContent = `
<div class="row">
<h2>Options</h2>
<p> You can directly proceed to setup the Lights on your device or first Setup your local
    WiFi-configuration to make the configuration more convenient. </p>
</div>
<div class="row">
    <div class="four columns"><button class="u-full-width" type="button" onclick="PwdUi.showWifiOptions()">Setup WiFi</button>
    </div>
</div>
<div class="row">
    <div class="four columns"><button class="button-primary u-full-width" type="button"
            onclick="window.location.href='${Route_SetupFull}'">Setup Lights</button></div>
</div>
`;
const queryWiFiContent = `
<div class="row">
<h2>WiFi Connect</h2>
<p> You can input your WiFi access code, in order to connect your device automatically.
    Consider to use a guest network to prevent exposing critical access codes to your network. </p>
<p> Note: If you what to reset the WiFi parameter, please press the first button during startup. </p>
</div>
<div class="row">
    <div class="eight columns">
        <div class="emphasis">
            <form onsubmit='return setWifiConfig(this)'>
                <div class="row">
                    <div class="four columns"><label for="ssid">SSID</label></div>
                    <div class="eight columns"><input class="u-full-width" type="text" id="ssid" name="ssid"></div>
                </div>
                <div class="row">
                    <div class="four columns"><label for="password">Password</label></div>
                    <div class="eight columns"><input class="u-full-width" type="password" id="password"
                            name="password">
                    </div>
                </div>
                <div class="row">
                    <div class="six columns"><input class="button-primary u-full-width" type="submit" value="Submit"></div>
                    <div class="six columns"><button class="u-full-width" type="button"
                            onclick="window.location='${Route_SetupFull}'">Skip</button></div>
                </div>
            </form>
        </div>
    </div>
</div>
`;
const configuredWiFiContent = `
<div class="row">
<h2>Options </h2>
<p> The station is already set to your wifi access point. You can either proceed to setup the Lights, or reset the wifi configuration </p>
<div class="row">
    <div class="four columns"><button class="button-primary u-full-width" type="button"
            onclick="window.location='${Route_SetupFull}'">Setup Lights</button></div>
</div>
<div class="row">
    <div class="four columns"><button class="u-full-width" type="button"
            onclick="resetWifiConfig()">Reset Wifi Config</button></div>
    </div>
</div>
`;