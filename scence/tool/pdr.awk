#  是根据旧无线格式统计
#根据时间段
#计算丢包率
BEGIN{
    rec_n["r"]=0
    send_n["s"]=0

     # 需要变化的
#   模拟时间 和 分段数
     total_time = 300
     segmentation = 6

  
     init_segmentation_arr()
     step_time = total_time/segmentation
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
  


    count_packet_num()


}
END{
   # print_odd_even()
    print_result()
}


function init_segmentation_arr()
{
    
     
     for (i=0; i<segmentation; i++)
     {
         send_n[i] = 0 
         rec_n[i] = 0

     }
     

}

function count_packet_num()
{
     if ( flag == "M" )
          return false 
     if ( layer != "AGT" )
          return false
     pos = int(time/step_time)
    
     if ( flag == "s" ) {
         send_n[pos] ++;
          
     } else if ( flag == "r" )
     {
         rec_n[pos]++;

          }
          
     
     
    
     
}


function print_result()
{
     for (i=0; i<segmentation; i ++ )
     {
         if ( send_n[i] == 0 && rec_n[i] == 0 )
         {
             send_n[i] = 1
             rec_n[i] = 1
         }
         
      #   print ( rec_n[i], send_n[i])
         printf ("%d %.2f\n", i*step_time, (send_n[i]-rec_n[i])/send_n[i] )
     }
     
}



