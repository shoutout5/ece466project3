            .ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-221  ; decrement stack pointer beyond frame
            SET     R11   , #10          
            ADD     R12   , R5    , #-17  
            ADD     R13   , R12   , #0    
            STR     R11   , R13   , #0    
            SET     R14   , #1           
            ADD     R15   , R5    , #-17  
            ADD     R16   , R15   , #1    
            STR     R14   , R16   , #0    
            ADD     R17   , R5    , #-17  
            ADD     R18   , R5    , #-18  
            STR     R17   , R18   , #0    
            ADD     R19   , R5    , #-18  
            ADD     R20   , R5    , #-19  
            STR     R19   , R20   , #0    
            ADD     R21   , R5    , #-19  
            ADD     R10   , R21   , #0    
            SET     R22   , #0           
            STR     R22   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #221   ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
