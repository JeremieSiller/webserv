NAME = webserv
CC = clang++
CFLAGS = -Wall -Wextra
#need to add -Werror

SRC =	main.cpp					\
		parsing/AToken.cpp			\
		parsing/ConfigParser.cpp	\
		parsing/ConfigToken.cpp		\
		parsing/utility.cpp			\
		server/client.cpp			\
		server/connection.cpp		\
		server/location.cpp			\
		server/server.cpp			\
		server/webserv.cpp

OBJ = $(SRC:.cpp=.o)


%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(NAME)


$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all