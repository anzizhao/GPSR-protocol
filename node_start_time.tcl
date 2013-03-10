#! /usr/local/bin/tclsh
# 处理移动的节点的移动时间 ns将在节点移动时开始gpsr


array set arr_node [list]
array set arr_node_end [list]

# 判断数组pos位置是否有数字
proc IsArrayPos {pos} {
    global  arr_node
    if { [array get arr_node  $pos] == ""} {
	return false
    }
    return true 
}


#if { $argc == 0 } then {
#    set filename "simple_1_2.tcl"  

#} elseif { $argc == 1 } {
#    set filename [lindex $argv 1]
#} else {
#    return 0
#}


proc handle_node_time {filename} {
    set status(init)  0
    set status(start) 1
    set status(end)   2
    global arr_node
    global arr_node_end 

    set f [open $filename r ]
  #  puts $filename
    set start_flag  "###movements_start"
    set my_end_flag "###movements_end"

    set flag $status(init) 
    set time_start_pos [string length "\$ns_ at "]

    while { [gets $f line] >= 0 } {
#	puts $line 
	if { [expr $flag-$status(init)] == 0 }   {
#	    puts $line
	    if  { [string compare  -length [string bytelength $start_flag] $start_flag  $line ] == 0 }  {
		set flag $status(start) 
	    } else {
		;
	    }
	    
	} elseif {[expr $flag-$status(start)] == 0 }   {
	  
	    if { [string compare  -length [string bytelength $my_end_flag] $my_end_flag  $line ] == 0 }  {
	#	puts $line 
		set flag $status(end)
		continue

#		puts $line 
	    }
	    #	puts $line 
	    set time 0.0
	    set node_id  0
	    set time_end_pos [string wordend $line $time_start_pos]
	    incr time_end_pos 
	    set time [string range $line $time_start_pos $time_end_pos]
	    #	puts -nonewline $time 
	    set node_start_pos [string first "node_(" $line]
	    set node_start_pos [expr $node_start_pos+[string length "node_("]]
	    set node_end_pos [string first ")" $line ]
	    set node_end_pos [expr $node_end_pos-1]
	    set node_id [string range $line $node_start_pos $node_end_pos]
	    #	puts " $node_id" 
	    
	    if { [IsArrayPos $node_id] == false } { ;# 移动节点第一次出现时间
		set arr_node($node_id) $time 
		set arr_node_end($node_id) $time 
	    } else {                                 # 寻找最后一次出现时间 
		if { $time > $arr_node_end($node_id) } {
		    set arr_node_end($node_id) $time 
		}
	    }
	    
	} else {
	    break 
	}
    
    }
    
    close $f 
    return 0
}


# ####test node time 
#  handle_node_time "./grid_scense.tcl"

# #test result 
# for {set i 0} {$i<99} {incr i} {
#    puts -nonewline $i
#    puts -nonewline " $arr_node($i)"
#    puts " $arr_node_end($i)"
# }

array set flow1 [list]
array set flow2 [list]
array set flow3 [list]
array set flow4 [list]


proc handle_node_pos { filename } {
    
    set status(init)  0
    set status(start) 1
    set status(end)   2
   
    global flow1 flow2 flow3 flow4

  
    set f [open $filename r ]

    set start_flag  "###pos_start"
    set my_end_flag "###pos_end"

    set flag $status(init) 
   
    set x -1
    set y -1
    while { [gets $f line] >= 0 } {
	if { [expr $flag-$status(init)] == 0 }  {

	    if { [string compare -length [string bytelength $start_flag] $start_flag  $line ] == 0 }  {
		set flag $status(start) 
	    }
	    
	   # puts $line
	} elseif {[expr $flag -$status(start)] == 0 }  {

	    if { [string compare -length [string bytelength $my_end_flag] $my_end_flag  $line ] == 0} then  {

		set flag $status(end)
	
	#	puts $line 
		continue
	    }
	    #	puts $line 
	  
	    set node_id  0
#	    set x        -1
#	    set y        -1
	  
	    #	puts -nonewline $time 
	    set node_start_pos [string first "node_(" $line]
	    set node_start_pos [expr $node_start_pos+[string length "node_("]]
	    set node_end_pos [string first ")" $line ]
	    set node_end_pos [expr $node_end_pos-1]
	    #得到id
	    set node_id [string range $line $node_start_pos $node_end_pos]
	  #  puts $node_id
	    #得到（x,y）
	    set node_start_pos [string first "set" $line]
	    set  node_start_pos [expr $node_start_pos + 4 ]
	    set sign [string range $line $node_start_pos $node_start_pos ]

	  #  puts $sign 
	    
	    switch -exact $sign {
		"X" { set x [string range $line [incr node_start_pos 3] [string length $line]]; continue }
		"Y" { set y [string range $line [incr node_start_pos 3] [string length $line]]}
	
		default { continue }
	    }

	    #加入对应的流中 
	    if { $x <= 11 } {
		if { $y >= 450 } {
		    set flow1($node_id) [list $x $y]
		} elseif { $y >= 220 } {
		    set flow2($node_id) [list $x $y]
		}
		
	    } elseif { $x < 210.5 } {
		if { $y >= 210.5 && $y < 410.5 } {
		    set flow3($node_id) [list $x $y]
		}
		
	    } elseif { $x >= 210.5 } {
		if { $y <= 210.5 } {
		    set flow4($node_id) [list $x $y]
		}
		
	    }

	    set x -1
	    set y -1 
	    
	} else {
	    break 
	}
    
    }
    
    close $f 
    
    

}



