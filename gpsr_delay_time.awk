#根据时间段
#计算端到端延时
BEGIN{
     seg_total_delay["s"] = 0
     data_num["d"] = 0
     start_time["s"] = 0
     end_time["e"] = 0

     # 需要变化的
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
  


    count_delay()


}
END{
   # print_odd_even()
    print_result()
}


function init_segmentation_arr()
{
    
     
     for (i=0; i<segmentation; i++)
     {
         seg_total_delay[i] = 0
          data_num[i] = 0;
          

     }
     

}

function count_delay()
{
     if ( flag == "M" )
          return false 
     if ( layer != "AGT" )
          return false

     tag = sprintf("%d_%d_%d", src, dst, uid)
     if ( flag == "s" ) {
          start_time[tag] = time 
          
     } else if ( flag == "r" )
     {
          # start_time存在相关记录
          if ( tag in start_time )
          {
              end_time[tag] = time
               start_pos = int(start_time[tag]/step_time)
               end_pos = int(end_time[tag]/step_time)
              # printf ("%d %d\n", start_pos, end_pos)
             #  print (start_time[tag], start_pos, end_time[tag], end_pos)
               if ( end_pos > segmentation )
               {
                    print $0
                    return false 
               }
               
               #数据收发在分割时间端内
               if ( start_pos == end_pos )
               {
                    seg_total_delay[start_pos] += (end_time[tag] - start_time[tag])
                    data_num[start_pos] ++;
                                        
               }else {
                    seg_total_delay[start_pos] += (end_pos*step_time-start_time[tag])
                    data_num[start_pos] ++
                    seg_total_delay[end_pos] += (end_time[tag]- end_pos*step_time)
                    data_num[end_pos] ++
                    
               }
               
          }
          
     }
     
    
     
}


function print_result()
{
     for (i=0; i<segmentation; i ++ )
     {
         if ( data_num[i] == 0 && seg_total_delay[i] == 0)
             data_num[i] = 1;
         avg_delay_time = seg_total_delay[i]/data_num[i];
       #  printf ( "%.3f\n", seg_total_delay[i])
         printf ("%d %.3f\n", i*step_time, avg_delay_time )
     }
     
}



