
/**
 * Http Request handler
 * Ajax-Based
 */
function easyHttp() {
    this.http = new XMLHttpRequest();
}

/**
 * Execution of Get-Request
 * @param {string} url URL to API route
 * @param {function} cbk Callback function for displaying of the response
 */
easyHttp.prototype.get = function(url, cbk) {
    this.http.open('GET', url, true);
    let self = this;
    this.http.onload = function () {
        if (self.http.status === 200) {
            cbk(null, self.http.responseText);
        } else {
            cbk('Error: ' + self.http.status, '');
        }
    }
    this.http.send();
}

/**
 * Execution of Post-Request
 * @param {string} url URL to API route
 * @param {JSON} data Data object as JSON-object
 * @param {function} cbk Callback function for displaying of the response
 */
easyHttp.prototype.post = function(url, data, cbk) {
    this.http.open('POST', url, true);
    this.http.setRequestHeader('Content-type', 'application/json');
    let self = this;
    this.http.onload = function () {
        cbk(null, self.http.responseText);
    }
    this.http.send(JSON.stringify(data));
}

/**
 * Execution of Put-Request
 * @param {string} url URL to API route
 * @param {JSON} data Data object as JSON-object
 * @param {function} cbk Callback function for displaying of the response
 */
easyHttp.prototype.put = function(url, data, cbk) {
    this.http.open('PUT', url, true);
    this.http.setRequestHeader('Content-type', 'application/json');
    let self = this;
    this.http.onload = function () {
        cbk(null, self.http.responseText);
    }
    this.http.send(JSON.stringify(data));
}

/**
 * Execution of Delete-Request
 * @param {string} url URL to API route
 * @param {function} cbk Callback function for displaying of the response
 */
easyHttp.prototype.delete = function(url, cbk) {
    this.http.open('DELETE', url, true);
    this.http.setRequestHeader('Content-type', 'application/json');
    let self = this;
    this.http.onload = function () {
        if (self.http.status === 200) {
            cbk(null, 'Post Deleted');
        } else {
            cbk('Error: ' + self.http.status, '');
        }
    }
    this.http.send(JSON.stringify(data));
}