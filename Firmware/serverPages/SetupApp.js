const http = new easyHttp();


function toRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}
function hexVal(value) {
    return (+value).toString(16).padStart(2, '0');
}
function rgbHex(r, g, b) {
    return "#" + hexVal(r) + hexVal(g) + hexVal(b)
}
function rgb(r, g, b) {
    return "rgb(" + r + "," + g + "," + b + ")";
}

function idFinder(id) { }
function UpdateColor(sender) {
    parent = sender.parentElement;
    while (parent.tagName != 'FORM') {
        parent = parent.parentElement;
    }
    elements = Array.from(parent.elements);

    let picker = elements.find(e => e.id.indexOf('rgbSelector') == 0);
    //let selected = elements.find( e => e.id.indexOf('rgbSelected') == 0 );
    let redCh = elements.find(e => e.id.indexOf('rOut') == 0);
    let greenCh = elements.find(e => e.id.indexOf('gOut') == 0);
    let blueCh = elements.find(e => e.id.indexOf('bOut') == 0);
    let redSl = elements.find(e => e.id.indexOf('red') == 0);
    let greenSl = elements.find(e => e.id.indexOf('green') == 0);
    let blueSl = elements.find(e => e.id.indexOf('blue') == 0);

    if (sender.id != picker.id) {
        picker.value = rgbHex(redCh.value, greenCh.value, blueCh.value);
    }
    let color = picker.value;

    // var colorStr = rgb(redCh.value, greenCh.value, blueCh.value);
    //selected.innerHTML = picker.value;
    let rgb = toRgb(color);
    redCh.value = redSl.value = rgb.r;
    greenCh.value = greenSl.value = rgb.g;
    blueCh.value = blueSl.value = rgb.b;

    sender.preventDefault;
}

function onSubmitColor(form, SetUrl) {
    const formData = new FormData(form).entries();
    let jsonObject = {};
    jsonObject["form"] = form.name;

    for (const [key, value] of formData) {
        jsonObject[key] = value;
    }
    console.log(JSON.stringify(jsonObject));

    SetupUi.showMessage(form, "Data sent ...");

    var xhr = new XMLHttpRequest();
    xhr.open("POST", SetUrl, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(jsonObject));
    return false; //don't submit
}


