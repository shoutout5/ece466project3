L10:	    ADD     R36   , R5    , #-3   
            LDR     R37   , R36   , #0    
            ADD     R38   , R5    , #0    
            LDR     R39   , R38   , #0    
            MUL     R37   , R37   , #1    
            ADD     R40   , R37   , R39   
            LDR     R41   , R40   , #0    
            ADD     R42   , R5    , #-1   
            LDR     R43   , R42   , #0    
            MUL     R41   , R41   , #1    
            ADD     R44   , R41   , R43   
            LDR     R45   , R44   , #0    
            ADD     R46   , R5    , #4    
            LDR     R47   , R46   , #0    
            ADD     R48   , R5    , #0    
            LDR     R49   , R48   , #0    
            MUL     R47   , R47   , #1    
            ADD     R50   , R47   , R49   
            LDR     R51   , R50   , #0    
            ADD     R52   , R5    , #-2   
            LDR     R53   , R52   , #0    
            MUL     R51   , R51   , #1    
            ADD     R54   , R51   , R53   
            LDR     R55   , R54   , #0    
            ADD     R56   , R5    , #5    
            LDR     R57   , R56   , #0    
            ADD     R58   , R5    , #-2   
            LDR     R59   , R58   , #0    
            MUL     R57   , R57   , #1    
            ADD     R60   , R57   , R59   
            LDR     R61   , R60   , #0    
            ADD     R62   , R5    , #-1   
            LDR     R63   , R62   , #0    
            MUL     R61   , R61   , #1    
            ADD     R64   , R61   , R63   
            LDR     R65   , R64   , #0    
            MUL     R66   , R55   , R65   
            ADD     R67   , R45   , R66   
            ADD     R68   , R5    , #-3   
            LDR     R69   , R68   , #0    
            ADD     R70   , R5    , #0    
            LDR     R71   , R70   , #0    
            MUL     R69   , R69   , #1    
            ADD     R72   , R69   , R71   
            LDR     R73   , R72   , #0    
            ADD     R74   , R5    , #-1   
            LDR     R75   , R74   , #0    
            MUL     R73   , R73   , #1    
            ADD     R76   , R73   , R75   
            STR     R67   , R76   , #0    
            ADD     R77   , R5    , #-2   
            LDR     R78   , R77   , #0    
            SET     R79   , #1           
            ADD     R80   , R78   , R79   
            ADD     R81   , R5    , #-2   
            STR     R80   , R81   , #0    
            ADD     R32   , R5    , #-2    
            LDR     R33   , R32   , #0    
            SET     R34   , #100         
            SUB     R35   , R33   , R34   
            BRn     R35   , L10         

 