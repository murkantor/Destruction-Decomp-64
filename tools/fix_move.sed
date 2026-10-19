# KMC's old assembler encodes "move" as addu; modern gas uses or.
s/^([[:space:]]+)move[[:space:]]+(\$[0-9a-z]+),(\$[0-9a-z]+)/\1addu\t\2,\3,$0/
