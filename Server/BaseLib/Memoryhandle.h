#ifndef __RWSMEMORY_H_
#define __RWSMEMORY_H_

#define RWS_NEW						new

#define RWS_DELETE( p )				if ( p ) { delete p; p = NULL; } 
#define RWS_ARRAY_DELETE( p )		if ( p ) { delete [] p; p = NULL; }


#endif
