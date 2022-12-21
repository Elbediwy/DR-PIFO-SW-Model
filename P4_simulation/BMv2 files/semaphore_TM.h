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

namespace {

class TM_semaphore {

public:

static bool TM_key_available;
static std::vector<bool> pending_requests;

bool is_TM_available(unsigned int this_id, unsigned int other_id) // this is the id of the threads, ingress thread 0 (so at the ingress it will call "is_TM_available(0,1)"), egress thread 1 (so at the ingress it will call "is_TM_available(1,0)")
{
	if((TM_key_available == true) && (pending_requests[other_id] == false))
	{
		pending_requests[this_id] = false;
		TM_key_available = false;
		return true;
	}
	else if (pending_requests[other_id] == false) 
	{
		pending_requests[this_id] = true;
		return false;
	}
	{
		return false;
	}
}

void release_TM_key()
{
	TM_key_available = true;

}

};

}