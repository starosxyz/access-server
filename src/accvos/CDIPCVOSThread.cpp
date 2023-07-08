/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#include "CDIPCVOSThread.h"
#include "fstack.h"
using namespace starbase;

CDIPCVOSThread::CDIPCVOSThread()
{
}
CDIPCVOSThread::~CDIPCVOSThread()
{
}
void CDIPCVOSThread::OnThreadInit()
{
}

int loop(void *arg)
{
/* Wait for events to happen */
	return 0;
}

char *dipcvnet_arg[50];
int dipcvnet_argc;

void CDIPCVOSThread::OnThreadRun()
{
	//CStar5GCoreConfig::Instance();
	//dipcvnet_arg[0]="star5gcore";
	//dipcvnet_arg[1]="-c";
	//dipcvnet_arg[2]="/opt/staros.xyz/star5gcore/conf/star5gcore.ini";
	fs_init(dipcvnet_argc, dipcvnet_arg);

	printf("run main thread\n");
    fs_run(loop, NULL);
    return;
}
	
CAWResult CDIPCVOSThread::Start(int argc, char **argv)
{
    int i=0;
    dipcvnet_argc=argc;
    for(i=0;i<argc; i++)
    {
        dipcvnet_arg[i]=strdup(argv[i]);
        //printf("\n%d-dipcvnet_arg[%d]=%s\n",argc,i,dipcvnet_arg[i]);
    }
    CAWResult rv = Create(CAWThreadManager::Instance()->GetAndIncreamUserType());
    if (CAW_FAILED(rv)) {
        return rv;
    }
	return CAW_OK;
}