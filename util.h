#ifndef UTIL_H
#define UTIL_H

class Trace{
public:

	template <class T>
	Trace& operator << (T val){
#ifdef DEBUG
		cout << val;
#endif
		return *this;
	}	

};

#endif