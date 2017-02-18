objects=  yinka_daemon_shell.o  yinka_daemon_ui.o
yinka_daemon_ui: $(objects)
	cc -o yinka_daemon_ui $(objects) -lcurses
yinka_daemon_shell.o: yinka_daemon_shell.h
.PHONY: clean
clean:
	rm yinka_daemon_ui $(objects)
