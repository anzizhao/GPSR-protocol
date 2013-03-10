# 现在增加了12个不移动的节点
# 位移了 (497, 403)
set x_m 497
set y_m 403

set bnode  113 
$node_($bnode) set X_ [expr 735 - $x_m ]
$node_($bnode) set Y_ [expr 1500 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"


incr bnode 
$node_($bnode) set X_  [expr 765 - $x_m ]
$node_($bnode) set Y_ [expr 1200.00 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"



# 115
incr bnode 
$node_($bnode) set X_  [expr 665 - $x_m ]
$node_($bnode) set Y_ [expr 1100.00 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"



#
incr bnode 
$node_($bnode) set X_  [expr 645 - $x_m ]
$node_($bnode) set Y_ [expr 950.00 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"



incr bnode 
$node_($bnode) set X_  [expr 745 - $x_m ]
$node_($bnode) set Y_ [expr 850.00 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"



#118
incr bnode
$node_($bnode) set X_  [expr 800.00 - $x_m ]
$node_($bnode) set Y_ [expr 1340.00 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"

incr bnode
$node_($bnode) set X_ [expr 994.00 - $x_m ]
$node_($bnode) set Y_ [expr  1370.00- $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"

incr bnode

$node_($bnode) set X_ [expr 1180.00 - $x_m ]
$node_($bnode) set Y_ [expr 1370 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"

incr bnode
$node_($bnode) set X_ [expr 1370.00 - $x_m ]
$node_($bnode) set Y_ [expr 1370  - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"


# 119
incr bnode
$node_($bnode) set X_ [expr 965.00 - $x_m ]
$node_($bnode) set Y_ [expr 1232 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"


incr bnode
$node_($bnode) set X_  [expr 1000 - $x_m ]
$node_($bnode) set Y_ [expr 1134  - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"

incr bnode
$node_($bnode) set X_ [expr 1100.00 - $x_m ]
$node_($bnode) set Y_ [expr 1000 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"

incr bnode
$node_($bnode) set X_  [expr 1200.00 - $x_m ]
$node_($bnode) set Y_ [expr 900 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"


incr bnode
$node_($bnode) set X_  [expr 1300.00 - $x_m ]
$node_($bnode) set Y_ [expr 800 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"

incr bnode
$node_($bnode) set X_  [expr 1387.00 - $x_m ]
$node_($bnode) set Y_ [expr 694 - $y_m]
$node_($bnode) set Z_ 0.0
$ns_ at 0.001 "$node_($bnode) start"
