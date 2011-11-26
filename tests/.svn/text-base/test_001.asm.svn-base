            .ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-23   ; decrement stack pointer beyond frame
            IN
            ADD     R15   , R0    , #0    
            ADD     R8    , R15   , #0    
            IN
            ADD     R16   , R0    , #0    
            ADD     R9    , R16   , #0    
            IN
            ADD     R17   , R0    , #0    
            ADD     R10   , R17   , #0    
            IN
            ADD     R18   , R0    , #0    
            ADD     R11   , R18   , #0    
            IN
            ADD     R19   , R0    , #0    
            ADD     R12   , R19   , #0    
            IN
            ADD     R20   , R0    , #0    
            ADD     R13   , R20   , #0    
            IN
            ADD     R21   , R0    , #0    
            ADD     R14   , R21   , #0    
            ADD     R22   , R9    , #0    
            ADD     R0    , R22   , #0    
            OUT
            ADD     R23   , R13   , #0    
            ADD     R0    , R23   , #0    
            OUT
            ADD     R24   , R14   , #0    
            ADD     R0    , R24   , #0    
            OUT
            ADD     R25   , R8    , #0    
            ADD     R0    , R25   , #0    
            OUT
            ADD     R26   , R12   , #0    
            ADD     R0    , R26   , #0    
            OUT
            ADD     R27   , R11   , #0    
            ADD     R0    , R27   , #0    
            OUT
            ADD     R28   , R10   , #0    
            ADD     R0    , R28   , #0    
            OUT
            ADD     R29   , R9    , #0    
            ADD     R0    , R29   , #0    
            OUT
            ADD     R30   , R8    , #0    
            ADD     R0    , R30   , #0    
            OUT
            SET     R31   , #0           
            STR     R31   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #23    ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
