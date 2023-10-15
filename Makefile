rgba2rle1: rgba2rle1.c codec.c
	$(CC) $? -o $@

rgba2rle: rgba2rle1
	ln -s $< $@

rgba2raw: rgba2rle1
	ln -s $< $@

clean:
	@rm -fv rgba2raw rgba2rle rgba2rle1 README.html

all: rgba2rle1 rgba2rle rgba2raw

README: README.html

.PHONY: all clean README
