NAME = bperform

$(NAME) : bperform.c prepEffects.c signal_events.c splitline.c ac1.c bperform.h voicePage.h voicePage.c effectStrip.c effectStrip.h splitline.h
	gcc -Wall -O0 -g -o $(NAME) bperform.c prepEffects.c signal_events.c splitline.c ac1.c voicePage.c effectStrip.c -I./ `pkg-config --libs --cflags gtk+-3.0` -lm  -lasound

install : $(NAME)
	cp ./$(NAME) $(HOME)/bin/$(NAME)

clean :
	rm $(NAME)


