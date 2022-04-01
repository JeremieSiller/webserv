NAME = webserv
CC = g++
CFLAGS = -Wall -Wextra -std=c++98
#need to add -Werror

INC =	src/interpreter/Interpreter.hpp	\
		src/parsing/AToken.hpp			\
		src/parsing/ConfigParser.hpp	\
		src/parsing/ConfigToken.hpp		\
		src/parsing/Lexer.hpp			\
		src/parsing/utility.hpp			\
		src/request/Request.hpp			\
		src/response/errorcodes.hpp		\
		src/response/response.hpp		\
		src/server/client.hpp			\
		src/server/connection.hpp		\
		src/server/server.hpp			\
		src/server/webserv.hpp			\
		src/main.hpp					\
		src/interpreter/Interpreter.hpp	\
		src/interpreter/cgi.hpp			\
		src/interpreter/utils.hpp		\

SRC =	src/main.cpp					\
		src/parsing/AToken.cpp			\
		src/parsing/ConfigParser.cpp	\
		src/parsing/ConfigToken.cpp		\
		src/parsing/utility.cpp			\
		src/server/client.cpp			\
		src/server/connection.cpp		\
		src/server/server.cpp			\
		src/server/webserv.cpp			\
		src/request/Request.cpp			\
		src/response/response.cpp		\
		src/interpreter/Interpreter.cpp	\
		src/interpreter/cgi.cpp			\

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