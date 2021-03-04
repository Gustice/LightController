import json
import os

print(os.curdir)
scriptLocation = (os.path.dirname(__file__))

sampleInput = '/api/GetPort/RGBISync'

def search_item (name, items):
    for keyval in items:
        if name.lower() == keyval['url'].lower():
            return keyval['responses']

with open(scriptLocation +'\Responses.json', 'r') as response_file:
    responsePool = json.loads(response_file.read())
    print(responsePool['GetResponses'])
    found = search_item(sampleInput, responsePool['GetResponses'])
    if (found != None):
        print("output is", found[0])


