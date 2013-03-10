
source "../../node_start_time.tcl"
set scene_data_file "./afton_oak_area3.tcl"

# 
#处理节点起始和终结时间 
handle_node_time $scene_data_file



set after_turnon 10.0  
# GPSR routing agent settings
set move_node_num 113
set savefile [ open "node_life.txt"  w ]
for {set i 0} { $i<$move_node_num } { incr i } {
   puts $savefile "$i \t$arr_node($i)\t$arr_node_end($i)"
}
close $savefile 


for {set i 0} {$i < $move_node_num } {incr i} {
   # puts $arr_node($i)
    $ns_ at 1 "$ragent_($i) turnon"
     $ns_ at $opt(nn) "$ragent_($i) turnoff"
}

for {set i 113} {$i < $opt(nn) } {incr i} {
    $ns_ at  5  "$ragent_($i) neighborlist"
    $ns_ at  50  "$ragent_($i) neighborlist"
    $ns_ at  100  "$ragent_($i) neighborlist"
}

# 设计数据流  [send node id ,  rec node id ] 

# 移动节点发送的数据流
set flow_list [list 30 70  23  66  1 123 24 118 ]
set n 0 
foreach {send_node  rec_node}  $flow_list {
    
    set null_($n) [new Agent/Null]
    $ns_ attach-agent $node_($rec_node) $null_($n)

    set udp_($n) [new Agent/UDP]
    $ns_ attach-agent $node_($send_node) $udp_($n)

    set cbr_($n) [new Application/Traffic/CBR]
    $cbr_($n) set packetSize_ 32
    $cbr_($n) set interval_ 2.0
    $cbr_($n) set random_ 1
    #    $cbr_(1) set maxpkts_ 100
    $cbr_($n) attach-agent $udp_($n)
    $ns_ connect $udp_($n) $null_($n)

    $ns_ at [expr $arr_node($send_node) + 5 ]  "$cbr_($n) start"
    $ns_ at [expr $arr_node($send_node) + 35  ]   "$cbr_($n) stop" 

    incr n 
}

set flow_file [ open "flow_pair.txt"  a ]
foreach {send_node  rec_node}  $flow_list {
    puts $flow_file "{$send_node $rec_node}"
}
close $flow_file


# 基点发送的数据流
set basenode_flow_list [ list 117 121 118 127  116 125 123 117 ]
set starttime_list [list 5 10 15 20 ]

foreach {send_node rec_node } $basenode_flow_list starttime $starttime_list {
    set null_($n) [new Agent/Null]
    $ns_ attach-agent $node_($rec_node) $null_($n)

    set udp_($n) [new Agent/UDP]
    $ns_ attach-agent $node_($send_node) $udp_($n)

    set cbr_($n) [new Application/Traffic/CBR]
    $cbr_($n) set packetSize_ 32
    $cbr_($n) set interval_ 2.0
    $cbr_($n) set random_ 1
    #    $cbr_(1) set maxpkts_ 100
    $cbr_($n) attach-agent $udp_($n)
    $ns_ connect $udp_($n) $null_($n)

    $ns_ at $starttime   "$cbr_($n) start"
    $ns_ at [expr $starttime + 30  ]   "$cbr_($n) stop" 

    incr n 

}


set flow_file [ open "flow_pair.txt"  a ]
foreach {send_node  rec_node}  $basenode_flow_list {
    puts $flow_file "{$send_node $rec_node}"
}
close $flow_file







# 偶数节点
#for {set i 0 } { $i < 90 } { incr i } {
# set i 16
#     set send_node  92 
#     set rec_node $i
#     set null_($n) [new Agent/Null]
#     $ns_ attach-agent $node_($rec_node) $null_($n)

#     set udp_($n) [new Agent/UDP]
#     $ns_ attach-agent $node_($send_node) $udp_($n)

#     set cbr_($n) [new Application/Traffic/CBR]
#     $cbr_($n) set packetSize_ 32
#     $cbr_($n) set interval_ 2.0
#     $cbr_($n) set random_ 1
#     #    $cbr_(1) set maxpkts_ 100
#     $cbr_($n) attach-agent $udp_($n)
#     $ns_ connect $udp_($n) $null_($n)

#     $ns_ at [expr $arr_node($i) + 1 ]   "$cbr_($n) start"
#     $ns_ at [expr $arr_node_end($i) - 0.5 ]  "$cbr_($n) stop" 
#     incr n 
    
# }


    # set send_node  93
    # set rec_node 92
    # set null_($n) [new Agent/Null]
    # $ns_ attach-agent $node_($rec_node) $null_($n)

    # set udp_($n) [new Agent/UDP]
    # $ns_ attach-agent $node_($send_node) $udp_($n)

    # set cbr_($n) [new Application/Traffic/CBR]
    # $cbr_($n) set packetSize_ 32
    # $cbr_($n) set interval_ 2.0
    # $cbr_($n) set random_ 1
    # #    $cbr_(1) set maxpkts_ 100
    # $cbr_($n) attach-agent $udp_($n)
    # $ns_ connect $udp_($n) $null_($n)

    # $ns_ at 150   "$cbr_($n) start"
    # $ns_ at $opt(stop)   "$cbr_($n) stop" 
    # incr n 