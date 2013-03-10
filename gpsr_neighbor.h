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

/* gpsr_neighbor.h : the head file for the definition of the neighbor 
 *                   list for each node in the network
 * Note: Due to the network nature, the neighbor list of different nodes 
 *       which are neighbors from a view of the transimission radio range
 *       may not consistent, which means for a pair of nodes, one may know
 *       the other is a neighbor, but the other one does not
 */


#ifndef GPSR_NEIGHBOR_H_
#define GPSR_NEIGHBOR_H_

#include "gpsr_packet.h"

#define DEFAULT_GPSR_TIMEOUT   200.0   
        //the default time out period is 200.0 sec
        //If the hello messge was not received during this period
        //the entry in the neighbor list may be deleted

#define INIFINITE_DISTANCE   1000000000.0 //bigger than radio range is enough
#define  NEIGHBORLIST   "neighborlist.txt"

/* The structure used for neighbor entry
 */
struct gpsr_neighbor {
	node_info node_;                                                            // the geo info 
	float ts_;                                                                           // the time to send 
	struct gpsr_neighbor *next ; 
};

class GPSRNeighbors {
private:
	struct gpsr_neighbor *head_;
	struct gpsr_neighbor *tail_ ;                                  // useless 
  
	int nbSize_;                                                           //number of neighbors
	node_info me_; 
  // add by anzizhao
  double safedis_;                                                     // it is important to keep the packet safe transform.
  
public:
  GPSRNeighbors();
  ~GPSRNeighbors();

  //using to update location information of myself
  void myinfo(const node_info  node)
	{  me_  = node ;  } 

  //return the number of neighbors
  int size () const 
	{  return nbSize_; }

  //calculate the distance between (x1, y1, x2, y2)
  static double getdis(double, double, double, double)  ;
  static double getdis ( const node_info &src, const node_info & dst )  ; 


  //add a possible new neighbor, or update the info 
  bool add ( const gpsr_neighbor & nb ); 
  bool getEntrybyID ( nsaddr_t  id , gpsr_neighbor *& nbp ); 
  bool remove ( nsaddr_t  );   
	bool isEntry ( nsaddr_t id ) ; 
	//    邻居里到dst最短的 是否比src-dst更短 最重要函数
  bool  shortestDistance ( const node_info & src, const node_info &dst , node_info *& np );     

	void delAllTimeout ( double period ) ; 

	// little tools 
	void showlist ();
	void setSafedistance ( double dis ) {  safedis_ = dis;  }
private:
  bool InSafetyZone( double x, double y )
  {
    return (getdis(me_.x_, me_.y_, x, y) < safedis_) ? true:false;
	}
  bool InSafetyZone( const node_info & node )
  {
    return (getdis(me_, node) < safedis_) ? true:false;
	}
	bool _findentry ( nsaddr_t id, gpsr_neighbor *&pre, gpsr_neighbor *&current ) ;  
	void _delAll ( ); 
};

#endif 
