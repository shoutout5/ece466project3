            .ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-320  ; decrement stack pointer beyond frame
            ADD     R12   , R5    , #-116 
            ADD     R9    , R12   , #0    
            IN
            ADD     R13   , R0    , #0    
            ADD     R11   , R13   , #0    
            ADD     R14   , R9    , #0    
            ADD     R0    , R14   , #0    
            OUT
            SET     R15   , #0           
            ADD     R8    , R15   , #0    
        L2: ADD     R16   , R8    , #0     ; enter for loop
            SET     R17   , #100         
            SUB     R18   , R16   , R17   
            BRn     R18   , L5          
            SET     R18   , #0           
        L5: NOP                            ; less-than label
            BRz     R18   , L4          
            ADD     R19   , R8    , #0    
            ADD     R10   , R19   , #0    
            ADD     R20   , R10   , #0    
            SET     R21   , #1           
            ADD     R22   , R20   , R21   
            ADD     R10   , R22   , #0    
            ADD     R23   , R8    , #0    
            ADD     R24   , R11   , #0    
            ADD     R25   , R23   , R24   
            ADD     R26   , R9    , #0    
            STR     R25   , R26   , #0    
        L3: ADD     R27   , R8    , #0    
            SET     R28   , #1           
            ADD     R29   , R27   , R28   
            ADD     R8    , R29   , #0    
            ADD     R30   , R9    , #0    
            SET     R31   , #1           
            MUL     R30   , R30   , #1    
            ADD     R32   , R30   , R31   
            ADD     R9    , R32   , #0    
            BRA     L2                     ; backedge of for-loop
        L4: NOP                           
            ADD     R33   , R5    , #-116 
            SET     R34   , #50          
            MUL     R35   , R34   , #1    
            ADD     R36   , R33   , R35   
            LDR     R37   , R36   , #0    
            ADD     R0    , R37   , #0    
            OUT
            SET     R38   , #0           
            STR     R38   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #320   ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
