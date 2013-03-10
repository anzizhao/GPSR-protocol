#ifndef  _GPSRCDTABLE_CPP
#define   _GPSRCDTABLE_CPP


#include "gpsrCoordinateTable.hpp"


cooentry 
cdtable::makeNNEntry ( nsaddr_t id )
{
  struct cooentry entry ;
  entry.node_.id_ = id;
  entry.node_.x_ = 0.0;
  entry.node_.y_  = 0.0;
  entry.lasthop_ = -1;
  entry.hops_ = -1;
  entry.seqno_ = -1;
  entry.ts_ = (float) GPSR_CURRENT ; 
  entry.next = NULL;
  return entry ; 
}


bool 
cdtable::_delAll ()
{
  struct cooentry *temp = coordinatelist_;
  struct cooentry *del  = NULL;

  while ( temp )
    {
      del = temp;
      temp = temp->next;
      free(del);
    }
  coordinatelist_ = NULL;
  coornum_ = 0; 
  return true; 
}



cdtable:: cdtable()
{
  coordinatelist_ = NULL;
  coornum_ = 0; 
}


cdtable:: ~cdtable()
{
  _delAll ();
}


bool 
cdtable::_findentry ( nsaddr_t id, cooentry *& pre, cooentry *& current  )
{
  if ( coornum_ == 0 )
    return false ;
  
  if ( coordinatelist_->node_.id_ == id )
    {
      pre = NULL;
      current = coordinatelist_; 
      return true; 
    }
  pre = coordinatelist_;
  current = coordinatelist_->next;
  
  while ( current )
    {
      if ( current->node_.id_  ==  id )
	return true;
      pre = current ;
      current = current->next;
    }

  
  return false ;
}

bool 
cdtable::add ( const cooentry & entry )
{
  struct cooentry *pre, *cur; 
  if (  ! _findentry(entry.node_.id_, pre, cur ) )             //新的目的坐标
    {
      cur = ( struct cooentry *) malloc ( sizeof ( struct cooentry ) );
      if ( ! cur )
	assert ( false );
      
      *cur = entry ;
      // 在链表头插入
      cur->next = coordinatelist_; 
      coordinatelist_  = cur;
      coornum_ ++; 
    }
  else                                                                            // 更新坐标
    {
      // 判断是否有效 
      // 每一个节点使用requestAskseqno_序列   序列和时间值是越来越大的
      if ( cur->seqno_ >= entry.seqno_  )
	return false;

      pre  = cur->next; 
      *cur = entry ; 
      cur->next = pre ; 
    }

  return true;
}

bool 
cdtable::add  ( const struct packet_info &info, nsaddr_t lasthop , int hops )
{
  struct cooentry entry ;
  entry.node_ = info.node_;
  entry.seqno_ = info.seqno_;
  entry.ts_         = info.ts_;
  entry.lasthop_  = lasthop;
  entry.hops_   =  hops;
  return add ( entry ) ; 

}

bool 
cdtable::remove ( nsaddr_t id )
{
  struct cooentry *pre, *cur; 
  if (   _findentry( id , pre, cur ) )           
    {
      if ( !pre )                                                                                         // 第一项
	coordinatelist_ = cur->next; 
      else 
	pre->next  = cur->next;
      free(cur);
      coornum_ --; 
      return true;
    }
  return false;
}

bool 
cdtable::getEntrybyID  ( nsaddr_t id , struct cooentry  * &entry )
{
  struct cooentry *pre, *cur; 
   if (   _findentry( id , pre, cur ) )           
    {
      entry = ( struct cooentry *) malloc ( sizeof(struct cooentry) );
      if ( ! entry )
	assert ( false );
      
      *entry = *cur;
      entry -> next = NULL;
      return true;
    }
   entry  = NULL;
   return false;
}

