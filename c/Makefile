BIN_saline-solution-cli=saline-solution-cli
SRCS_saline-solution-cli=saline-solution-cli.c
OBJS_saline-solution-cli=$(SRCS_saline-solution-cli:.c=.o)
LIBS_saline-solution-cli=-lm

CFLAGS=-Wall -Werror -ggdb -O0 -D_GNU_SOURCE=1
LIBS_ALL=

all: $(BIN_saline-solution-cli)


$(BIN_saline-solution-cli): $(OBJS_saline-solution-cli)
	$(CC) -o $@ $(OBJS_saline-solution-cli) $(LIBS_saline-solution-cli) $(LIBS_ALL)

clean:
	$(RM) $(BIN_saline-solution-cli) $(OBJS_saline-solution-cli) *~

run_saline-solution-cli: $(BIN_saline-solution-cli)
	./$(BIN_saline-solution-cli)
