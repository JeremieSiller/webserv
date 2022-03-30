import requests

# data = {
# 	'abc':'yeet',
# 	'delta':'yefb'
# }
# url = "http://localhost:5000/"
# response = r.post(url=url, data=data)
# print("status code:", response.status_code)
# print("text:", response.text)
# print("json:", response.json())
with open('file_should_exist_after.bla', 'rb') as f:
	r = requests.post('http://localhost:8000/directory/youpi.bla', files={'file_should_exist_after.bla': f})