            .ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-19   ; decrement stack pointer beyond frame
            SET     R11   , #10          
            ADD     R8    , R11   , #0    
            SET     R12   , #20          
            ADD     R9    , R12   , #0    
            SET     R13   , #30          
            ADD     R10   , R13   , #0    
            ADD     R14   , R8    , #0    
            SET     R15   , #10          
            SUB     R16   , R14   , R15   
            NOTL    R17   , R16          
            BRz     R17   , L3           ; if-then-else branch, fall thru to then-stmts
            ADD     R18   , R9    , #0    
            SET     R19   , #10          
            SUB     R20   , R18   , R19   
            ADD     R9    , R20   , #0    
            BRA     L2                    
        L3: NOP                            ; else label
        L2: NOP                            ; if-then-else label
            ADD     R21   , R9    , #0    
            SET     R22   , #10          
            SUB     R23   , R21   , R22   
            NOTL    R24   , R23          
            BRz     R24   , L5           ; if-then-else branch, fall thru to then-stmts
            ADD     R25   , R10   , #0    
            SET     R26   , #10          
            SUB     R27   , R25   , R26   
            ADD     R10   , R27   , #0    
            BRA     L4                    
        L5: NOP                            ; else label
        L4: NOP                            ; if-then-else label
            SET     R28   , #10          
            ADD     R10   , R28   , #0    
            BRz     R10   , L7           ; if-then-else branch, fall thru to then-stmts
            SET     R29   , #10          
            ADD     R8    , R29   , #0    
            BRA     L6                    
        L7: NOP                            ; else label
        L6: NOP                            ; if-then-else label
            ADD     R30   , R8    , #0    
            ADD     R0    , R30   , #0    
            OUT
            SET     R31   , #1           
            NOT     R32   , R31          
            ADD     R32   , R32   , #1    
            STR     R32   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #19    ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
