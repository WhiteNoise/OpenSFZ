//
//  SFZCommon.cpp
//  OpenSFZ
//
//  Created by David Wallin on 4/25/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#include "SFZCommon.h"

float decibelsToGain(float db)
{
    return powf(10.0f, db / 20.0f);
}

float getMidiNoteInHertz(int noteNumber)
{
    noteNumber -= 12 * 6 + 9; // now 0 = A
    return 440.0f * pow (2.0, noteNumber / 12.0);
}

void sfzDebugPrint(const char *sz)
{
#ifdef DEBUG
    printf(sz);
#endif
}

#ifdef __APPLE__

#import <mach/mach.h>
#import <mach/mach_host.h>

void print_free_memory ()
{
#ifdef DEBUG
    mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_size_t pagesize;
    
    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    host_page_size(host_port, &pagesize);
    
    vm_statistics_data_t vm_stat;
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
        printf("Failed to fetch vm statistics");
    
    /* Stats in bytes */
    natural_t mem_used = (vm_stat.active_count +
                          vm_stat.inactive_count +
                          vm_stat.wire_count) * pagesize;
    natural_t mem_free = vm_stat.free_count * pagesize;
    natural_t mem_total = mem_used + mem_free;
    char szBuff[256];
    sprintf(szBuff, "used: %u free: %u total: %u\n", mem_used, mem_free, mem_total);
    sfzDebugPrint(szBuff);
#endif
}

unsigned int getFreeMemory()
{
    mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_size_t pagesize;
    
    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    host_page_size(host_port, &pagesize);
    
    vm_statistics_data_t vm_stat;
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
        printf("Failed to fetch vm statistics");
    

    natural_t mem_free = vm_stat.free_count * pagesize;
    
    return (unsigned int)mem_free;
}

#endif

#ifdef _WIN32_

#include <windows.h>


void print_free_memory ()
{
#ifdef DEBUG
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    char szBuff[256];
    sprintf(szBuff, "available: %u total: %u\n", status.ullAvailPhys, status.ullTotalPhys);
    sfzDebugPrint(szBuff);
#endif
}

unsigned int getFreeMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    
    return status.ullAvailPhys;
}

#endif