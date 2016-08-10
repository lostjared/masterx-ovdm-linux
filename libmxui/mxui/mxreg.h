#ifndef _MX_REG_H__
#define _MX_REG_H__

#include<mxl.h>
#include<map>
#include<string>


namespace mxui {


	using std::map;
	using std::string;
	


	class mxReg {
			
	public:
		static mxReg app_registry;
		
		mxReg();
		
		void addMXL(mxlParser &p);
		
		map < string, map<string,string> >& table() { return regs; }	
	protected:
		map< string, map<string,string> > regs;
		
		
	
	};
















}



















#endif


