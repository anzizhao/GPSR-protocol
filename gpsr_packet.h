/* -*- Mode:C++; c-basic-offset: 2; tab-width:2; indent-tabs-width:t -*- 
 * Copyright (C) 2005 State University of New York, at Binghamton
 * All rights reserved.
 *
 * NOTICE: This software is provided "as is", without any warranty,
 * including any implied warranty for merchantability or fitness for a
 * particular purpose.  Under no circumstances shall SUNY Binghamton
 * or its faculty, staff, students or agents be liable for any use of,
 * misuse of, or inability to use this software, including incidental
 * and consequential damages.

 * License is hereby given to use, modify, and redistribute this
 * software, in whole or in part, for any commercial or non-commercial
 * purpose, provided that the user agrees to the terms of this
 * copyright notice, including disclaimer of warranty, and provided
 * that this copyright notice, including disclaimer of warranty, is
 * preserved in the source code and documentation of anything derived
 * from this software.  Any redistributor of this software or anything
 * derived from this software assumes responsibility for ensuring that
 * any parties to whom such a redistribution is made are fully aware of
 * the terms of this license and disclaimer.
 *
 * Author: Ke Liu, CS Dept., State University of New York, at Binghamton 
 * October, 2005
 *
 * GPSR code for NS2 version 2.26 or later
 * Note: this implementation of GPSR is different from its original 
 *       version wich implemented by Brad Karp, Harvard Univ. 1999
 *       It is not guaranteed precise implementation of the GPSR design
 */

/* gpsr_packet.h : the definition the gpsr routing protocol packet header
 *
 * hdr_hello : hello message header, broadcast to one hop neighbors
 * hdr_query : query message header, flood the data sink info into networks 
 * hdr_data  : not a really data packet header, it is just used to 
 *             be attached to any data packet created by higher layer apps
 *             The reason to use this type of header is decided by the 
 *             design of the GPSR: each data packet has to carry the 
 *             location info of its data sink, maintain the routing protocol
 *             stateless.
 */

#ifndef GPSR_PACKET_H_
#define GPSR_PACKET_H_

#include "packet.h"
#include <math.h>
#include <cassert> 

using namespace std; 




#define SINK_TRACE_FILE "sink_trace.tr"
#define NB_TRACE_FILE "gpsrnb_trace.tr"

#define GPSR_CURRENT Scheduler::instance().clock()
#define INFINITE_DELAY 5000000000000.0

#define GPSRTYPE_HELLO  0x01   //hello msg
#define GPSRTYPE_QUERY_REQUEST  0x02   //query msg from the sink
#define GPSRTYPE_QUERY_ASK  0x04     // ask the query from the destination 
#define GPSRTYPE_DATA   0x08   //the CBR data msg

// #define GPSR_MODE_GF    0x01   //greedy forwarding mode
// //#define GPSR_MODE_PERI  0x02   //perimeter routing mode
// #define GPSR_MODE_FLOOD  0x02   //location flood  routing mode

#define GPSR_RQTYPE_DST  0x01      // 请求目的地址
#define  GPSR_RQTYPE_OUT  0x02    // 请求跳出空洞

#define GPSR_TFTYPE_GF    0x01      // 贪婪模式
#define GPSR_TFTYPE_PF    0x02      // part flood  跳出空洞  

#define HDR_GPSR(p)   ((struct hdr_gpsr*)hdr_gpsr::access(p))
#define HDR_GPSR_HELLO(p) ((struct hdr_gpsr_hello*)hdr_gpsr::access(p))
#define HDR_GPSR_QUERY_REQUEST(p) ((struct hdr_gpsr_query*)hdr_gpsr::access(p))
#define HDR_GPSR_QUERY_ASK(p) ((struct hdr_gpsr_ask*)hdr_gpsr::access(p))

#define HDR_GPSR_DATA(p) ((struct hdr_gpsr_data*)hdr_gpsr::access(p))


struct hdr_gpsr {
  u_int8_t type_;
  
  static int offset_;
  inline static int& offset() {return offset_;}
  inline static struct hdr_gpsr* access(const Packet *p){
    return (struct hdr_gpsr*) p->access(offset_);
  }
};


//  anzi design 

//  the geo info        
struct node_info {
	nsaddr_t id_; 
	float x_;               
	float y_;
};

struct packet_info {
	node_info node_;
	unsigned int seqno_;
	float ts_; 
};

// 两个地址 第一个终点地址， 第二个是下一个可达到的地址
struct  dst_list {
	unsigned int num_ ;                                                 // 地址数3
	node_info  storage_[2]; 

