                section         .text

                global          _start
                               
long_qlength:   equ                 128                 
                
_start:

                sub             rsp, 2 * long_qlength * 8
                lea             rdi, [rsp + long_qlength * 8]
                mov             rcx, long_qlength
                call            read_long
                mov             rdi, rsp
                call            read_long
                
                lea             rsi, [rsp + long_qlength * 8]
                sub             rsp, 2 * long_qlength * 8
                mov             r11, rsp
                
                mov             rdi, rsp
                mov             rcx, long_qlength * 2
                call            set_zero
                
                lea             rdi, [rsp + long_qlength * 2 * 8]
                mov             rcx, long_qlength             
                call            multiply_long_long

                mov             rdi, r11
                mov             rcx, long_qlength * 2
                call            write_long

                mov             al, 0x0a
                call            write_char

                jmp             exit

                
; multiplies two long numbers
;    rsi -- address of first long number
;    rdi -- address of second long number
;    rcx -- length of long numbers in qwords
;    r11 -- address of result
; local:
;    r11 -- address of tmp result
;    r12 -- address of accumulator
; result:
;    r11 -- address of result
multiply_long_long:

                push            rsi
                push            rdi
                push            rcx
                
                mov             r13, rsi
                mov             r14, rdi
                mov             r15, rcx
                
                mov             r8, rcx
                imul            r8, 16
                sub             rsp, r8
                mov             r12, rsp
                
                mov             rdi, r12
                mov             r8, r15
                imul            r8, 2
                call            set_zero
                
                mov             r9, r12
                mov             r8, r15
                
                clc
.loop:
; Умножаю 1-ое число на текущий разряд 2-ого.
                mov             rbx, [r14]
                add             r14, 8
                mov             rdi, r13
                mov             r10, r9
                mov             rcx, r15
                call            mul_long_short
 
; Результат умножения мог вылезти за длину числа, поэтому не забываю прибавить этот остаток.
                mov             rax, [r9 + r15 * 8]
                mov             rbx, [r9 + r15 * 8 + 8]
                add             rax, rsi
                adc             rbx, 0
                mov             [r9 + r15 * 8], rax
                mov             [r9 + r15 * 8 + 8], rbx

; Складываю accumulator и текущий результат.
                mov             rdi, r11
                mov             rsi, r12
                mov             rcx, r15
                imul            rcx, 2
                call            add_long_long
 
; Зануляю младший разряд accumulator и сдвигаю адрес, имитируя сдвиг на длину разряда при умножении.
                xor             rax, rax
                mov             [r9], rax
                add             r9, 8
                
                dec             r8
                jnz             .loop

                imul            r15, 16
                add             rsp, r15
                
                pop             rcx
                pop             rdi
                pop             rsi
                ret               
                
; adds two long number
;    rdi -- address of long number #1
;    rsi -- address of long number #2
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to rdi
add_long_long:
                push            rdi
                push            rsi
                push            rcx

                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                adc             [rdi], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rsi
                pop             rdi
                ret
                
; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
;    r10 -- address of result
; result:
;    product is written to r10
mul_long_short:
                push            r10
                push            rax
                push            rdi
                push            rcx

                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [r10], rax
                add             r10, 8
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rdi
                pop             rax
                pop             r10
                ret

; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                mov             r10, rdi
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata               
invalid_char_msg:               db                 "Invalid character: "
invalid_char_msg_size:          equ             $ - invalid_char_msg
