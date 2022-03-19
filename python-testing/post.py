import requests as r

header = 
{
	'Host: localhost:8070',
	'Accept: */*',
	'Transfer-Encoding: Chunked',
}
data = {
	'1\r\naE\r\nHelloWorld!\nfd0\r\n\r\n'
}
url = "http://localhost:8070/"
response = r.post(url=url, data=data, header=header)
print("status code:", response.status_code)
print("text:", response.text)
print("json:", response.json())