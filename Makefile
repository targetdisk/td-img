CFLAGS ?= -Wall -std=c99

### TARGETS ####################################################################

rgba2rle1: rgba2rle1.c codec.c
	$(CC) $(CFLAGS) $? -o $@

rgba2rle: rgba2rle1
	ln -s $< $@

rgba2raw: rgba2rle1
	ln -s $< $@

### README #####################################################################

UNAME = $(shell uname)
ifeq ($(UNAME),Linux)
	OPEN=xdg-open
endif

pub.css:
	wget https://github.com/manuelp/pandoc-stylesheet/raw/acac36b976966f76544176161ba826d519b6f40c/pub.css

# Requires Pandoc to be installed
README.html: README.md pub.css
	pandoc $< -s -c pub.css -o README.html
	$(OPEN) README.html

README: README.html

### ETC ########################################################################

clean:
	@rm -fv rgba2raw rgba2rle rgba2rle1 README.html

all: rgba2rle1 rgba2rle rgba2raw

.PHONY: all clean README
