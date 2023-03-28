SHELL := /bin/bash
all: _doprnt.o _arraysnprintf.o _doprnt.test _arraysnprintf _arraysnprintf.test
%.o: %.c .FORCE
	$(MAKE) CFLAGS='-DTEST=1' $@
%: %.o ./libiberty.a .FORCE
	gcc $< -o $@ $(word 2, $+)
libiberty.a:
	$(MAKE) libiberty.a
%.test: %
	diff <(./$< 2>&1 >/dev/null) <(./$< 2>/dev/null)
.FORCE:
