const http = new easyHttp();

const rgbiSyncForm = document.getElementById('rgbiSyncForm');
const rgbiAsyncForm = document.getElementById('rgbiAsyncForm');

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
 
function idFinder (id) { }
function UpdateColor(sender) {
    parent = sender.parentElement;
    while(parent.tagName != 'FORM'){
        parent = parent.parentElement;
    }
    elements = Array.from(parent.elements);
    
    let picker = elements.find( e => e.id.indexOf('rgbSelector') == 0 );
    //let selected = elements.find( e => e.id.indexOf('rgbSelected') == 0 );
    let redCh = elements.find( e => e.id.indexOf('rOut') == 0 );
    let greenCh = elements.find( e => e.id.indexOf('gOut') == 0 );
    let blueCh = elements.find( e => e.id.indexOf('bOut') == 0 );
    let redSl = elements.find( e => e.id.indexOf('red') == 0 );
    let greenSl = elements.find( e => e.id.indexOf('green') == 0 );
    let blueSl = elements.find( e => e.id.indexOf('blue') == 0 );

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


function onSubmitColor( form, SetUrl){
    const formData = new FormData(form).entries();
    let jsonObject = {};
    jsonObject["form"] = form.name;

    rgbiChannel_h = document.getElementById('rgbiChannel');

    for (const [key, value]  of formData) {
        jsonObject[key] = value;
    }
    console.log(JSON.stringify(jsonObject));

    var xhr = new XMLHttpRequest();
    xhr.open("POST", SetUrl, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(jsonObject));
    return false; //don't submit
}


// const SetupUi = (function() {
	
//   	function init() {
//            data.push(item); 
//         }
  
//     return {
//     	add: init,
    
//     }
// })();