const SetupUi = (function () {
    const controllerSpace = document.getElementById('formSpace');

    function createRgbControl(definition) {
        let frame = createDivRowFrame();
        frame.appendChild(rgbFormGenerator(definition));
        return frame;
    }
    function createGrControl(definition) {
        let frame = createDivRowFrame();
        frame.appendChild(grFormGenerator(definition));
        return frame;
    }

    function buildGuiToConfig(err, response) {
        if (err) {
            console.log(err);
        } else {

            const inputParam1 = {
                number: 1,
                name: 'rgbiSync',
                caption: 'SyncSerial LED-Stripe',
                description: 'Connects LED stripes with synchronous serial protocol (e.g. APA102 or LC8822). Each LED can be assigned with a different color.',
                channels: [
                    createColorChannelParam('darkred', 'Red', 'red', 'rOut', 255, 'R'),
                    createColorChannelParam('darkgreen', 'Green', 'green', 'gOut', 255, 'G'),
                    createColorChannelParam('darkblue', 'Blue', 'blue', 'bOut', 255, 'B'),
                    createColorChannelParam('black', 'Intensity', 'intens', 'iOut', 100, 'I')
                ],
                route: '/api/Port/RGBISync'
            };
            controllerSpace.appendChild(createRgbControl(inputParam1));
            
            const inputParam2 = {
                number: 2,
                name: 'rgbwAsync',
                caption: 'AsyncSerial LED-Stripe',
                description: 'Connects LED stripes with asynchronous serial RTZ-protocol (e.g. WS2812 or SK6812). Each LED can be assigned with a different color.',
                notes: ['Note: There several Types available on the market. Some with RGBW-Channels, most with RGB-Channels.'],
                channels: [
                    createColorChannelParam('darkred', 'Red', 'red', 'rOut', 255, 'R'),
                    createColorChannelParam('darkgreen', 'Green', 'green', 'gOut', 255, 'G'),
                    createColorChannelParam('darkblue', 'Blue', 'blue', 'bOut', 255, 'B'),
                    createColorChannelParam('gray', 'White', 'white', 'wOut', 255, 'W')
                ],
                route: '/api/Port/RGBWAsync'
            };
            controllerSpace.appendChild(createRgbControl(inputParam2));

            const inputParam3 = {
                number: 3,
                name: 'rgbAsync',
                caption: 'AsyncSerial LED-Stripe',
                description: 'Connects LED stripes with asynchronous serial RTZ-protocol (e.g. WS2812 or SK6812). Each LED can be assigned with a different color.',
                notes: ['Note: There several Types available on the market. Some with RGBW-Channels, most with RGB-Channels.'],
                channels: [
                    createColorChannelParam('darkred', 'Red', 'red', 'rOut', 255, 'R'),
                    createColorChannelParam('darkgreen', 'Green', 'green', 'gOut', 255, 'G'),
                    createColorChannelParam('darkblue', 'Blue', 'blue', 'bOut', 255, 'B'),
                ],
                route: '/api/Port/RGBWAsync'
            };
            controllerSpace.appendChild(createRgbControl(inputParam3));

            const inputParam4 = {
                number: 4,
                name: 'rgbwStrip',
                caption: 'RGB-LED-Stripe',
                description: 'Connects ordinary RGB-LED stripes. Whole stripe is illuminated in one color',
                channels: [
                    createColorChannelParam('darkred', 'Red', 'red', 'rOut', 255, 'R'),
                    createColorChannelParam('darkgreen', 'Green', 'green', 'gOut', 255, 'G'),
                    createColorChannelParam('darkblue', 'Blue', 'blue', 'bOut', 255, 'B'),
                ],
                route: '/api/Port/RGBWAsync'
            };
            controllerSpace.appendChild(createRgbControl(inputParam4));


            const inputParam5 = {
                number: 5,
                name: 'rgbwStrip',
                caption: 'LED-Expander',
                description: 'Connects LED-driver that are controlled by gray values',
                channels: {
                    count: 16,
                    inLine: 8,
                    definition: createColorChannelParam('gray', 'Out', 'chV', 'chS', 255, 'G'),
                },
                route: '/api/Port/IValues'
            };
            controllerSpace.appendChild(createGrControl(inputParam5));
        }
    }

    function init() {
        http.get(ApiGetStatus_DeviceConfig, buildGuiToConfig);
    }

    function showMessage(anchor, message) {
        const basePoint = document.getElementById(anchor.id);
        const parent = basePoint.parentElement.parentElement;
                
        const messageObj = document.createElement('div');
        messageObj.className = 'row userNote';
        messageObj.appendChild(document.createTextNode(message));
        
        parent.insertBefore(messageObj, basePoint.parentElement.nextSibling);
        setTimeout(function() {document.querySelector('.userNote').remove()}, 3000);
    }
    

    return {
        init: init,
        showMessage: showMessage
    }
})();

SetupUi.init();





function createColorChannelParam(color, text, varName, input, max, objName) {
    sliderParam = {
        bntColor: color,
        btnText: text,
        varName: varName,
        fieldRef: input,
        maxValue: max,
        objName: objName
    }
    return sliderParam;
}

function createDivRowFrame() {
    let frame = document.createElement('div');
    frame.className = 'row';
    return frame;
}
function createFormFrame(id, name, action) {
    let form = document.createElement('form');
    form.id = id;
    form.name = name;
    return form;
}

function rgbFormGenerator(param) {
    function createChannelControl(num){
        const controlTemplate = `
        <div class="four columns">
            <label for="appTo">Apply to ...</label>
            <input type="text" class="u-full-width" id="appTo${num}" name="appTo" value="1">
            <!-- <label for="rgbSelector" id="rgbSelected">Selected Color #000000</label> -->
            <input type="color" id="rgbSelector${num}" class="u-full-width" oninput="UpdateColor(this);" style="height: 70px;"><br>
            <button class="u-full-width" onclick="">Get Setting</button>
            <input class="button-primary u-full-width" type="submit" value="Apply">
        </div>
        `;
        return controlTemplate;
    }
    function createColoredSlider(param, num) {
        const sliderTemplate = `
            <button type="button" class="u-full-width" style="background-color: ${param.bntColor};" 
            onclick="${param.fieldRef}${num}.value = ${param.varName}${num}.value= ${param.maxValue}; UpdateColor(this);">${param.btnText}</button>
            <input type="range" id="${param.varName}${num}" oninput="${param.fieldRef}${num}.value = this.value; UpdateColor(this);" min="0"
            max="${param.maxValue}" step="1" value="0">
            `;
    
        const fieldTemplate = `
            <input type="number" id="${param.fieldRef}${num}" name="${param.objName}" oninput="${param.varName}${num}.value = this.value; UpdateColor(this);"
            class="u-full-width" value="0" min="0" max="${param.maxValue}">
            `;
    
        return { 
            slider: sliderTemplate,
            field: fieldTemplate
        };
    }

    const num = param.number;
    const caption = `Port ${num}: ${param.caption}`;
    const description = param.description;
    const formName = param.name; 
    const formId = `rgbiSyncForm_${num}`;
    const targetUrl = param.route;
    
    let table = document.createElement('table');
    let sliderRow = document.createElement('tr');
    let fieldRow = document.createElement('tr');
    param.channels.forEach(function(element){
        const control = createColoredSlider(element, num);

        const r1 = document.createElement('td');
        r1.innerHTML = control.slider,
        sliderRow.appendChild(r1);

        const r2 = document.createElement('td');
        r2.innerHTML = control.field,
        fieldRow.appendChild(r2);
    });
    table.appendChild(sliderRow);
    table.appendChild(fieldRow);
   
    let main = createDivRowFrame();
    main.innerHTML = `
    <h2>${caption}</h2>
    <p>${description}</p>
    `;

    let form = createFormFrame(formId, formName, )
    form.setAttribute('onsubmit',`return onSubmitColor(this, "${targetUrl}")`);
    let control = createDivRowFrame();
    control.innerHTML = createChannelControl(num);

    let slider = document.createElement('div');
    slider.className = "eight columns";
    slider.appendChild(table);

    control.appendChild(slider);
    form.appendChild(control);
    main.appendChild(form);
    return main;
}

