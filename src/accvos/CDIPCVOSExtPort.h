/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#ifndef CDIPCVOSEXTPORT_H
#define CDIPCVOSEXTPORT_H
#include <starbase/CAWStarBase.h>
#include <voskern/voskern.h>
using namespace starbase;

class CDIPCVOSExtPort
{
public:
	CDIPCVOSExtPort();
	virtual ~CDIPCVOSExtPort();

public:
	CAWResult ExtTapInit(const CAWString &ext_tap_name,
        const CAWString &ext_tap_gateway,
        CAWString ext_tap_mask = "255.255.0.0",
        CAWString ext_tap_ip = "0.0.0.0");
    
	void ExtTapInput(const char* data, size_t datasize);
	virtual void ExtTapOutput(const char* data, size_t datasize) = 0;

private:
	struct vos_veth* m_extosveth;
    CAWString m_ext_tap_name;
    CAWString m_ext_tap_ip;
    CAWString m_ext_tap_mask;
    CAWString m_ext_tap_gateway;
	/* DIPCVNET *m_dipc */
};


#endif//CDIPCVOSEXTPORT_H