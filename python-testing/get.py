import requests as r

url = "http://localhost:8070/"
header = {
	'hallo':'test',
	'xyz':'test'
}
#Query string
data = {
	'code':'123',
	'valuee':'ddd'
}
# while (True):
response = r.post(url=url, data=data)
print("status code:", response.status_code)
print("text:", response.text)
# print("json:", response.json())