function grFormGenerator(param) {
    function createChannelControl(num){
        const controlTemplate = `
        <div class="row">
          <div class="four columns">
            <button class="u-full-width" onclick="">Get Setting</button>
          </div>
          <div class="four columns">
            <input class="button-primary u-full-width" type="submit" value="Apply">
          </div>
        </div>
        `;
        return controlTemplate;
    }
    function createGraySlider(param, cnt, num) {
        const sliderTemplate = `
            <button type="button" style="background-color: ${param.bntColor};" onclick="${param.fieldRef}_${cnt}_${num}.value = ${param.varName}_${cnt}_${num}.value= ${param.maxValue};">${param.btnText} ${cnt}</button>
            <input type="range" id="${param.varName}_${cnt}_${num}" oninput="${param.fieldRef}_${cnt}_${num}.value = this.value;" min="0" max="${param.maxValue}" step="1" value="0"><br>
            <input type="number" name="${param.objName}${cnt}" id="${param.fieldRef}_${cnt}_${num}" oninput="${param.varName}_${cnt}_${num}.value = this.value;" class="u-full-width" value="0" min="0" max="${param.maxValue}">
            `;
        return sliderTemplate;
    }
    
    const num = param.number;
    const caption = `Port ${num}: ${param.caption}`;
    const description = param.description;
    const formName = param.name; 
    const formId = `rgbiSyncForm_${num}`;
    const targetUrl = param.route;
    
    let table = document.createElement('table');
    
    let toDo = param.channels.count;
    let cnt = 1;
    while(cnt <= toDo) {
        let row = param.channels.inLine;
        let sliderRow = document.createElement('tr');

        while ((row > 0) && (cnt <= toDo)) {
            const control = createGraySlider(param.channels.definition , cnt, num);
            const r1 = document.createElement('td');
            r1.innerHTML = control,
            sliderRow.appendChild(r1);
            cnt++; row--;
        }
        table.appendChild(sliderRow);
    }
       
    let main = createDivRowFrame();
    main.innerHTML = `
    <h2>${caption}</h2>
    <p>${description}</p>
    `;

    let form = createFormFrame(formId, formName, )
    form.setAttribute('onsubmit',`return onSubmitColor(this, "${targetUrl}")`);
    form.innerHTML = createChannelControl(num);

    let slider = document.createElement('div');
    slider.className = "eight columns";
    slider.appendChild(table);

    form.appendChild(slider);
    main.appendChild(form);
    return main;
}



function onSave(page) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/SaveToPage", true);
    xhr.setRequestHeader('Content-Type', 'application/text');
    xhr.send(page);
  }
  function onResetPages() {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/ResetProgramm", true);
    xhr.setRequestHeader('Content-Type', 'application/text');
    xhr.send();
  }

  function onGet(form) {
    var SetUrl = "/GetValues/RGBValues"
    console.log("sending to %s", SetUrl);

    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
      if (this.readyState != 4) return;
      if (this.status == 200) {
        //var data = JSON.parse(this.responseText);
        console.log(this.responseText);
      }
      // end of state change: it can be after some time (async)
    };
    xhr.open("GET", SetUrl, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send();
  }