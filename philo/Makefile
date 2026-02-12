NAME = philo

HEADER = includes/philo.h

SRCS = src/main.c \
	src/init.c \
	src/routine.c \
	src/monitor.c \
	src/cleanup.c \
	src/actions.c \
	src/parsing.c \
	src/utils.c

OBJS = $(SRCS:src/%.c=obj/%.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror -Iincludes

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $(NAME)
	@echo "$(NAME) compiled successfuly."

obj/%.o: src/%.c $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
