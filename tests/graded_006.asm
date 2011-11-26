			.ORIG x2000
            JSR     main                  
            HALT
      main: ADD     R6    , R6    , #-1    ; reserve space for the return value
            ADD     R6    , R6    , #-1   
            STR     R7    , R6    , #0     ; save caller's return address
            ADD     R6    , R6    , #-1   
            STR     R5    , R6    , #0     ; save caller's frame pointer
            ADD     R5    , R6    , #-1    ; set up frame pointer
            ADD     R6    , R6    , #-205  ; decrement stack pointer beyond frame
            SET     R8    , #10          
            ADD     R9    , R5    , #-1   
            ADD     R10   , R9    , #0    
            STR     R8    , R10   , #0    
            SET     R11   , #1           
            ADD     R12   , R5    , #-1   
            ADD     R13   , R12   , #1
			BRp     R13   , L4    
		L3: ADD     R50   , R50   , #1
	    	LDR		R51	  , R50   , #10
			ADD     R51   , R50   , #2
	   	L4:	ADD		R52   , R50   , #3
			STR		R53   , R51	  , #5
			ADD     R53   , R52   , #6
			SET     R50   , #2
			BRnz	R50	  , L3
			IN
			ADD     R56   , R0    , #0
		L2: ADD     R50   , R56   , #1
	    	ADD		R51	  , R50   , #10
			ADD     R52   , R51   , #2
			SUB		R53   , R52   , #3
			LDR     R60   , R53   , #4
			ADD		R54   , R53	  , #5
			STR     R68   , R45   , #1 
			ADD     R56   , R54   , #2
			SUB     R63   , R50   , #150
			BRp		R63	  , L2
			SET     R0    , #0
			ADD     R0    , R50   , #0
			OUT
			SET     R0    , #0
			ADD     R0    , R51   , #0
			OUT           
 			SET     R0    , #0
			ADD     R0    , R52   , #0
			OUT           
			SET     R0    , #0
			ADD     R0    , R53   , #0
			OUT           
			SET     R0    , #0
			ADD     R0    , R54   , #0
			OUT
			SET     R0    , #0
			ADD     R0    , R56   , #0
			OUT
			STR     R11   , R13   , #0    
            ADD     R14   , R5    , #-1   
            ADD     R15   , R5    , #-2   
            STR     R14   , R15   , #0    
            ADD     R16   , R5    , #-2   
            ADD     R17   , R5    , #-3   
            STR     R16   , R17   , #0    
            ADD     R18   , R5    , #-3   
            ADD     R19   , R5    , #-4   
            STR     R18   , R19   , #0    
            SET     R20   , #0           
            STR     R20   , R5    , #3    
            BRA     L1                    
        L1: ADD     R6    , R6    , #205   ; remove locals from stack
            LDR     R5    , R6    , #0     ; restore caller's frame pointer
            ADD     R6    , R6    , #1    
            LDR     R7    , R6    , #0     ; restore caller's return address
            ADD     R6    , R6    , #1    
            RET                            ; return to caller
			.END
