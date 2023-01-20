/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

extern pFabric_scheduler<T1,T2> {
    pFabric_scheduler(bit<1> verbose); 
void my_scheduler(in T1 in_flow_id, in T1 number_of_levels_used, in T1 in_pred, in T1 in_arrival_time, in T2 in_shaping, in T2 in_enq, in T1 in_pkt_ptr, in T2 in_deq, in T2 in_use_updated_rank, in T2 reset_time);
void pass_rank_values ( in T1 rank_value, in T1 level_id);
void pass_updated_rank_values ( in T1 rank_value, in T1 flow_id, in T1 level_id);
}

const bit<16> TYPE_IPV4 = 0x800;


/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

typedef bit<9>  egressSpec_t;
typedef bit<48> macAddr_t;
typedef bit<32> ip4Addr_t;

header ethernet_t {
    macAddr_t dstAddr;
    macAddr_t srcAddr;
    bit<16>   etherType;
}

header ipv4_t {
    bit<4>    version;
    bit<4>    ihl;
    bit<6>    diffserv;
    bit<2>    ecn;
    bit<16>   totalLen;
    bit<16>   identification;
    bit<3>    flags;
    bit<13>   fragOffset;
    bit<8>    ttl;
    bit<8>    protocol;
    bit<16>   hdrChecksum;
    ip4Addr_t srcAddr;
    ip4Addr_t dstAddr;
    bit<32>  options;
}

header ipv4_option_t {
    bit<1> copyFlag;
    bit<2> optClass;
    bit<5> option;
    bit<8> optionLength;
}

struct metadata {
}

struct headers {
    ethernet_t   ethernet;
    ipv4_t       ipv4;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.etherType) {
            TYPE_IPV4: parse_ipv4;
            default: accept;
        }
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }
}


/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}


/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    action drop() {
        mark_to_drop(standard_metadata);
    }

    action ipv4_forward(macAddr_t dstAddr, egressSpec_t port) {
        standard_metadata.egress_spec = port;
        hdr.ethernet.srcAddr = hdr.ethernet.dstAddr;
        hdr.ethernet.dstAddr = dstAddr;
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }


    table ipv4_lpm {
        key = {
            hdr.ipv4.dstAddr: lpm;
        }
        actions = {
            ipv4_forward;
            drop;
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }


    @userextern @name("my_pFabric")
    pFabric_scheduler<bit<48>,bit<1>>(1) my_pFabric;

    bit <48> level_3_rank;
    bit <48> in_pred= 15;
    bit <48> in_pkt_ptr;
    bit <48> new_rank_flow_id=0;
    bit <48> flow_new_rank=0;
    bit <48> number_of_levels_used = 1;

    bit <1> in_shaping = 1;
    bit <1> in_enq = 1;
    bit <1> in_deq = 0;
    bit <1> in_use_updated_rank = 1;
    bit <1> update_flow_rank = 0;
    bit <1> reset_time = 0;

    register<bit<48>>(1) register_last_ptr;
    register<bit<48>>(1024) flows_min_rank;
    bit <48> in_flow_id = 0;


    action assign_flow_id(bit <48> flow_id) {
        in_flow_id = flow_id;
    }


    table lookup_flow_id {
        key = {
            hdr.ipv4.srcAddr: lpm;
        }
        actions = {
            assign_flow_id;
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }

    apply {

        lookup_flow_id.apply();

        register_last_ptr.read(in_pkt_ptr,0);
        in_pkt_ptr = in_pkt_ptr + (bit<48>)(1);
        register_last_ptr.write(0,in_pkt_ptr);

        flows_min_rank.read(level_3_rank,(bit<32>)in_flow_id);

    if((level_3_rank == 0)||((bit<48>)(hdr.ipv4.options) < level_3_rank))
    {
        level_3_rank = (bit<48>)(hdr.ipv4.options);
        new_rank_flow_id = in_flow_id;
        flow_new_rank = level_3_rank;
        update_flow_rank = 1;
        flows_min_rank.write((bit<32>)in_flow_id,level_3_rank);
    }
    

        reset_time = 0;

    my_pFabric.pass_rank_values(level_3_rank,0);
    
    if(update_flow_rank == 1)
    {
        my_pFabric.pass_updated_rank_values(flow_new_rank,new_rank_flow_id,0);
    }


        if(hdr.ipv4.dstAddr == 0)
        {
            drop();        
        }
        else
        {
            my_pFabric.my_scheduler(in_flow_id, number_of_levels_used, in_pred, in_pkt_ptr, in_shaping, in_enq, in_pkt_ptr, in_deq, in_use_updated_rank, reset_time);
        }
        
        if (hdr.ipv4.isValid()) {   
           ipv4_lpm.apply();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
     apply {
	update_checksum(
	    hdr.ipv4.isValid(),
            { hdr.ipv4.version,
	      hdr.ipv4.ihl,
	      hdr.ipv4.diffserv,
	      hdr.ipv4.ecn,
              hdr.ipv4.totalLen,
              hdr.ipv4.identification,
              hdr.ipv4.flags,
              hdr.ipv4.fragOffset,
              hdr.ipv4.ttl,
              hdr.ipv4.protocol,
              hdr.ipv4.srcAddr,
              hdr.ipv4.dstAddr },
            hdr.ipv4.hdrChecksum,
            HashAlgorithm.csum16);
    }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;
