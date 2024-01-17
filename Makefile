SRCS_DIR	= srcs/


SRC	=	main.cpp\
			Server.cpp\
			Channel.cpp\
			User.cpp\
			tools.cpp\


SRCS =$(addprefix $(SRCS_DIR), $(SRC))

INCS = -I ./includes/

OBJS_DIR = obj/

OBJ	=	${SRC:.cpp=.o}

OBJS = $(addprefix $(OBJS_DIR), $(OBJ))

NAME	=	ircserv

CC		=	c++

CCFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g

all: 	$(OBJS_DIR) ${NAME}

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	${CC} ${CCFLAGS} -c $< -o $@ ${INCS}

$(NAME): $(OBJS)
	${CC} ${CCFLAGS} ${OBJS} -o ${NAME}

$(OBJS_DIR):
	mkdir $(OBJS_DIR)

clean:
	rm -rf ${OBJS_DIR}

fclean: clean
	rm -f ${NAME}

re:		fclean all

.PHONY:	all clean fclean re
