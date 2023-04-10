BOOKSHELF = BOOKSHELF
DSV = bin/dsv


.PHONY:	all dsv


all:
	@echo "This is just a placeholder for now" 1>&2
	@exit 1

dsv:
	$(DSV) setup $(BOOKSHELF)
