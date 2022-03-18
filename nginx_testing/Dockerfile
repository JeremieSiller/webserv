FROM	nginx:latest
RUN     apt-get update -y
RUN     apt-get install net-tools -y
RUN		mkdir /var/www/
RUN		mkdir /var/www/files
RUN		mkdir /var/www/files/www
RUN		mkdir /var/www/files/www/xyz
COPY	nginx.conf /etc/nginx/nginx.conf
COPY	index.html /var/www/files/index.html
COPY	index2.html /var/www/files/index2.html
COPY	www.html /var/www/files/www/www.html
COPY	index2.html /var/www/files/www/index2.html
COPY	www.html /var/www/files/www/xyz/www.html
COPY	index2.html /var/www/files/www/xyz/index2.html
CMD		["nginx", "-g", "daemon off;"]