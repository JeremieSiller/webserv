docker rm nginx
docker build -t nginx_test .
docker run -v $PWD:/var/www/files -p"8080:8080" -p"7070:7070" --name nginx nginx_test