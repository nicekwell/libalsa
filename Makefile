all: play capture
.PHONY: all

play: play.c tinyalsa/*
	gcc play.c tinyalsa/* -Iinclude -o play

capture: capture.c tinyalsa/*
	gcc capture.c tinyalsa/* -Iinclude -o capture

clean:
	rm play capture
