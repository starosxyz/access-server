/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#ifndef CDIPCVOSTRETHREAD
#define CDIPCVOSTRETHREAD
#include <starbase/CAWStarBase.h>
#include <voskern/voskern.h>
using namespace starbase;

class CDIPCVOSThread: public CAWThread
{
public:
	CDIPCVOSThread();
	virtual ~CDIPCVOSThread();
    virtual void OnThreadInit();
    virtual void OnThreadRun();
public:
	CAWResult Start(int argc, char **argv);
};


#endif//CDIPCVOSTRETHREAD