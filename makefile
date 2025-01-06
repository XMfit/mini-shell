all: bin/shell

bin/shell: src/main.c src/shell_interactive.c src/exec_args.c src/built_in_proc.c src/new_proc.c src/color_setting.c -lreadline
	gcc -o $@ $^

clean:
	rm -rf bin/*
