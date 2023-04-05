SHELL := /bin/bash
all: _doprnt.o _arraysnprintf.o _doprnt.test _arraysnprintf _arraysnprintf.test
%.o: %.c .FORCE
	$(MAKE) CFLAGS='-DTEST=1 -DDISABLE_LONG_LONGS' $@
%: %.o ./libiberty.a .FORCE
	gcc $< -o $@ $(word 2, $+)
libiberty.a:
	$(MAKE) libiberty.a
%.test: %
	diff <(./$< 2>/dev/null) <(./$< 2>&1 >/dev/null)
%.cpp: %.c
	# show preprocessor output
	gcc -E -I. -I../include $< | less
.FORCE:
