# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jesus <jesus@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/10/26 11:10:51 by jtrancos          #+#    #+#              #
#    Updated: 2020/11/23 14:17:40 by jesus            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS		= cub3d.c
OBJS		= ${SRCS:.c=.o}
NAME		= cub3D
CC			= gcc
RM			= rm -f
CFLAGS		= #-Wall -Werror -Wextra
FRAMEWORK	= -lmlx -lm -lbsd -lXext -lX11

#-lz -L . -lmlx -framework OpenGL -framework AppKit

LIBS		= -Lmlx_linux



.c.o:
			#${CC} ${CFLAGS} -g -c $< -o ${<:.c=.o}
			gcc -o $@ -c $< $(CFLAGS)

${NAME}:	${OBJS}
			#${MAKE} -C ./mlx
			${MAKE} -C ./mlx_linux
			#${MAKE} -C ./libft
			#mv ./minilibx/libmlx.a .
			#mv ./libft/libft.a .
			#${CC} -fsanitize=address -g -o ${NAME} ${OBJS} ${FRAMEWORK}
			${CC} -fsanitize=address ${LIBS} -o $(NAME) ${OBJS} ${FRAMEWORK} 

all:		${NAME}

clean:
			${RM} ${OBJS}
#			${MAKE} clean -C ./minilibx
#			${MAKE} clean -C ./libft

fclean : 	clean
			${RM} ${NAME} libmlx.a 

re:			fclean all

.PHONY:		re all clean fclean