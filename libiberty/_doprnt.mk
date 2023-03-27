SHELL := /bin/bash
all: _doprnt.test arraysnprintf.test
%.o: %.c .FORCE
	$(MAKE) CFLAGS='-DTEST=1' $@
%: %.o ./libiberty.a .FORCE
	gcc $< -o $@ $(word 2, $+)
libiberty.a:
	$(MAKE) libiberty.a
%.test: %
	diff <(./$< 2>&1 >/dev/null) <(./$< 2>/dev/null)
.FORCE:
