
const http = new EasyHttp();

const PwdUi = (function () {
    const container = document.getElementById('WiFiSetup');
    const devConfigSpace = document.getElementById('showDeviceConfiguration');

    function evalWifiStatus(response) {
        console.log(response);
        if (response.wifiStatus === 'WiFiParamIsSet') {
            container.innerHTML = configuredWiFiContent;
        } else {
            container.innerHTML = initWifiContent;
        }
    }
    function evalDeviceConfig(response) {
        const caption = document.createElement('h3');
        caption.appendChild(document.createTextNode('This Device includes following ports'));
        const list = document.createElement('ul');
        
        ports = response.Outputs;
        ports.forEach(function(element) {
            const item = document.createElement('li');
            item.appendChild(document.createTextNode(element.Description));
            list.appendChild(item);
        })
        devConfigSpace.appendChild(caption);
        devConfigSpace.appendChild(list);
    }

    async function init() {
        await http.get(ApiGetStatus_DeviceConfig)
            .then(data => evalDeviceConfig(data))
            .catch(err => console.log(err)); 
    
        await http.get(ApiGetStatus_WiFiStatus)
        .then(data => evalWifiStatus(data))
        .catch(err => console.log(err));
    }
    function showWifiOptions() {
        container.innerHTML = queryWiFiContent;
    }
    
    // type may be  emphasis ,userNote ,userWarning ,userError ,userSuccess ,
    function showMessage(type, message) {
        const basePoint = container;
        const parent = basePoint.parentElement;
        const now = new Date();

        const messageObj = document.createElement('div');
        messageObj.className = 'row ' + type;
        let stamp = 'VolatileNote' + now.getTime();
        messageObj.id = stamp;
        messageObj.appendChild(document.createTextNode(message));
        
        parent.insertBefore(messageObj, basePoint.nextSibling);
        setTimeout(function() {document.getElementById(stamp).remove()}, 3000);
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
    PwdUi.init()
        .then(() => console.log("Init Successful"))
        .catch(() => alert("Gui init failed - Navigate to " + Route_SetupFull));
}

function setWifiConfig(form) {
    http.post(ApiSetDevice_SetWiFi);
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
<h2>WiFi Connect</h2>
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
<h2>WiFi Connect </h2>
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