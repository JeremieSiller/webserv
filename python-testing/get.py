import logging
import requests

logging.basicConfig(level=logging.DEBUG)
s = requests.Session()
s.get('http://localhost:9000')
s.get('http://localhost:9000')
r = s.get("http://localhost:9000")
print(r.text)
