  /* -*- Mode:C++; c-basic-offset: 2; tab-width:2, indent-tabs-width:t -*- 
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

#include "gpsr_neighbor.h"

#define PI 3.141593
#define SAFEDISTANCE   225                     // 无线接收距离250时安全距离                               

#define MAX(a, b) (a>=b?a:b)
#define MIN(a, b) (a>=b?b:a)

GPSRNeighbors::GPSRNeighbors(){
  me_.id_ = -1;
  me_.x_ = 0.0;
  me_.y_ = 0.0;

  head_ = tail_ = NULL;
  nbSize_ = 0;
  safedis_ = SAFEDISTANCE  ; 
}

GPSRNeighbors::~GPSRNeighbors(){
  _delAll ();
}

void 
GPSRNeighbors::_delAll ()
{
  struct gpsr_neighbor *temp = head_;
  struct gpsr_neighbor * dd = NULL;
  while(temp){
    dd = temp ; 
    temp = temp->next;
    free(temp);
  }
  head_ = NULL;
}

double 
GPSRNeighbors::getdis(double ax, double ay, double bx, double by)  
{
  double tempx = ax - bx;
  double tempy = ay - by;

  tempx = tempx * tempx;
  tempy = tempy * tempy;

  return sqrt(tempx + tempy);
}

double 
GPSRNeighbors::getdis(const node_info & src, const node_info &dst ) 
{
  double tx = src.x_ - dst.x_;
  double ty = src.y_  - dst.y_;
  
  return sqrt ( (tx * tx) + (ty * ty) ); 
}

bool
GPSRNeighbors::_findentry ( nsaddr_t id, gpsr_neighbor *&pre, gpsr_neighbor *&current ) 
{
 
  if ( nbSize_ ==  0 )
    return false ;
  // 第一个需要特别对待    
  // 如果第一个是目标 pre NULL current 指向第一个
  if ( head_ ->node_.id_ == id ) 
    {
      pre = NULL;
      current = head_; 
      return true; 
    }
  
  pre = head_;
  current = head_->next; 
  while ( current )
    {
      if ( current ->node_.id_  == id )
	return true; 
      pre = current ; 
      current = current->next; 
    }
  return false ; 
}

bool 
GPSRNeighbors::add ( const gpsr_neighbor & nb )
{
  struct gpsr_neighbor *pre = NULL, *current = NULL; 
  // 是否在安全区域的数据包  为了传输的可靠，确保邻居表里都是安全区域的节点信息
  if ( ! InSafetyZone( nb.node_ ) )
    return false;
  if ( ! _findentry ( nb.node_.id_, pre, current ) )                         // 没有该项
    {
      gpsr_neighbor *nbp ;
      nbp = ( gpsr_neighbor *) malloc ( sizeof(gpsr_neighbor) );
      if ( ! nbp )
	assert ( false); 
      *nbp = nb ;
      nbp->next = head_;
      head_ = nbp ;  
      nbSize_ ++; 
    } else  {                                                                                     
      // 有该项
      // 筛选
      if (  false )
	return false ; 
      pre = current->next; 
      *current = nb ; 
      current->next = pre; 
    }

  return true; 
}

bool 
GPSRNeighbors::getEntrybyID ( nsaddr_t  id , gpsr_neighbor *& nbp )
{
  gpsr_neighbor *pre, *current; 
  if ( ! _findentry ( id , pre, current ) )                         // 没有该项
    {
      nbp = NULL;
      return false;
    }
  else                                                                                        // 有该项
    { 
      nbp =   ( gpsr_neighbor *) malloc ( sizeof(gpsr_neighbor) );
      if ( !nbp )
	assert ( false ); 
      *nbp = *current ; 
      nbp -> next = NULL;
      return true; 
    }

}

bool 
GPSRNeighbors::remove ( nsaddr_t  id )
{
  gpsr_neighbor *pre, *current; 
  if ( _findentry ( id , pre, current ) )                         // 有该项
    {   
      if (  !  pre  )                                                                           // 第一项
	{
	  head_ = current->next;
	}
      else
	{
	  pre->next = current->next;
	}
      free ( current );
      nbSize_ --; 
      return true; 
    }
  return false ; 
}

bool 
GPSRNeighbors::isEntry ( nsaddr_t id ) 
{
  gpsr_neighbor *pre, *current; 
  return _findentry ( id , pre, current ) ;   
}


void
GPSRNeighbors::delAllTimeout ( double useful_period ){
  struct gpsr_neighbor *temp = head_;
  struct gpsr_neighbor *pre = NULL;
 
  float current_time = ( float) GPSR_CURRENT ;  
  while( temp ) {
    if( ( current_time  - temp->ts_) > useful_period  ){
      if ( !pre ) {
	// 第一项
	head_ = temp->next; 
	free ( temp ) ; 
	temp = head_; 
      } else {
	pre->next = temp->next ;
	free ( temp ) ; 
	temp = pre->next; 
      }    // else 2 
      nbSize_ --; 
    } else { 
      pre = temp ; 
      temp = temp->next;
    }  // else  1
  } // while 
  
}


bool 
GPSRNeighbors::shortestDistance ( const node_info & src, const node_info &dst , node_info  *& np )
{
  if ( nbSize_ == 0 )
    return false ;
  np = NULL;
  struct gpsr_neighbor  *temp = head_;

 
  // 查找邻居表中中距离最短的下一跳
  if (  head_->node_.id_ == dst.id_   )
    {
      np = (node_info * ) malloc ( sizeof(node_info ));
      if ( ! np )
	assert ( false );
      *np = temp->node_;
      return true; 
    }

  temp = head_; 
  // 第一个在安全区域
  double shortest_dis =  getdis ( temp->node_ ,  dst );                  
  struct gpsr_neighbor * shortest_id = temp ;
  temp = temp->next; 
  while ( temp ) {
    // 跳过源节点
    if ( temp->node_.id_ == src.id_ ) 
      {
	temp = temp->next;
	continue;
      }
    // 目标节点是否就在邻居
    if (  temp->node_.id_ == dst.id_   )
      { // 是  立即返回信息
	np = (node_info * ) malloc ( sizeof(node_info ));
	if ( ! np )
	  assert ( false );
	*np = temp->node_;
	return true; 
      }

    double temp_dis  = getdis ( temp->node_ ,  dst );
    if ( shortest_dis >  temp_dis  )
      {
	shortest_dis = temp_dis; 
	shortest_id = temp ;
      }
    temp = temp->next;
  }
 
  double  original_dis = getdis ( src , dst );
  if ( original_dis  >= shortest_dis )
    {
      np = (node_info* ) malloc ( sizeof(node_info ));
      if ( ! np )
	assert ( false );
      *np = shortest_id->node_; 
      return true;
    }
  else 
    {
      np = NULL;
      return false;
    }

}


void 
GPSRNeighbors::showlist () 
{
  FILE *fp = fopen ( NEIGHBORLIST, "a+" ); 
  struct gpsr_neighbor  * current = head_; 
  fprintf ( fp, "%2.f\t%d\t", GPSR_CURRENT, me_.id_ ); 
  while ( current ) {
    fprintf ( fp, "%d ", current->node_.id_ ); 
    current = current->next; 
  }
  fprintf ( fp, "\n"); 
   fclose ( fp  ); 
}
