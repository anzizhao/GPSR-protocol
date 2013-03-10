# 计算特别的数据流集 收发送数据包情况
# 处理数据集的时候 需要read_flow_file 和 handle_special_flow
# 处理单个数据流的时候 需要 handle_single
BEGIN{
    rec_n["r"] = 0;
    send_n["s"] = 0;
    drop_n["d"] = 0;
    forward_n["f"] = 0;
    total_n["t"] = 0;
    
    src_node_arr["src"] = 0  ;
    dst_node_arr["dst"] = 0 ;

    send_time["st"] = 0 
    
    flow_num = 51
    # 时间限制 time [min_time max_time]
    min_time = 0
    max_time = 100000000

    # 初始化src 和 dst 数组
    # 1. 
    # init_recode_array();
    # 2. 初始化指定节点对 文件给出
    #    文件格式每一行一对:
    #                     { send_node  recv_node }
     read_flow_file("./flow_pair.txt")
    # 3. 初始化基数和偶数节点对
   # init_odd_even_arr();
    
#追踪的数据包id
    uid = 0

}

{
    flag = $1;
	time = $2;
	temp = length($3)
	node = int(substr($3, 2, temp-2))
	layer = $4;
	pid = $6;	
	proto = $7;
	src_mac = $10;
	dst_mac = $11;

   
    temp = index ($14, ":");
	src = int(substr($14,2,temp))
    temp = index ($15, ":");
	dst = int(substr($15, 1, temp));
  


    # 处理指定节点对
   handle_sepcial_flow()
    # 处理基数偶数数据对
  #  handle_odd_even()


}
END{
   # print_odd_even()
    print_special_pair()
}



function _print_pair(j)
{
    split(j, temps,"_");
    s = temps[1]
    r = temps[2]
    printf ( " %d --> %d \n" , s, r )
    printf ( "  send packet : %d\n", send_n[j] );
    printf ( "  receive packet : %d\n", rec_n[j] );
    printf ( "  forward packet : %d\n", forward_n[j])
    printf ( "  drop  packet : %d\n", drop_n[j] );
  #  printf ( "  total packet : %d\n", total_n[j]);
}


#################################### 指定节点对

function print_special_pair ()
{
    for ( i=0; i<flow_num ; i++ )
    {
        flow_i = sprintf ("%d_%d", src_node_arr[i], dst_node_arr[i] )
        _print_pair(flow_i)
    }
    
}



#### 处理函数
#  查看记录是否符合要求 是的话 处理
#读入flow_pair.txt文件 记录于数组src_node_arr 和dst_node_arr 
function read_flow_file ( filename ) {
    i = 0 
    while ( getline str < filename ){
    #    printf ( "%s\n", str )
        s_pos = index (str, "{")
        e_pos = index (str, "}")
        s_pos ++ 
    #    printf ( "%d %d\n", s_pos, e_pos )
        temp = substr(str, s_pos, e_pos-s_pos )
     #   printf ( "%s\n", temp )
        split(temp, temps)
        
        src_node_arr[i] = temps[1]
        dst_node_arr[i] = temps[2]
     #   printf ("%s %s\n", temps[1], temps[2]);

        tmp = sprintf ("%d_%d", src_node_arr[i], dst_node_arr[i])
        # 初始化节点对记录数组
        send_n[tmp] = 0;
        rec_n[tmp] = 0;
        drop_n[tmp] = 0;
        forward[tmp] = 0;
        total_n[tmp] = 0;
        i ++ 
    }
#    print i 
#    printf ("%d\n", i )
    flow_num = i 

}

function handle_sepcial_flow () {

    if ( flag == "M" )
        return false 
    
    for ( i=0; i < flow_num ; i++ )
    {
        if ( src == src_node_arr[i]  &&  dst == dst_node_arr[i] )
        {
            break ;
        }
        

    }
 
    if ( i == flow_num )
        return false 

 #   print $0
    flow_i = sprintf ("%d_%d", src, dst)
    if ( flag == "D" )
    {
        drop_n[flow_i] ++;
        total_n[flow_i] ++;
        print $0
  
    }
    
    if ( layer == "AGT"  && (node == src || node == dst) && min_time < time && time < max_time )
    {
        total_n[flow_i] ++;
        if ( flag == "r" ) {
            rec_n[flow_i] ++;

        }
        else if ( flag == "s" )
            send_n[flow_i] ++;
        else if (flag == "f" )
            forward_n[flow_i] ++
        else 
	    return false 
#        print $0

#        print $0
        return true 
        
    } #if 
    return false 
}

