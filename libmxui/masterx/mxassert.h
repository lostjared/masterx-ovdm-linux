#ifndef __ASSERT_H_
#define __ASSERT_H_
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include<cassert>
#ifndef ASSERT_OFF
#define __assert__(x) assert(x)
#else
#define __assert__(x)
#endif
#endif


