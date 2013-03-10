#ifndef _GPSRBUFFERQUEUE_HPP
#define  _GPSRBUFFERQUEUE_HPP




#include <list>
#include <functional>
#include "packet.h"

// using std::list;
// using std::bind2nd ; 
using namespace std; 
class GPSRAgent ; 

//   跳出空洞路由计时器
//   外界中断计时器时 就是销毁时候
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


struct keepTimePacket {
  Packet *p_ ;
  GPSRJumpoutTimer t_; 
  keepTimePacket ( GPSRAgent *a, Packet *p ):p_(p) , t_(a, p)
  {}
};

class bufferqueue {
private:
  list<keepTimePacket> bq_; 
  GPSRAgent *a_ ; 

public :
  
  bufferqueue () 
  {}
  bufferqueue ( GPSRAgent  * const a ); 
  ~bufferqueue () ; 
  int size () const 
  { return bq_.size(); }
  void add ( Packet *p ) ; 
  void delAllTimeout ( double useful_period ); 
  list<Packet*> pop  ( nsaddr_t dst );
};
#endif
