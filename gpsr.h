#ifndef GPSR_ROUTING_H_
#define GPSR_ROUTING_H_

#include "config.h"
#include "agent.h"
#include "ip.h"
#include "address.h"
//#include "scheduler.h"
#include "timer-handler.h"
#include "mobilenode.h"
#include "tools/random.h"
#include "packet.h"
#include "trace.h"
#include "classifier-port.h"

//不知道为什么要用自己的cmu-trace.h 文件 用回ns的
#include <cmu-trace.h>

#include "gpsr_packet.h"
#include "gpsr_neighbor.h"
#include "gpsrCoordinateTable.hpp"

/* #include <list> */
/* #include <functional> */
/* using std::list;  */
/* using std::bind2nd ;  */





/*
// timer define 
//   跳出空洞路由计时器
//   外界中断计时器时 就是销毁时候
*/

class GPSRAgent ; 

class GPSRJumpoutTimer :  public TimerHandler {
public:
  static unsigned ttls_[4]; 
private:
  unsigned ttl_seqno_ ; 
  u_int8_t   status_;                                                                     //  计时器是否有效
protected:
  virtual void expire ( Event * );
  GPSRAgent *a_;
  Packet *p_;
public:
  GPSRJumpoutTimer ( GPSRAgent *a, Packet *p ); 
  ~GPSRJumpoutTimer () ; 
  void sendrequest ( ); 
  bool isUseful () const 
  {
    return (status_ == 1) ? true : false ; 
  }
};

/*
 * Hello timer which is used to fire the hello message periodically
 */
class GPSRHelloTimer : public TimerHandler {
protected:
  virtual void expire(Event *e);
  GPSRAgent *a_;

private:
  unsigned long counter_; 
  unsigned modulus_; 

public:
  GPSRHelloTimer(GPSRAgent *a) : TimerHandler(), counter_(1), a_(a), modulus_(1) 
  {}
  ~GPSRHelloTimer( )
  {
    resched(INFINITE_DELAY) ;
  }
  void setModulus ( unsigned m ) {
    modulus_ = m; }
};



// 各个数据库的计时  用于清理数据
// 目标坐标数据库
class CDTableTimer : public TimerHandler {
public:
  CDTableTimer(GPSRAgent *a) : TimerHandler() {a_=a;}
  ~CDTableTimer () { resched(INFINITE_DELAY) ;  a_ = NULL;  }
protected:
  virtual void expire(Event *e);
  GPSRAgent *a_;
};

// 辅助数据库 
class HelpOutDataTimer : public TimerHandler {
public:
  HelpOutDataTimer(GPSRAgent *a) : TimerHandler(), a_(a)
  {}
  ~HelpOutDataTimer() { a_ = NULL; }
protected:
  virtual void expire(Event *e);
  GPSRAgent *a_;
};

// 缓冲数据库
// 周期应该设置大些   
class BufferQueueTimer : public TimerHandler {
public:
  BufferQueueTimer(GPSRAgent *a) : TimerHandler(), a_(a)
  {}
  ~BufferQueueTimer() { a_ = NULL; }
protected:
  virtual void expire(Event *e);
  GPSRAgent *a_;
};


// timer define end 
///////////////////////////////////
// buffer queue declare
struct keepTimePacket {
  Packet *p_ ;
  GPSRJumpoutTimer t_; 
  struct keepTimePacket *next; 
  struct keepTimePacket *samedstp; 
  keepTimePacket ( GPSRAgent *a, Packet *p ):p_(p) , t_(a, p) , next(NULL), samedstp(NULL) {}
  ~keepTimePacket()
  {
    p_ =NULL; 
    next = NULL; 
    samedstp = NULL;
  }
};
class bufferqueue {
 
private:
  keepTimePacket  *bq_; 
  unsigned int packet_num_; 
  GPSRAgent *a_ ; 
public :
  bufferqueue (): bq_(NULL), packet_num_(0) {}
  bufferqueue ( GPSRAgent  * const a ); 
  ~bufferqueue () ; 
  int size () const 
  { return packet_num_ ; }
  bool  add ( Packet *p ) ;                                     // 返回值 true为新的目的地址类型数据包 false 已有目的地址数据包在等待
  void delAllTimeout ( double useful_period ); 
  void delAll (); 
  keepTimePacket * pop  ( nsaddr_t dst );
};


