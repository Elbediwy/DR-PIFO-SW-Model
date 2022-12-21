#include "DR_PIFO.h"
#include <bm/bm_sim/logger.h>

namespace bm {

std::vector<std::shared_ptr<DR_PIFO_scheduler::flow_scheduler>> DR_PIFO_scheduler::FS = { NULL};

std::vector<std::shared_ptr<DR_PIFO_scheduler::fifo_bank>> DR_PIFO_scheduler::FB = { NULL};

unsigned int DR_PIFO_scheduler::time_now = 0;
unsigned int DR_PIFO_scheduler::number_levels = 1;

std::vector<unsigned int> DR_PIFO_scheduler::number_of_queues_per_level = {1};
std::vector<unsigned int> DR_PIFO_scheduler::number_of_pkts_per_queue_each_level = {1024};

unsigned int sum_all_queues = DR_PIFO_scheduler::number_of_queues_per_level[0];

std::vector<unsigned int> DR_PIFO_scheduler::error_detected_each_level(sum_all_queues);
std::vector<unsigned int> DR_PIFO_scheduler::internal_force_flow_id_each_level(sum_all_queues);

unsigned int number_of_update_ranks_all_level = (DR_PIFO_scheduler::number_of_pkts_per_queue_each_level[0]*DR_PIFO_scheduler::number_of_queues_per_level[0] * DR_PIFO_scheduler::number_levels);

std::vector<unsigned int> DR_PIFO_scheduler::new_ranks_each_level(number_of_update_ranks_all_level);

std::vector<unsigned int> DR_PIFO_scheduler::flow_id_not_empty_each_level(number_of_update_ranks_all_level);

std::queue<unsigned int> DR_PIFO_scheduler::pkt_ptr_queue;

unsigned int DR_PIFO_scheduler::use_updated_rank = 0;
unsigned int DR_PIFO_scheduler::last_force_deq = 0;
unsigned int DR_PIFO_scheduler::force_deq_flow_id = 0;
unsigned int DR_PIFO_scheduler::shaping = 0;
unsigned int DR_PIFO_scheduler::enable_error_correction = 0;
unsigned int DR_PIFO_scheduler::number_of_enqueue_packets = 0;
std::vector<unsigned int> DR_PIFO_scheduler::pkt_levels_ranks = {0};
unsigned int DR_PIFO_scheduler::number_of_read_packets = 0;
unsigned int DR_PIFO_scheduler::number_of_dequeue_packets = 0;
unsigned int DR_PIFO_scheduler::switch_is_ready = 1;

int DR_PIFO_scheduler::start_time = 0;
int DR_PIFO_scheduler::last_time = 0;

BM_REGISTER_EXTERN(DR_PIFO_scheduler)
BM_REGISTER_EXTERN_METHOD(DR_PIFO_scheduler, my_scheduler, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&);

BM_REGISTER_EXTERN_METHOD(DR_PIFO_scheduler, pass_rank_values, const Data&, const Data&);

BM_REGISTER_EXTERN_METHOD(DR_PIFO_scheduler, pass_updated_rank_values, const Data&, const Data&, const Data&);

}  // namespace bm

int import_DR_PIFO(){
  return 0;
}
