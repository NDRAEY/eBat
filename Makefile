# Сборка для прочих ос
SRC_MAIN = $(wildcard modules/other/*.c) $(wildcard modules/*.c) $(wildcard *.c)
# Сборка для Linux || Windows
SRC_UNIX = $(wildcard modules/unix/*.c) $(wildcard modules/*.c) $(wildcard *.c)
OUTPUT_NAME = eBat.elf

all:
	$(MAKE) unix

debug:
	gcc $(SRC_UNIX) -o $(OUTPUT_NAME) -Wall -g -O0 -DDEBUG

build:
	gcc $(SRC_MAIN) -o $(OUTPUT_NAME) -Wall -g -Wunused-variable -O0

unix:
	gcc $(SRC_UNIX) -o $(OUTPUT_NAME) -Wall -g -Wunused-variable -O0

run:
	chmod 0777 $(OUTPUT_NAME)
	./$(OUTPUT_NAME)

log:
	chmod 0777 $(OUTPUT_NAME)
	./$(OUTPUT_NAME) > ebat.log

tests:
	-@./$(OUTPUT_NAME) "./examples/demo.bat"
	-@./$(OUTPUT_NAME) "./examples/enabled.bat"
	-@./$(OUTPUT_NAME) "./examples/if.bat"
	-@./$(OUTPUT_NAME) "./examples/set.bat"
	-@./$(OUTPUT_NAME) "./examples/run.bat"
	-@./$(OUTPUT_NAME) "./examples/goto.bat"
	-@./$(OUTPUT_NAME) "./examples/goto-if.bat"