	unsigned size () const 
	{
		 unsigned  sz = 
			 1 *sizeof(unsigned int ) +
			 2 * sizeof(node_info ) ;
		 return sz; 
	}
	void add ( const struct node_info & info ) 
	{
		if ( num_ == 0 ) {                                           // 第一项
			storage_[0] = info ;
			num_ ++; 
		} 
		else if ( num_ == 1  ) {
			storage_[ 1 ] = info ;
			num_ ++; 
		} 
		else if ( num_ > 1 ) {
			storage_[1] = info; 
		}
		else
			assert(false) ;
		
	
	}
	// 弹出最后一个数据
	void pop ()
	{
			if ( num_ ) { 
				// 有目的地址
				num_ --;
			} 
	}
	bool isFinalDst () const 
	{
		return ( num_ == 1 )? true : false; 
	}
	bool empty () const 
	{
		return ( ! num_ )? true : false ; 
	}

	node_info & nextdst ()  {
		if ( empty() ) {
			return storage_[0];
		} else {
			return storage_[num_-1]; 
		}
	}

	node_info & top () {  return nextdst() ; }
};


struct hdr_gpsr_query {
	u_int8_t type_;
	u_int8_t query_type_;                 //  请求类型  目标地址 或 跳出空洞
	int hops_;                                   // 跳数
	struct packet_info base_;                     // 数据包的基本信息 the geo info, time, and seqno...

	struct node_info  dst_;                        //    请求的目标id 跳出空洞模式时 loc信息用于判断到目的节点是否更近

	inline int size() const 
	{
    int sz =
      2*sizeof(u_int8_t) +
			1*sizeof(node_info) +
			1*sizeof(packet_info) ; 
		return sz;
  }
};


struct hdr_gpsr_data {
  u_int8_t type_;
  u_int8_t mode_;                          // GPSR_TFTYPE_GF  GPSR_TFTYPE_TF
  int hops_;
  struct packet_info base_;     
  struct dst_list  dstgroup_;                     // 目的地址
  struct dst_list  srcgroup_;                     // 记录上一地址，用于最短路径时比较
	inline int size() const 
	{
    int sz =
      2*sizeof(u_int8_t) +
      sizeof(packet_info) +
			dstgroup_.size() + srcgroup_.size()	;
    return sz;
  }
};

struct hdr_gpsr_ask {
	u_int8_t type_;                           // 数据包类型
	u_int8_t mode_;                       // GPSR_TFTYPE_GF  GPSR_TFTYPE_TF
	u_int8_t query_type_;              //  请求类型  目标地址 或 跳出空洞
	int hops_;
	nsaddr_t ask_for_id_;                // 标记发送请求询问的目的地  
	struct packet_info  base_; 
	struct packet_info  ask_info_ ;             // 响应请求信息  为目的节点信息 
	dst_list  dstgroup_;                              // 源节点被第一压入   
	struct dst_list  srcgroup_;                     // 记录上一地址，用于最短路径时比较
	inline int size ()  const 
	{
		int sz = 
			3*sizeof(u_int8_t) + 
			1*sizeof( int  ) + 
			2*sizeof( packet_info ) + 
			dstgroup_.size() + srcgroup_.size() ; 		
		return sz; 
	}
};

struct hdr_gpsr_askdata_common{
private:
	struct hdr_gpsr_ask *gah_;
	struct hdr_gpsr_data *gdh_ ; 
public:
	hdr_gpsr_askdata_common( Packet *p ) {
		struct hdr_gpsr *gh = HDR_GPSR(p);
		if ( gh->type_ == GPSRTYPE_QUERY_ASK  ) {
			 gah_ = HDR_GPSR_QUERY_ASK(p);
			 gdh_ = NULL;
		}
		else { 
			gah_ = NULL;
			gdh_ = HDR_GPSR_DATA(p);
		}
	}
	~hdr_gpsr_askdata_common () { gah_ = NULL; gdh_  = NULL; }
	u_int8_t & Type () { return ( ! gah_ ) ? gdh_->type_ : gah_->type_ ; }
	u_int8_t & Mode ( ) { return ( ! gah_ ) ? gdh_->mode_ : gah_->mode_ ; }
	struct dst_list & Dstgroup ()  { return ( ! gah_ ) ? gdh_->dstgroup_ : gah_->dstgroup_ ; }
	struct dst_list & Srcgroup ()  { return ( ! gah_ ) ? gdh_->srcgroup_ : gah_->srcgroup_ ; }
	int Size() const { return ( ! gah_ ) ? gdh_->size() : gah_->size() ;}
	struct packet_info & Base () { return ( ! gah_ ) ? gdh_->base_ : gah_->base_ ;}
}; 


struct hdr_gpsr_hello {
	u_int8_t type_;
	struct node_info  node_;
	float ts_;
	
  inline int size(){
    int sz =
      sizeof(u_int8_t) +
			sizeof(node_info) +
			sizeof ( float );
    return sz;
  }
};

union hdr_all_gpsr {
  hdr_gpsr       gh;
  hdr_gpsr_hello ghh;
  hdr_gpsr_query gqh;
  hdr_gpsr_ask  gah;
  hdr_gpsr_data  gdh;
};



#endif
