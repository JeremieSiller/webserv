import requests as r

data = {
	'first':'xyz',
	'second':'abc',
	'third':'lol'
}
url = "http://localhost:8070/"
response = r.post(url=url, data=data)
print("status code:", response.status_code)
print("text:", response.text)
print("json:", response.json())