#include "mxreg.h"




namespace mxui {

	mxReg mxReg::app_registry;

	mxReg::mxReg() {
		
		regs.clear();
		
	}
		
	void mxReg::addMXL(mxlParser &p) {
		
		std::cout << "loading data to the registry...\n";
	
		if(p.size() == 0) return;
		
		mxlArray varray;
		p.fetchArray(varray);
			
			for(unsigned int i = 0; i < varray.size(); i++) {
		    
				string key = varray[i].first;
		        
				for(subtag tag = p.begin_mxl(i, varray); tag != p.end_mxl(i, varray); tag++)  {
		        	
		        	if(regs[key][tag->first] ==  tag->second) {
		        		std::cout << "* - [ mxui ] data already in registry\n"; 
		        		return;
		        	}
		        	
		        	std::cout << "MX System Registry: adding key: " << key << " and entry: " << tag->first <<  " [ completed ]\n";
		        	regs[key][tag->first] = tag->second;        	
		        	}
		               
				}	
	
		}

}