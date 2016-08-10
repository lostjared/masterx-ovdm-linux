#ifdef _OLD_WAY_
#ifndef __MXPLUGIN__
#define __MXPLUGIN__
#include<mxfunction.h>
#include<string>
#include<vector>
#include<iostream>
#include<mx_exception.h>



namespace mxui {
	
	
	using std::vector;
	using std::string;
	
	
	class Plugin {
								
	public:
		
		Plugin() {
			create = 0;
			send =   0;
			quit_shared = 0;
		}
		
		void loadProgram(string name)
		{
#ifndef _WIN32
			std::cout <<   "Loading Module: " << name << "\n";
			if( shared.loadModule(name) == false) {
				
				throw mx::mxException<string>("[ mx ]: error loading shared module pluigin: "+name);
			}
			
			create = reinterpret_cast<p_create>(shared.loadFunction("CreateDimension"));
			if(create == 0)
				throw mx::mxException<string>("[ mx ]: error loading shared module pluigin: CreateDimension");
				
				std::cout << "[ mx ]  loaded function: void *(*)() \n";
				
				
			send = reinterpret_cast<p_send>(shared.loadFunction("EventsPassed"));
			if(send == 0)
				throw mx::mxException<string>("[ mx ]: error loading shared module pluigin: EventsPassed");
				
				std::cout << "[ mx ] loaded function void (*)(void *)  \n";
				
				
			quit_shared = reinterpret_cast<p_quit_shared>(shared.loadFunction("CloseDimension"));
			if(quit_shared == 0)
				throw mx::mxException<string>("[ mx ]: error loading shared module pluigin: CloseDimension");
				
				std::cout << "[ mx ] loaded function void  (*)()\n";
#endif
		}
		
		Plugin& operator=(const Plugin& p) {
			
			create = p.create;
			send = p.send;
			quit_shared = p.quit_shared;
			return *this;
		}
		
		Plugin(const Plugin& p)
		{
			this->operator=(p);
		}
		
	protected:
		mx::mxShared shared;
	public:
		typedef void *(*p_create)( int ); // Alloation process
		typedef void (*p_send)(void *); // message processing
		typedef void (*p_quit_shared)(); // termination
		
		p_create create;
		p_send send;
		p_quit_shared quit_shared;
		
	};
	
}

#endif

#endif

