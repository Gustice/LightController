const http = new EasyHttp();

let uploadTarget = undefined;

function DeleteFile(file) {
    
}
function DownloadFile(file) {

}

function GetFileName (str) {
    return str.split('\\').pop().split('/').pop();
}

function UploadFile() {
    var filePath = document.getElementById("filepath").value;
    let filename = GetFileName(filePath);
    var fileInput = document.getElementById("newfile").files;

    if (fileInput.length == 0) {
        alert("Can't upload file. No file selected!");
    } else if (filePath.indexOf(' ') >= 0) {
        alert("Can't upload file. File path on server cannot have spaces!");
    } else if (fileInput[0].size > FileSizeLimit.size) {
        alert(`Can't upload file. File size must be less than ${FileSizeLimit.sizeString}!`);
    } else {
        document.getElementById("filepath").disabled = true;
        document.getElementById("uploadButton").disabled = true;

        var file = fileInput[0];
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (xhttp.readyState == 4) {
                if (xhttp.status == 200) {
                    document.open();
                    document.write(xhttp.responseText);
                    document.close();
                } else if (xhttp.status == 0) {
                    alert("Server closed the connection abruptly!");
                    location.reload()
                } else {
                    alert(xhttp.status + " Error!\n" + xhttp.responseText);
                    location.reload()
                }
            }
        };
        xhttp.open("POST", "/upload/" + filename, true);
        xhttp.send(file);
    }
}

const SetupPage = (function () {
    const fileGetUrl = ApiGetFiles;

    function GenerateFileTableRow(file) {
        const row = document.createElement('tr');

        const fileCol = document.createElement('td');
        fileCol.innerText = file;
        const loadButton = document.createElement('td');
        loadButton.innerHTML = `<button  type="button" onclick="DownloadFile('${file})'">download</button>`
        const delButton = document.createElement('td');
        delButton.innerHTML = `<button  type="button" onclick="DeleteFile('${file}')">Delete</button>`

        row.appendChild(fileCol);
        row.appendChild(loadButton);
        row.appendChild(delButton);
        return row;
    }
    function GenerateFileTableHeader() {
        let header = document.createElement('tr');
        header.innerHTML = `<th>Filename</th> <th></th> <th></th>`;

        return header;
    }
    function GenerateFileTableFooter() {
        let header = document.createElement('tr');

        const label = document.createElement('td');
        label.innerHTML = `<td>  </td>`;
        const input = document.createElement('td');
        input.innerHTML = ``
        const button = document.createElement('td');
        button.innerHTML = ``

        header.appendChild(label);
        header.appendChild(input);
        header.appendChild(button);

        return header;
    }


    data = [];
    function GetFiles() {
        http.get(ApiGetStatus_DeviceType).then(data => {
            document.getElementById('DeviceTypeText').innerText = data.DeviceType;
            document.getElementById('DeviceSnText').innerText = data.SerialNumber;
        }).catch(ex => {
            console.error(ex);
            alert('Files cannot be requested. Please contact support, and see log for more details');
        })

        

        // http.get(fileGetUrl).then(data => {

        //     // const table = document.getElementById('localFilesTable');
        //     // table.innerHTML = '';
        //     // table.appendChild(GenerateFileTableHeader());
        //     // data.files.forEach(file => {
        //     //     table.appendChild(GenerateFileTableRow(file));
        //     // });

        // }).catch(ex => {
        //     console.error(ex);
        //     alert('Files cannot be requested. Please contact support, and see log for more details');
        // })
    }

    function Setup() {
        document.getElementById("ConfigFileBtn").innerText = FileConfigFile
        document.getElementById("DeviceFileBtn").innerText = FileDeviceFile

    }

    return {
        GetFiles: GetFiles,
        Setup: Setup
    }
})();

/// Usage
SetupPage.Setup();
SetupPage.GetFiles();