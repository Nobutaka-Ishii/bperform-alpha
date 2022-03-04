NAME = bperform

$(NAME) : bperform.c ctrlFuncs.c prepIns.c signal_events.c splitline.c
	gcc -Wall -O0 -g -o $(NAME) bperform.c ctrlFuncs.c prepIns.c signal_events.c splitline.c -I./ `pkg-config --libs --cflags gtk+-3.0` -lm  -lasound

install : $(NAME)
	cp ./$(NAME) $(HOME)/bin/$(NAME)

clean :
	rm $(NAME)


