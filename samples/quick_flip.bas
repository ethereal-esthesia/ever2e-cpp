10 hgr : hcolor = 7
20 for x = 0 to 69 step 4 : for y = 0 to 191 : hplot x, y : next y, x
30 for x = 71 to 139 step 4 : for y = 0 to 191 : hplot x, y : next y, x
40 hcolor = 3
50 for x = 141 to 209 step 4 : for y = 0 to 191 : hplot x, y : next y, x
60 for x = 212 to 279 step 4 : for y = 0 to 191 : hplot x, y : next y, x
70 hgr2
80 poke 49236, 0 : poke 49237, 0 : goto 80
run
