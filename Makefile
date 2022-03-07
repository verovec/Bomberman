CC = gcc

CFLAGS = -Wall -Wextra -Werror

THREADS = -pthread

NAME = bomberman

RM = rm -rf

SRC = $(wildcard src/*.c)

OBJ = $(SRC:.c=.o)

LIB =  -lSDL2 -lSDL2_image -lSDL2_ttf

all : $(NAME)

%.o : %.c
	$(CC) -o $@ -c $<

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(THREADS) -o $@ $^ $(LIB)

clean :
	$(RM) *.o

fclean : clean
	$(RM) $(NAME)

re	: fclean all