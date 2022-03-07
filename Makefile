NAME = bperform

$(NAME) : bperform.c ctrlFuncs.c prepEffects.c signal_events.c splitline.c ac1.c bperform.h
	gcc -Wall -O0 -g -o $(NAME) bperform.c ctrlFuncs.c prepEffects.c signal_events.c splitline.c ac1.c -I./ `pkg-config --libs --cflags gtk+-3.0` -lm  -lasound

install : $(NAME)
	cp ./$(NAME) $(HOME)/bin/$(NAME)

clean :
	rm $(NAME)


