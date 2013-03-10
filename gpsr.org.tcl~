Agent/GPSR set sport_        0
Agent/GPSR set dport_        0

puts "GPSR configuration file"

# ======================================================================

proc create-gpsr-routing-agent { node id } {
    global ns_ ragent_ tracefd opt

    #
    #  Create the Routing Agent and attach it to port 255.
    #
    #set ragent_($id) [new $opt(ragent) $id]
    set ragent_($id) [new Agent/GPSR]
    set ragent $ragent_($id)

    ## setup address (supports hier-addr) for dsdv agent 
    ## and mobilenode
    set addr [$node node-addr]
    
    $ragent addr $addr
    $ragent node $node
    if [Simulator set mobile_ip_] {
	$ragent port-dmux [$node set dmux_]
    }
    $node addr $addr
    $node set ragent_ $ragent
	    
	set T [new Trace/Generic]
	$T target [$ns_ set nullAgent_]
	$T attach $tracefd
	$T set src_ $id
	$ragent tracetarget $T

    $node attach $ragent [Node set rtagent_port_]

}


proc gpsr-create-mobile-node { id args } {
	global ns ns_ chan prop topo tracefd opt node_

	set ns_ [Simulator instance]
    set node_($id) [new Node/MobileNode]
	set node $node_($id)
	$node random-motion 0		;# disable random motion
	$node topography $topo
    
	# XXX Activate energy model so that we can use sleep, etc. But put on 
	# a very large initial energy so it'll never run out of it.
	if [info exists opt(energy)] {
		$node addenergymodel [new $opt(energy) $node 1000 0.5 0.2]
	}

	#
	# This Trace Target is used to log changes in direction
	# and velocity for the mobile node.
	#
	set T [new Trace/Generic]
	$T target [$ns_ set nullAgent_]
	$T attach $tracefd
	$T set src_ $id
	$node log-target $T
    
	if ![info exist inerrProc_] {
		set inerrProc_ ""
	}
	if ![info exist outerrProc_] {
		set outerrProc_ ""
	}
	if ![info exist FECProc_] {
		set FECProc_ ""
	}

	$node add-interface $chan $prop $opt(ll) $opt(mac)	\
	    $opt(ifq) $opt(ifqlen) $opt(netif) $opt(ant) \
	    $topo $inerrProc_ $outerrProc_ $FECProc_ 
    
	#
	# Create a Routing Agent for the Node
	#
	create-gpsr-routing-agent $node $id
    
    $ns_ at 0.0 "$node_($id) start"
    
    return $node
}

