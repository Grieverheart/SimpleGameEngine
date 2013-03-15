#ifndef __ID_MANAGER_H
#define __ID_MANAGER_H

#include <list>
#include <climits>
#include <ostream>

namespace SES{

	typedef unsigned int uint;

	namespace detail{
		struct Range{
			union{
				struct{
					uint l, r;
				};
				uint data_[2];
			};
			
			Range(void):
				l(0),
				r(UINT_MAX)
			{}
			
			Range(uint left, uint right):
				l(left),
				r(right)
			{}
			
			const uint& operator[](uint n)const{
				return data_[n & 1];
			}
			
			uint& operator[](uint n){
				return data_[n & 1];
			}
		};
	}

	class IDManager{
	public:
		IDManager(void):
			ranges_(1)
		{}
		
		uint drawID(void){
			uint returnID;
			detail::Range& range = ranges_.front();
			returnID = range.l++;
			//Careful: We pop front value while we have a reference 'range' to it
			if(range.l > range.r) ranges_.pop_front(); //It means the range only contained one number so we remove it
			return returnID;
		}
		
		void releaseID(uint id){
			//Check if the released id is the smallest available id
			auto front = ranges_.begin();
			if(id < front->l){
				//Check if it is smaller than the least available by 1
				if(id + 1 == front->l) front->l--;
				else ranges_.push_front(detail::Range(id,id));
				return;
			}
			//Else find where to insert it by checking if the id large than the maximum of each range
			for(auto itr = ranges_.begin(); itr != ranges_.end(); itr++){
				front++;
				if(id > itr->r && id < front->l){
					if(id > itr->r + 1 && id < front->l - 1){
						ranges_.insert(front, detail::Range(id, id));
						return;
					}
					//Here we check cases were the id is smaller or large by only 1, in these cases we just have to adjust the available ranges
					if(id == itr->r + 1) itr->r++;
					if(id + 1 == front->l) front->l--;
					if(itr->r == front->l) ranges_.erase(front);
					return;
				}
			}
		}
		
		void serialize(std::ostream& out){
			for(auto range: ranges_){
				out << "[" << range.l;
				if(range.r > range.l) out << "..." << range.r;
				out << "]" << std::endl;
			}
		}
		
	private:
		std::list<detail::Range> ranges_;
	};
}
#endif