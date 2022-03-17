import requests as r

url = "http://localhost:8070/"
response = r.get(url=url)
print("status code:", response.status_code)
print("text:", response.text)
print("json:", response.json())
