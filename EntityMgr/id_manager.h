#ifndef __ID_MANAGER_H
#define __ID_MANAGER_H

#include <set>

namespace SES{

	typedef unsigned int uint;

	class IDManager{
	public:
		IDManager(void):
			counter_(0)
		{}
		
		uint drawID(void){
			if(available_.empty()) return counter_++;
			auto first = available_.begin();
			uint ret_val = *first;
			available_.erase(first);
			return ret_val;
		}
		
		void releaseID(uint id){
			if(id < counter_) available_.insert(id);
		}
	
		/* Use with caution, tries to shrink the set available_ by finding the N last contiguous numbers */
		void defragment(void){
			auto itr = available_.crbegin();
			if(*itr == counter_ - 1){
				for(auto itr2 = itr++; itr != available_.crend(); ++itr, ++itr2){
					if(*itr != *itr2 - 1){
						counter_ = *itr2;
						available_.erase(--itr2.base(), available_.end());
						return;
					}
				}
			}
		}
		
	private:
		uint counter_;
		std::set<uint> available_;
	};
}
#endif