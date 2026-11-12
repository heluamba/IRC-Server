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
BONUS_NAME = botClient

OBJ_DIR = ./build/
BOT_OBJ_DIR = ./build_bot/
SRCS_DIR = ./src/
BOT_DIR = ./bot/
INCLUDEDIR = ./include/

CXX = c++
CXXFLAGS = -g -Wall -Werror -Wextra -std=c++98

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

BOT_SRCS = Bot.cpp\
		   main.cpp\
		   BotCommands.cpp\
		   BotUtils.cpp

OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))

BOT_OBJS = $(addprefix $(BOT_OBJ_DIR), $(BOT_SRCS:.cpp=.o))

MAKEFLAGS += --no-print-directory
.SILENT:

all: $(NAME)

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	echo "$(NAME) built successfully."

$(OBJ_DIR)%.o: $(SRCS_DIR)%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -DENABLE_DEBUG -c $< -o $@

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BOT_OBJS)
	$(CXX) $(CXXFLAGS) $(BOT_OBJS) -o $(BONUS_NAME)
	echo "$(BONUS_NAME) built successfully."

$(BOT_OBJ_DIR)%.o: $(BOT_DIR)%.cpp
	mkdir -p $(BOT_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -I$(BOT_DIR) -c $< -o $@


clean:
	rm -f $(OBJS)
	rm -f $(BOT_OBJS)
	echo "Object files cleaned."

fclean: clean
	rm -f $(NAME)
	rm -f $(BONUS_NAME)
	rm -f $(LIBFTNAME)
	echo "Fully cleaned project."

re: fclean all

.PHONY: all clean fclean re
