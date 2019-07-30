##
## EPITECH PROJECT, 2018
## FTP
## File description:
## FTP Makefile
##

SRC		:=	$(addprefix ./src/, \
			main.cpp \
			argument.cpp \
			makefile.cpp)

OBJ		=	$(SRC:.cpp=.o)

NAME		=	checkmake

CXX		=	g++

CXXFLAGS	=	-W -Wall -Wextra -Werror -I include -std=c++17

all:			$(NAME)

$(NAME):		$(OBJ)
			$(CXX) $(OBJ) -o $(NAME)

clean:
			rm -rf $(OBJ)

fclean:			clean
			rm -rf $(NAME)

re:			fclean all

dbg:			CXXFLAGS += -g -D__DEBUG_MAKEFILE
dbg:			re

.PHONY:			all re dbg clean fclean