#puts $i
#set index [array name arr_node]

#puts [llength $index]
#foreach i $index {
#    puts -nonewline $i
#    puts " $arr_node($i)"
#}



#test node pos 
# set total_num 0 
# set node_list [list]
# handle_node_pos "./simple_1_2.tcl"
# puts [array name flow1]
# foreach j [list flow1 flow2 flow3 flow4 ] {

#    set index   [eval array name $j]  
#    set total_num  [expr $total_num + [llength $index]]
#    set node_list [concat $node_list $index]

#    foreach i [eval array name $j] {
# 	puts [eval array  get $j $i]
#    }
# }

# puts -nonewline "total num : "
# puts $total_num 

# puts [lsort $node_list]



#  源流向目标流随机发送n条能到达的目标数据流 数据流为源开始到结束
# 返回匹配的n条流对
# 参数分别为 源流数组 发送节点 目的流数组 流数目 

# total_flow_pair 
#
#

array set flow_pair [list]
proc create_flow_data { src_flow send_nodes dst_flow n } {
    global arr_node arr_node_end 
    global flow_pair 
    array set total_flow_pair [list]
  
    if { [llength $send_nodes] == 0} {
	puts [eval array name $src_flow]
	set send_nodes [eval array name $src_flow]
    }

    set receive_nodes [eval array name $dst_flow]
    puts $src_flow
    puts $send_nodes
    puts $dst_flow
    puts $n 

    set num 0 
    foreach i $send_nodes {
	foreach j $receive_nodes {
	    if { $arr_node($i) == $arr_node($j) } {
		puts [list $i $j]
		set total_flow_pair($num) [list $i $j]
	    } elseif {$arr_node($i) > $arr_node($j) && [expr $arr_node($i)+5] <  $arr_node_end($j) } {
		puts [list $i $j]
		set total_flow_pair($num) [list $i $j]

	    } elseif { $arr_node($i) < $arr_node($j) &&  $arr_node_end($i)
		       > [expr $arr_node($j)+5]	} {
		puts [list $i $j]
		set total_flow_pair($num) [list $i $j]
	    } else {
	    # 不符合要求
	    }

	    incr num 
	    
	} ;# foreach j

    } ;# foreach i 


    if { $num < $n } {
	set n $num 
    }
    
    set index [array name total_flow_pair]
    foreach i $index {
	if { $n == 0 } {
	    break 
	}
	set flow_pair($i) $total_flow_pair($i)
    }
    return flow_pair 
}

###########
proc find_flow_data_1 { src_flow_list send_nodes dst_flow_list n } {
    global arr_node arr_node_end 
  #  global flow_pair 

    array set flow_pair [list]

    array set src_flow $src_flow_list
    array set dst_flow $dst_flow_list 

    array set total_flow_pair [list]
  
    if { [llength $send_nodes] == 0} {
#	puts [array name src_flow]
	set send_nodes [array name src_flow]
    }

    set receive_nodes [array name dst_flow]

   # puts [array get src_flow]
  #  puts $send_nodes
   # puts [array get dst_flow]
   # puts $n 
   # puts $receive_nodes
    set num 0 
    foreach i $send_nodes {
	foreach j $receive_nodes {
	    if { $arr_node($i) == $arr_node($j) } {
	#	puts [list $i $arr_node($i) $arr_node_end($i)  $j $arr_node($j) $arr_node_end($j) ]
		set total_flow_pair($num) [list $i $j]
	    } elseif {$arr_node($i) > $arr_node($j) && [expr $arr_node($i)+5] <  $arr_node_end($j) } {
	
	#	puts [list $i $arr_node($i) $arr_node_end($i)  $j $arr_node($j) $arr_node_end($j) ]
		set total_flow_pair($num) [list $i $j]

	    } elseif { $arr_node($i) < $arr_node($j) &&  $arr_node_end($i)
		       > [expr $arr_node($j)+5]	} {
	
	#	puts [list $i $arr_node($i) $arr_node_end($i)  $j $arr_node($j) $arr_node_end($j) ]
		set total_flow_pair($num) [list $i $j]
	    } else {
	    # 不符合要求
	    }

	    incr num 
	    
	} ;# foreach j

    } ;# foreach i 


    if { $num < $n } {
	set n $num 
    }

   
    set index [array name total_flow_pair]
    foreach i $index {
	if { $n == 0 } {
	    break 
	}
	set flow_pair($i) $total_flow_pair($i)

	incr n -1 
    }

    
  
    return [array get flow_pair] 
}


# #test proper sequence
# handle_node_time "./simple_1_2.tcl"
# handle_node_pos  "./simple_1_2.tcl"

#puts [array name flow1]
#puts [array name flow2]
#set data_flow [find_flow_data_1 [array get flow1] [list] [array get flow2] 3]

#puts $data_flow 
#set index [eval array name $data_flow]
#puts $index 
## foreach  i $index {
#    puts [eval array get $data_flow $i ]
#  #   puts [eval array name $data_flow $i ]
#     puts [lindex [eval array get $data_flow $i] 1]
# }