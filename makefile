# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cbarber <cbarber@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/08 10:25:40 by cbarber           #+#    #+#              #
#    Updated: 2026/09/19 15:33:08 by misalber         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

OBJ_DIR = ./build/
SRCS_DIR = ./src/
INCLUDEDIR = ./include/

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS = main.cpp\
	   utils.cpp\
	   Debug.cpp\
       Parser.cpp\
       Server.cpp\
       Client.cpp\
	   Channel.cpp\
	   ChannelDomain.cpp\
       NumericReplies.cpp\
	   handler_message.cpp\
	   handler_commands.cpp\

OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))

MAKEFLAGS += --no-print-directory
.SILENT:

all: $(NAME)

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	echo "$(NAME) built successfully."

$(OBJ_DIR)%.o: $(SRCS_DIR)%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -g -DENABLE_DEBUG -c $< -o $@

clean:
	rm -f $(OBJS)
	echo "Object files cleaned."

fclean: clean
	rm -f $(NAME)
	rm -f $(LIBFTNAME)
	echo "Fully cleaned project."

re: fclean all

.PHONY: all clean fclean re
