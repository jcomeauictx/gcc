_doprnt: _doprnt.o ./libiberty.a .FORCE
	gcc $< -o $@ $(word 2, $+)
_doprnt.o: .FORCE
	$(MAKE) CFLAGS='-DTEST=1' _doprnt.o
libiberty.a:
	$(MAKE) libiberty.a
.FORCE:
