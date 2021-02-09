from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
import re

versatileApiRouts = [   
    '/api/GetStatus/WiFiStatus', 
    '/api/GetStatus/DeviceConfig'
]
nextApiResponse = '{"wifiStatus": "Test"}'
stationConfig ='{ "StartupMode": "RunDemo", "DisplayMode": "ExpertView", "Outputs": [ { "Type": "SyncLedCh", "Description": "Synchronous serial LED port",  "Strip": { "LedCount": 6, "Intens": 16, "Channel": "RGB" }, "Color": [0,0,0,0] }, { "Type": "AsyncLedCh", "Description": "Asynchronous serial LED port",  "Strip": { "LedCount": 24, "Intens": 16, "Channel": "RGBW" }, "Color": [0,0,0,0] },         { "Type": "RgbStrip", "Description": "Rgb-Strip LED port", "Strip": { "LedCount": 6, "Intens": 1024, "Channel": "RGBW" }, "Color": [0,0,0,0] }, { "Type": "I2cExpander", "Description": "I2C-Expander port", "Device": { "LedCount": 3, "Intens": 1024, "Channel": "Grey" }, "Address": 1, "GrayValues": [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0] } ] }'



class MockServer(BaseHTTPRequestHandler):

    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        

    def _html(self, message, details = ""):
        content = f"<html><body><h1>{message}</h1><p>{details}</p></body></html>"
        return content.encode("utf8")  # NOTE: must return a bytes object!

    def do_GET(self):
        if (self.path == '/api/GetStatus/DeviceConfig'):
            print(f"        api Path for Device Config found '{self.path}'")
            self.send_response(200)
            wStat = stationConfig
            self.send_header('Content-type', 'text/json')
            self.end_headers()
            self.wfile.write("{}".format(wStat).encode('utf-8'))
            return
        if any(item == self.path in item for item in versatileApiRouts):
            print(f"        api Path found '{self.path}'")
            self.send_response(200)
            wStat = nextApiResponse
            self.send_header('Content-type', 'text/json')
            self.end_headers()
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
        if ending == "":
            responseType = 'text/html'
            self.path = self.path + ".html"
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
            return
        
        self.send_header('Content-type', responseType)
        self.end_headers()
        self.wfile.write(bytes(file_to_open, 'utf-8'))


    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        data = post_data.decode("utf-8")

        if self.path == '/Debug/SetNextJsonResponse':
            nextApiResponse = data
            print(f"       => Next Payload on API path '{nextApiResponse}'")
            self._set_response()
            self.send_response(200)
            return
        
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
