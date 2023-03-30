break main
printf "To call subroutine, type 'n'\n"
printf "To advance one machine instruction, type 'si'\n"
printf "To set a register, e.g: set $eax = 0x55\n"
display/i $pc
display/x $rsp
display/x $rbp
display/x $rdi
display/x $rsi
display/x $rdx
display/x $rcx
display/x $rbx
display/x $rax
run
