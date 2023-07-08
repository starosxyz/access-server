/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/

#ifndef STAROS5GCORE_EXTOS_H
#define STAROS5GCORE_EXTOS_H

#include "xos/xos-core.h"

#ifdef __cplusplus
extern "C" {
#endif
int extos_vport_open(void);
int extos_vport_close(void);
void extos_vport_input_eth_cb(const char* data, size_t datasize);
#ifdef __cplusplus
}
#endif

#endif /* STAROS5GCORE_EXTOS_H */