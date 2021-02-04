from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
import re

class MockServer(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def _html(self, message, details = ""):
        content = f"<html><body><h1>{message}</h1><p>{details}</p></body></html>"
        return content.encode("utf8")  # NOTE: must return a bytes object!

    def do_GET(self):
        if self.path == '/api/Status/WiFiStatus':
            self.send_response(200)
            wStat = 'WiFiParamNotSet'
            self.wfile.write("{}".format(wStat).encode('utf-8'))
            return
        if self.path == '/api/Status/DeviceConfig':
            self.send_response(200)
            wStat = 'Full'
            self.wfile.write("{}".format(wStat).encode('utf-8'))
            return

        if self.path == '/':
            self.path = '/welcome.html' # redirect

        responseType = 'text/html'
        ending = ""
        if match := re.search('\\w+[.](\\w+)', self.path):
            ending = match.group(1)
        
        print(f"        Evaluation request path '{self.path}'")
        print(f"        Ending recognized'{ending}'")
        if ending == 'txt':
            responseType = 'text/txt'
        elif ending == 'js':
            responseType = 'application/javascript'
        elif ending == 'css':
            responseType = 'text/css'

        try:
            file_to_open = open(self.path[1:]).read()
            self.send_response(200)
        except:
            file_to_open = "File not found"
            self.send_response(404)
        
        self.send_header('Content-type', responseType)
        self.end_headers()
        self.wfile.write(bytes(file_to_open, 'utf-8'))


    def do_POST(self):
        # if self.path == 'SetDevice/WiFiConnect':
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        data = post_data.decode("utf-8")
        
        self._set_response()
        self.wfile.write(self._html("POST "+self.path, data))
        print(f"        Payload {data}")


port = 80
httpd = HTTPServer(('localhost', port), MockServer)
print("serving at port", port)
try:
    httpd.serve_forever()
except KeyboardInterrupt:
    pass

httpd.server_close()
