            .ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-20   ; decrement stack pointer beyond frame
            ADD     R12   , R5    , #-16  
            ADD     R13   , R5    , #-17  
            STR     R12   , R13   , #0    
            ADD     R14   , R5    , #-17  
            ADD     R15   , R5    , #-18  
            STR     R14   , R15   , #0    
            ADD     R16   , R5    , #-18  
            ADD     R11   , R16   , #0    
            SET     R17   , #10          
            ADD     R18   , R11   , #0    
            LDR     R19   , R18   , #0    
            LDR     R20   , R19   , #0    
            STR     R17   , R20   , #0    
            ADD     R21   , R5    , #-16  
            LDR     R22   , R21   , #0    
            ADD     R0    , R22   , #0    
            OUT
            SET     R23   , #0           
            STR     R23   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #20    ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
