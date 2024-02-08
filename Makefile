NAME = micro

CXX = gcc
CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address

SRC = micro.c

all : $(NAME)

$(NAME) : $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(NAME)

clean :
	rm -rf $(NAME)

fclean : clean

re : fclean all

.PHONE : all clean fclean re