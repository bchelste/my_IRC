# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/26 16:41:36 by lbenedar          #+#    #+#              #
#    Updated: 2022/05/25 20:57:35 by lbenedar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	ircserv
INC_DIR		=	includes
SRC_DIR		=	srcs
CC			=	clang++
SRCS		=	$(SRC_DIR)/main.cpp \
				$(SRC_DIR)/Server.cpp \
				$(SRC_DIR)/User.cpp \
				$(SRC_DIR)/Commands.cpp \
				$(SRC_DIR)/InputMessage.cpp \
				$(SRC_DIR)/Replies.cpp \
				$(SRC_DIR)/Utils.cpp \
				$(SRC_DIR)/Channel.cpp \
				$(SRC_DIR)/Tracer.cpp
OBJS		=	$(SRCS:.cpp=.o)
INC			=	-I$(INC_DIR) \
				-I$(INC_DIR)/templates
HEAD		=	$(INC_DIR)/Server.hpp \
				$(INC_DIR)/User.hpp \
				$(INC_DIR)/Commands.hpp \
				$(INC_DIR)/InputMessage.hpp \
				$(INC_DIR)/Utils.hpp \
				$(INC_DIR)/Replies.hpp \
				$(INC_DIR)/Channel.hpp \
				$(INC_DIR)/Tracer.hpp
LOGS		=	logs/*
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

clean_logs:
	$(RM) $(LOGS)

fclean: clean clean_logs
	$(RM) $(NAME)

re: fclean all

%.o:%.cpp $(HEAD)
	$(CC) $(CFLAGS) $(OPTFLAGS) -c $< -o $@

.PHONY: all bonus clean fclean re
