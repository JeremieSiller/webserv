FROM	nginx:latest
RUN		mkdir /var/www/
RUN		mkdir /var/www/files
RUN		mkdir /var/www/files/file2
COPY	nginx.conf /etc/nginx/nginx.conf
COPY	index.html /var/www/files/index.html
COPY	index2.html /var/www/files/file2/index2.html
CMD		["nginx", "-g", "daemon off;"]