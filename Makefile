play: play.c tinyalsa/*
	gcc play.c tinyalsa/* -Iinclude -o play

clean:
	rm play
