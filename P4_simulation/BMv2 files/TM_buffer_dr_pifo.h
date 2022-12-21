#include <bm/bm_sim/_assert.h>
#include <bm/bm_sim/parser.h>
#include <bm/bm_sim/tables.h>
#include <bm/bm_sim/logger.h>
#include <bm/bm_sim/extern.h>

#include <unistd.h>

#include <condition_variable>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include <queue>

#include "simple_switch.h"
#include "register_access.h"
#include </home/vagrant/tutorials/utils/user_externs_dr_pifo/DR_PIFO.h>

std::vector<std::shared_ptr<bm::DR_PIFO_scheduler::flow_scheduler>> bm::DR_PIFO_scheduler::FS = { NULL};

std::vector<std::shared_ptr<bm::DR_PIFO_scheduler::fifo_bank>> bm::DR_PIFO_scheduler::FB = { NULL};

unsigned int bm::DR_PIFO_scheduler::time_now = 0;
unsigned int bm::DR_PIFO_scheduler::number_levels = 1;

std::vector<unsigned int> bm::DR_PIFO_scheduler::number_of_queues_per_level = {1};
std::vector<unsigned int> bm::DR_PIFO_scheduler::number_of_pkts_per_queue_each_level = {1024};

unsigned int sum_all_queues = bm::DR_PIFO_scheduler::number_of_queues_per_level[0];

std::vector<unsigned int> bm::DR_PIFO_scheduler::error_detected_each_level(sum_all_queues);
std::vector<unsigned int> bm::DR_PIFO_scheduler::internal_force_flow_id_each_level(sum_all_queues);

unsigned int number_of_update_ranks_all_level = (bm::DR_PIFO_scheduler::number_of_pkts_per_queue_each_level[0]*bm::DR_PIFO_scheduler::number_of_queues_per_level[0] * bm::DR_PIFO_scheduler::number_levels);

std::vector<unsigned int> bm::DR_PIFO_scheduler::new_ranks_each_level(number_of_update_ranks_all_level);

std::vector<unsigned int> bm::DR_PIFO_scheduler::flow_id_not_empty_each_level(number_of_update_ranks_all_level);

std::queue<unsigned int> bm::DR_PIFO_scheduler::pkt_ptr_queue;

unsigned int bm::DR_PIFO_scheduler::use_updated_rank = 0;
unsigned int bm::DR_PIFO_scheduler::last_force_deq = 0;
unsigned int bm::DR_PIFO_scheduler::force_deq_flow_id = 0;
unsigned int bm::DR_PIFO_scheduler::shaping = 0;
unsigned int bm::DR_PIFO_scheduler::enable_error_correction = 0;
unsigned int bm::DR_PIFO_scheduler::number_of_enqueue_packets = 0;
unsigned int bm::DR_PIFO_scheduler::number_of_read_packets = 0;
unsigned int bm::DR_PIFO_scheduler::number_of_dequeue_packets = 0;
unsigned int bm::DR_PIFO_scheduler::switch_is_ready = 1;

int bm::DR_PIFO_scheduler::start_time = 0;
int bm::DR_PIFO_scheduler::last_time = 0;

std::queue<unsigned int> pkt_ptr_queue;

std::vector<unsigned int> bm::DR_PIFO_scheduler::pkt_levels_ranks = {0};

bool dequeued_pointers[50000] = {0};

namespace {

class TM_buffer{

bm::DR_PIFO_scheduler dequeue_scheduler; // create object of the DR_PIFO_scheduler class.
 public:
	struct buffer {
		std::unique_ptr<Packet> packet;
		double ptr;
		std::shared_ptr<buffer> next;
	};
	static std::shared_ptr<buffer> buffer_head;


unsigned int valid_pop(std::unique_ptr<Packet>& packet)
{
////////////////////////////////////// start with the push operation
	if(packet != NULL)
	{
		std::shared_ptr<buffer> cur_ptr_buffer;
		cur_ptr_buffer = std::make_shared<buffer>();
		std::shared_ptr<buffer> prev_ptr_buffer;
		prev_ptr_buffer = std::make_shared<buffer>();

        	unsigned int ptr = dequeue_scheduler.get_last_pkt_ptr();

		if(buffer_head == NULL)
		{
			buffer_head = std::make_shared<buffer>();
			buffer_head->packet = std::move(packet);
			buffer_head->ptr = ptr;
			buffer_head->next = NULL;
		}
		else
		{
			cur_ptr_buffer = buffer_head;
			prev_ptr_buffer = NULL;
			while (cur_ptr_buffer != NULL)
			{
				prev_ptr_buffer = cur_ptr_buffer;
				cur_ptr_buffer = cur_ptr_buffer->next;
			}
			std::shared_ptr<buffer> temp_ptr;
			temp_ptr = std::make_shared<buffer>();
			temp_ptr->packet = std::move(packet);
			temp_ptr->ptr = ptr;
			temp_ptr->next = NULL;
			prev_ptr_buffer->next = temp_ptr;
		}
	}
////////////////////// then the pop operation 
	std::shared_ptr<buffer> cur_ptr_buffer;
	cur_ptr_buffer = std::make_shared<buffer>();
	std::shared_ptr<buffer> prev_ptr_buffer;
	prev_ptr_buffer = std::make_shared<buffer>();
	cur_ptr_buffer = buffer_head;
	prev_ptr_buffer = NULL;

	unsigned int valid_packet = 0;
	packet = NULL;
	unsigned int ptr = 0;

	// Elbediwy : If we didn't finish dequeueing all enqueued packets, use the dequeue function of the scheduler (dequeue_my_scheduler())

	if((dequeue_scheduler.number_of_deq_pkts() < dequeue_scheduler.num_of_read_pkts()) && (dequeue_scheduler.number_of_enq_pkts() == dequeue_scheduler.num_of_read_pkts()))
	{
	
		ptr = dequeue_scheduler.dequeue_my_scheduler();
	}


	if((ptr != 0)||(!pkt_ptr_queue.empty()))
	{

	  	if(ptr != 0)
	  	{
	  		pkt_ptr_queue.push(ptr);	  		
	  	}
	  	ptr = pkt_ptr_queue.front();

		
	   	if (buffer_head->ptr == ptr)
		{
			packet = std::move(buffer_head->packet);
			buffer_head = buffer_head->next;
		}
		else
		{
			while (cur_ptr_buffer != NULL)
			{
				if(cur_ptr_buffer->ptr == ptr)
				{
					packet = std::move(cur_ptr_buffer->packet);
					prev_ptr_buffer->next = cur_ptr_buffer->next;
					break;
				}
				prev_ptr_buffer = cur_ptr_buffer;
				cur_ptr_buffer = cur_ptr_buffer->next;
			}
		}

		if(packet != NULL)
		{
			dequeue_scheduler.increment_deq_count();
      		start_dequeue(1);
			valid_packet = 1;
			dequeued_pointers[ptr] = true;
		}

  		start_dequeue(1);

		if(dequeued_pointers[ptr] == true)
		{
			pkt_ptr_queue.pop();
		}
	}
	return valid_packet;
}

unsigned int enqueued_packets()
{
	return dequeue_scheduler.number_of_enq_pkts();
}

unsigned int dequeued_packets()
{
	return dequeue_scheduler.number_of_deq_pkts();
}

unsigned int num_of_read_pkts()
{
	return dequeue_scheduler.num_of_read_pkts();
}

void start_dequeue(unsigned int start)
{
	dequeue_scheduler.start_dequeue(start);
}

};

}
