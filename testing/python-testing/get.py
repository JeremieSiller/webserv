import logging
import requests

logging.basicConfig(level=logging.DEBUG)
s = requests.Session()
s.get('http://localhost:9000/xyz')
s.get('http://localhost:9000/lol')
r = s.get("http://localhost:9000/dman")
# print(r.text)
