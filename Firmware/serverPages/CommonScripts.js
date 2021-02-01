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



function UpdateColor() {
    //var color = rgbHex(redCh.value, greenCh.value, blueCh.value);
    var color = rgb(redCh.value, greenCh.value, blueCh.value);
    previewField.style["background-color"] = color;
}

function onSubmit( form ){
    const formData = new FormData(form).entries();
    let jsonObject = {};

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