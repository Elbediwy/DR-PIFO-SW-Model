#include "pieo.h"
#include <bm/bm_sim/logger.h>
#include <queue>

namespace bm {

std::vector<std::shared_ptr<pieo_scheduler::flow_scheduler>> pieo_scheduler::FS = { NULL};

std::vector<std::shared_ptr<pieo_scheduler::fifo_bank>> pieo_scheduler::FB = { NULL};

unsigned int pieo_scheduler::time_now = 0;
unsigned int pieo_scheduler::number_levels = 1;

std::vector<unsigned int> pieo_scheduler::number_of_queues_per_level = {1};
std::vector<unsigned int> pieo_scheduler::number_of_pkts_per_queue_each_level = {1024};

unsigned int sum_all_queues = pieo_scheduler::number_of_queues_per_level[0];

unsigned int number_of_update_ranks_all_level = (pieo_scheduler::number_of_pkts_per_queue_each_level[0]*pieo_scheduler::number_of_queues_per_level[0] * pieo_scheduler::number_levels);

std::vector<unsigned int> pieo_scheduler::new_ranks_each_level(number_of_update_ranks_all_level);

std::vector<unsigned int> pieo_scheduler::flow_id_not_empty_each_level(number_of_update_ranks_all_level);

unsigned int pieo_scheduler::use_updated_rank = 0;
unsigned int pieo_scheduler::shaping = 0;
unsigned int pieo_scheduler::number_of_enqueue_packets = 0;
std::vector<unsigned int> pieo_scheduler::pkt_levels_ranks = {0};
unsigned int pieo_scheduler::switch_is_ready = 1;
unsigned int pieo_scheduler::number_of_read_packets = 0;
unsigned int pieo_scheduler::number_of_dequeue_packets = 0;
std::queue<unsigned int> pieo_scheduler::pkt_ptr_queue;

int pieo_scheduler::start_time = 0;
int pieo_scheduler::last_time = 0;

BM_REGISTER_EXTERN(pieo_scheduler)
BM_REGISTER_EXTERN_METHOD(pieo_scheduler, my_scheduler, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&, const Data&);

BM_REGISTER_EXTERN_METHOD(pieo_scheduler, pass_rank_values, const Data&, const Data&);

BM_REGISTER_EXTERN_METHOD(pieo_scheduler, pass_updated_rank_values, const Data&, const Data&, const Data&);

}  // namespace bm

int import_pieo(){
  return 0;
}
