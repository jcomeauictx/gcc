SHELL := /bin/bash
all: _doprnt.test arraysnprintf.test
%: %.o ./libiberty.a .FORCE
	gcc $< -o $@ $(word 2, $+)
%.o: %.c .FORCE
	$(MAKE) CFLAGS='-DTEST=1' $@
libiberty.a:
	$(MAKE) libiberty.a
%.test: %
	diff <(./$< 2>&1 >/dev/null) <(./$< 2>/dev/null)
.FORCE:
