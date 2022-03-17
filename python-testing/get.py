import logging
import requests

logging.basicConfig(level=logging.DEBUG)
s = requests.Session()
s.get('http://localhost:8070')
s.get('http://localhost:8070')
r = s.get("http://localhost:8070")
print(r.text)