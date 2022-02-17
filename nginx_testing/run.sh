docker rm nginx
docker build -t nginx_test .
docker run -p"8080:8080" --name nginx nginx_test