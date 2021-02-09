class EasyHttp {
    /**
     * Execution of Get-Request
     * @param {string} url API route for request 
     */
    async get(url) {
        const response = await fetch(url);
        const result = await response.json();
        return result;
    }

    /**
     * Execution of Post-Request
     * @param {string} url API route for request 
     * @param {json} data JSON object to post
     */
    async post(url, data) {
        const response = await fetch(url, {
            method: 'POST',
            headers: { 'content-type': 'application/json' },
            body: JSON.stringify(data)
        });
        const result = await response.json();
        return result;
    }

    /**
     * Execution of Put-Request
     * @param {string} url API route for request 
     * @param {json} data JSON object to put
     */
    async put(url, data) {
        const response = await fetch(url, {
            method: 'PUT',
            headers: { 'content-type': 'application/json' },
            body: JSON.stringify(data)
        });
        const result = await response.json();
        return result;
    }

    /**
     * Execution of Put-Request
     * @param {string} url API route for request 
     */
    async delete(url) {
        const response = await fetch(url, {
            method: 'DELETE',
            headers: { 'content-type': 'application/json' },
        });
        const result = await 'Resource deleted';
        return result;
    }
}
