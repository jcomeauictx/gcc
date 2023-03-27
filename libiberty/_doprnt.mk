SHELL := /bin/bash
_doprnt: _doprnt.o ./libiberty.a .FORCE
	gcc $< -o $@ $(word 2, $+)
_doprnt.o: .FORCE
	$(MAKE) CFLAGS='-DTEST=1' _doprnt.o
libiberty.a:
	$(MAKE) libiberty.a
test: _doprnt
	diff <(./$< 2>&1 >/dev/null) <(./$< 2>/dev/null)
.FORCE:
