import requests as r

data = {
	'abc':'yeet',
	'delta':'yefb'
}
url = "http://localhost:5000/"
response = r.post(url=url, data=data)
print("status code:", response.status_code)
print("text:", response.text)
print("json:", response.json())