// buffer queue declare end 
class GPSRAgent : public Agent {
private:

  friend class GPSRHelloTimer;
  friend class CDTableTimer; 
  friend class HelpOutDataTimer; 
  friend class GPSRJumpoutTimer; 
  friend class BufferQueueTimer; 
 
  MobileNode *node_;             //the attached mobile node
  PortClassifier *port_dmux_;    //for the higher layer app de-multiplexing
  
  RNG randSend_;                         

  bool on_gpsr_status_;             // 判断节点是否加入gpsr网络 

  nsaddr_t my_id_;               //node id (address), which is NOT necessary
  double my_x_;                  //node location info, fatal info
  double my_y_;                  //     obtained from the attached node

  node_info myinfo ( );  

  cdtable *coortable_;                 // 坐标数据库     
  GPSRNeighbors *nblist_;              //neighbor list: routing table implemenation 
  helpoutdata   *helplist_;      // 辅助数据库
  bufferqueue  *bufferq_;      //数据包缓冲队列

  unsigned int requestAskCounter_;                // 请求应答序列
 

  // 计时器
  GPSRHelloTimer hello_timer_;
  CDTableTimer    clean_cdtable_timer_;
  HelpOutDataTimer clean_helpoutdata_timer_; 
  BufferQueueTimer clean_bufferqueue_timer_; 

  double   safe_distance_; 
  
  double hello_period_;                           // 发送hello包 周期
  unsigned clean_hello_modulus_;                //  清理hello数据包周期 系数
  double clean_cdtable_period_;                    // 清理cdtable数据库周期
  double clean_helpoutdata_period_;            //  清理helpoutdata数据库周期
  double clean_bufferqueue_period_;            // 清理缓冲数据库周期

  double wait_ask_period_;                       // 发送每个请求的等待周期
  
  void turnon();              //节点加入gpsr网络 可以作应答 邻居广播自己 
  void turnoff();             // 离开gpsr网络  不做应答 不广播自己
  bool isGPSROn () const ; 

  // 更新地址信息
  virtual void getLoc();
  // 初始化计时器
  void initTimer (); 

  //发送hello数据包 和 请求数据包 
  void hellomsg();
  void query(nsaddr_t);
  void request (const node_info &,  u_int8_t type =GPSR_RQTYPE_DST , int ttl  = IP_DEF_TTL );              // 发送请求数据  ttl参数决定跳数
  void ask ( Packet *&  , const struct packet_info &  );
  void data ( Packet * );                                                     // 产生gpsr data类型数据包

  void recvHello(Packet*);
  void recvQueryRequest(Packet*);
  void recvQueryAsk (Packet*);
  void recvData ( Packet *);
  void forwardData(Packet*);
  void sendBufferPacket ( nsaddr_t dst ); 
 
  void dealwithAsk ( Packet *) ; 
  void showNeighbor () const ; 

private:
  void    _fillDsts ( Packet *);
  nsaddr_t  _findNexthop ( Packet *, dst_list & , int loop_num = 10 ) ; // loop_num 是为了防止两个以上组成循环，不断循环查找
  // nsaddr_t _findDataDst (Packet *);
  

protected:
  Trace *tracetarget;              //for routing agent special trace
  void trace(char *fmt,...);       //   Not necessary 

  void hellotout();                //called by timer::expire(Event*)
  

public:
  GPSRAgent();
  // modify by anzizhao 
  ~GPSRAgent();
  
  int command(int, const char*const*);
  void recv(Packet*, Handler*);         //inherited virtual function
  // 为bufferqueue 提供cdtable 增添接口
  void addCDtableItem ( cooentry entry ); 
 
  static u_int8_t  getDstgroup ( Packet *p ,  dst_list *&dstgroup  ) ;                  // get ask or data packet  dst
  //  void setSrcgroup (  Packet *p, const node_info & info ); 
};

#endif