bool 
cdtable::isEntry  ( nsaddr_t id )
{
   struct cooentry *pre, *cur; 
   if (   _findentry( id , pre, cur ) )    
     return true;
   
   return false;
}

void
cdtable::delAllTimeout ( double useful_period )
{
  struct cooentry *temp  = coordinatelist_;
  struct cooentry *pre = NULL ;
  float current_time = ( float) GPSR_CURRENT ; 
  while(temp){
    if( ( current_time  - temp->ts_) > useful_period  ){
      if ( ! pre ) {
	coordinatelist_ = temp ->next; 
	free ( temp ); 
	temp = coordinatelist_; 
      } else {
	pre->next = temp->next; 
	free ( temp ); 
	temp = pre ->next; 
      } // else  2 
      coornum_ --; 
    } else {
      pre = temp ; 
      temp = temp->next; 
    } // else 
  } // while 
  
}

void
helpoutdata::delAllTimeout ( double useful_period )
{
  helpoutentry *temp  = hlist_ ;
  helpoutentry  *pre  = NULL;
  float current_time = ( float) GPSR_CURRENT ; 
  while(temp){
    if( ( current_time  - temp->ts_) > useful_period  ){
      if ( !pre ) { 
	hlist_ = temp->next; 
	free ( temp ); 
	temp = hlist_; 
      } else {
	pre ->next = temp->next ;
	free ( temp ); 
	temp = pre->next; 
      }   // else 2 
      num_ --; 
    } else  {
      pre = temp ; 
      temp = temp->next;
    }   // else 
  } // while 


}

void 
helpoutdata::_delAll ()
{
    helpoutentry *temp  = hlist_ ;
    helpoutentry  *dd;
    while ( temp ) {
      dd = temp ;
      temp = temp->next; 
      free ( dd ) ; 
    }
    hlist_ = NULL; 
    num_ = 0; 
}


bool 
helpoutdata::_findentry ( nsaddr_t id, helpoutentry *& pre, helpoutentry *& cur  )
{
  if ( num_ == 0  )
    return false ;
  
  if ( hlist_->wheretogo_ == id )
    {
      pre = NULL;
      cur = hlist_;
      return true; 
    }
  
  pre = hlist_;
  cur = hlist_->next;
  while ( cur )
    {
      if ( cur->wheretogo_  == id )
	return true; 
      pre = cur;
      cur = cur->next; 
    }
  return false ; 
}


bool 
helpoutdata::add ( const struct helpoutentry & entry )
{
  helpoutentry *pre, *cur ;
  if (  _findentry ( entry.wheretogo_ , pre, cur ) ) {
    // 跳数越小越有利
    if ( cur->hops_ > entry.hops_   ) {
      pre = cur->next;
      *cur = entry;
      cur->next  = pre; 
      return true; 
    }
      
  } else {
    cur = (struct helpoutentry *)malloc (sizeof( struct helpoutentry ));
    if ( !cur )
      assert ( false );
    *cur = entry ;
    
    cur->next = hlist_;
    hlist_ = cur ; 
    num_ ++; 
    return true; 
  }

  return false ;
}

bool
helpoutdata::remove ( nsaddr_t id )
{
  struct helpoutentry *pre, *cur; 
  if (   _findentry( id , pre, cur ) )           
    {
      if ( !pre )                                                                                         // 第一项
	hlist_ = cur->next; 
      else 
	pre->next  = cur->next;
      free(cur);
      num_ --; 
      return true;
    }
  return false;
}

bool
helpoutdata::getEntrybyID ( nsaddr_t id , helpoutentry *& entry )
{
  struct helpoutentry  *pre, *cur; 
  if (   _findentry( id , pre, cur ) )           
    {
      entry = ( struct helpoutentry *) malloc ( sizeof ( struct helpoutentry )  );
      if ( ! entry )
	assert ( false );
      
      *entry = *cur;
      entry -> next = NULL;
      return true;
    }
  entry  = NULL;
  return false;
}

#endif 
