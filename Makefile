NAME = webserv
CC = g++
CFLAGS = -Wall -Wextra -std=c++98
#need to add -Werror

INC =	interpreter/Interpreter.hpp	\
		parsing/AToken.hpp			\
		parsing/ConfigParser.hpp	\
		parsing/ConfigToken.hpp		\
		parsing/Lexer.hpp			\
		parsing/utility.hpp			\
		request/Request.hpp			\
		response/errorcodes.hpp		\
		response/response.hpp		\
		server/client.hpp			\
		server/connection.hpp		\
		server/server.hpp			\
		server/webserv.hpp			\
		main.hpp					\
		interpreter/Interpreter.hpp	\
		interpreter/cgi.hpp			\

SRC =	main.cpp					\
		parsing/AToken.cpp			\
		parsing/ConfigParser.cpp	\
		parsing/ConfigToken.cpp		\
		parsing/utility.cpp			\
		server/client.cpp			\
		server/connection.cpp		\
		server/server.cpp			\
		server/webserv.cpp			\
		request/Request.cpp			\
		response/response.cpp		\
		interpreter/Interpreter.cpp	\
		interpreter/cgi.cpp			\

#server/location.hpp			\
#server/location.cpp			\

OBJ = $(SRC:.cpp=.o)


%.o : %.cpp $(INC)
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(NAME)


$(NAME) : $(OBJ) $(INC)
	$(CC) $(CFLAGS) $(OBJ) -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all