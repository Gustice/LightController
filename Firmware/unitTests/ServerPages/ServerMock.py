from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
import re
import os
import json

scriptLocation = (os.path.dirname(__file__))
rootDir = os.path.dirname(os.path.dirname(scriptLocation)) # ../..
dutDir = os.path.join(rootDir, 'serverPages')
configDir = os.path.join(rootDir, 'factoryCfg')
print("Initial Notes for Test Routine")
print("  Place test-data in: " + scriptLocation)
print("  Pages are placed in: " + dutDir)

os.chdir(dutDir)
nextApiResponse = '{"Test": "Response"}'

def load_binary(file):
    with open(file, 'rb') as file:
        return file.read()
def search_item (name, items):
    for keyval in items:
        if name.lower() == keyval['url'].lower():
            return keyval['responses']


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
            with open(configDir +'\IOConfiguration.json', 'r') as content_file:
                stationConfig = content_file.read()
            wStat = stationConfig
            self.send_header('Content-type', 'text/json')
            self.end_headers()
            self.wfile.write("{}".format(wStat).encode('utf-8'))
            return

        with open(scriptLocation +'\Responses.json', 'r') as response_file:
            responsePool = json.loads(response_file.read())
            found = search_item(self.path, responsePool['GetResponses'])
            if (found != None):
                print(f"        api Path found '{self.path}'")
                print(f"        => Responding: '{json.dumps(found[0])}'")

                self.send_response(200)
                wStat = json.dumps(found[0])
                self.send_header('Content-type', 'text/json')
                self.end_headers()
                self.wfile.write("{}".format(wStat).encode('utf-8'))
                return

        if self.path == '/':
            self.path = '/welcome.html' # redirect
        # if self.path == '/favicon.ico':
        #     self.path = '/favicon.png'

        responseType = 'text/html'
        ending = ""

        if match := re.search('\\w+[.](\\w+)', self.path):
            ending = match.group(1)
        
        print(f"        Evaluation request path '{self.path}'")
        print(f"        Ending recognized '{ending}'")
        if ending == "":
            responseType = 'text/html'
            self.path = self.path + ".html"
        if ending == 'txt':
            responseType = 'text/txt'
        elif ending == 'js':
            responseType = 'application/javascript'
        elif ending == 'css':
            responseType = 'text/css'
        elif ending == 'png':
            responseType = 'image/png'
        elif ending == 'ico':
            responseType = 'image/*'
            self.send_response(200)
            self.send_header('Content-type', responseType)
            self.end_headers()
            self.wfile.write(load_binary(self.path[1:]))
            return

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
