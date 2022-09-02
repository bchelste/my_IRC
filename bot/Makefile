# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/29 13:23:02 by lbenedar          #+#    #+#              #
#    Updated: 2022/05/29 15:07:38 by lbenedar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	infobot
INC_DIR		=	includes
SRC_DIR		=	srcs
CC			=	clang++
SRCS		=	$(SRC_DIR)/main.cpp \
				$(SRC_DIR)/Bot.cpp \
				$(SRC_DIR)/Utils.cpp
OBJS		=	$(SRCS:.cpp=.o)
INC			=	-I$(INC_DIR)
HEAD		=	$(INC_DIR)/Bot.hpp \
				$(INC_DIR)/Utils.hpp
CFLAGS		=	-Wextra -Werror -Wall -pthread -std=c++98 $(INC)
OPTFLAGS	=	-O0
MAKE		=	make
RM			=	rm -rf

all:
	@$(MAKE) $(NAME) -j 4

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OPTFLAGS) $(OBJS) -o $(NAME)

bonus: all

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

%.o:%.cpp $(HEAD)
	$(CC) $(CFLAGS) $(OPTFLAGS) -c $< -o $@

.PHONY: all bonus clean fclean re