//
//  main.cpp
//  CPU Scheduling
//
//  Created by Preston Kemp on 8/13/19.
//  Copyright © 2019 Preston Kemp. All rights reserved.
//

#include "oslabs.h"

//struct PCB {
//    int process_id;
//    int arrival_timestamp;
//    int total_bursttime;
//    int execution_starttime;
//    int execution_endtime;
//    int remaining_bursttime;
//    int process_priority;
//};
//
//const int QUEUEMAX = 100;

struct PCB readyqueue[30];

const struct PCB NULLPCB = {0,0,0,0,0,0,0};

void remove_element(struct PCB ready_queue[QUEUEMAX], int index, int array_length)
{
    int i;
    for(i = index; i < array_length - 1; i++) ready_queue[i] = ready_queue[i + 1];
}


int check_if_null_pcb(struct PCB pcb)
{
    if(pcb.process_id == NULLPCB.process_id &&
       pcb.arrival_timestamp == NULLPCB.arrival_timestamp &&
       pcb.total_bursttime == NULLPCB.total_bursttime &&
       pcb.execution_starttime == NULLPCB.execution_starttime &&
       pcb.execution_endtime == NULLPCB.execution_endtime) {
        return 1;
    }
    return 0;
}

////Checks to see if arriving PCB should replace the current PCB
//bool check_priority(PCB current, PCB arriving)
//{
//    return false;
//}

struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *readyQueueSize, struct PCB currentPCB, struct PCB arrivingPCB, int timestamp)
{
    //int readyQueueSize = *readyQueueCount;
    struct PCB pcb = currentPCB;
    if(check_if_null_pcb(currentPCB)){
        arrivingPCB.execution_starttime = timestamp;
        arrivingPCB.arrival_timestamp = timestamp;
        arrivingPCB.execution_endtime = arrivingPCB.execution_starttime + arrivingPCB.total_bursttime;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        pcb = arrivingPCB;
        //assign current PCB to be arriving PCB
    }
    else if (currentPCB.process_priority <= arrivingPCB.process_priority){
        ready_queue[*readyQueueSize-1] = arrivingPCB;
        ready_queue[*readyQueueSize-1].execution_starttime = 0;
        ready_queue[*readyQueueSize-1].execution_endtime = 0;
        ready_queue[*readyQueueSize-1].remaining_bursttime = arrivingPCB.total_bursttime;
        (*readyQueueSize)++;
        pcb = currentPCB;
    }
    else {
        currentPCB.remaining_bursttime = currentPCB.execution_endtime - timestamp;
        currentPCB.execution_endtime = 0;
        (*readyQueueSize)++;
        ready_queue[*readyQueueSize-1] = currentPCB;
        arrivingPCB.execution_starttime = timestamp;
        arrivingPCB.execution_endtime = timestamp + arrivingPCB.total_bursttime;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        pcb = arrivingPCB;
        
    }
    return pcb;
}

struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp)
{
    int highestPriorityIndex = 0;
    if(*queue_cnt != 0)
    {
        for (int i = 0; i < *queue_cnt; i++)
        {
            if(ready_queue[i].process_priority < ready_queue[highestPriorityIndex].process_priority)
            {
                highestPriorityIndex = i;
            }
        }
        ready_queue[highestPriorityIndex].execution_starttime = timestamp;
        ready_queue[highestPriorityIndex].execution_endtime = timestamp + ready_queue[highestPriorityIndex].remaining_bursttime;
        struct PCB next = ready_queue[highestPriorityIndex];
        remove_element(ready_queue, highestPriorityIndex, *queue_cnt);
        //        ready_queue[earliestArrivalTimeIndex] = NULLPCB;
        (*queue_cnt)--;
        
        return next;
    }
    else {
        return NULLPCB;
    }
}

//the method compares the remaining burst time of the
//currently-running process and the total burst time of the newly-arriving process. If the new
//process does not have a shorter burst time, then its PCB is simply added to the ready queue
//and the return value is the PCB of the currently running process

struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB currentPCB, struct PCB arrivingPCB, int timestamp)
{
    //int readyQueueSize = *readyQueueCount;
    struct PCB pcb = currentPCB;
    if(check_if_null_pcb(currentPCB)){
        arrivingPCB.execution_starttime = timestamp;
        arrivingPCB.arrival_timestamp = timestamp;
        arrivingPCB.execution_endtime = arrivingPCB.execution_starttime + arrivingPCB.total_bursttime;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        pcb = arrivingPCB;
        //assign current PCB to be arriving PCB
    }
    
    //if current remaining burst time is less than total burst time for the arriving process, then add the arriving process to the ready queue
    else if (arrivingPCB.total_bursttime < currentPCB.remaining_bursttime){
        arrivingPCB.execution_starttime = timestamp;
        arrivingPCB.execution_endtime = timestamp + arrivingPCB.total_bursttime;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        currentPCB.remaining_bursttime = currentPCB.execution_endtime - timestamp;
        currentPCB.execution_endtime = 0;
        currentPCB.execution_starttime = 0;
        pcb = arrivingPCB;
        ready_queue[*queue_cnt] = currentPCB;
        (*queue_cnt)++;
    }
    //else we assume the burst time of the arriving process is less than that of the currently running process
    else {
        arrivingPCB.execution_starttime = 0;
        arrivingPCB.execution_endtime = 0;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        ready_queue[*queue_cnt] = arrivingPCB;
        (*queue_cnt)++;
    }
    return pcb;
}

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *readyQueueSize, int timestamp)
{
    int lowestBurstTimeIndex = 0;
    if(*readyQueueSize != 0)
    {
        for (int i = 0; i < *readyQueueSize; i++)
        {
            if(!check_if_null_pcb(ready_queue[i])){
                if(ready_queue[i].remaining_bursttime < ready_queue[lowestBurstTimeIndex].remaining_bursttime)
                {
                    lowestBurstTimeIndex = i;
                }
            }
        }
        ready_queue[lowestBurstTimeIndex].execution_starttime = timestamp;
        ready_queue[lowestBurstTimeIndex].execution_endtime = timestamp + ready_queue[lowestBurstTimeIndex].remaining_bursttime;
        struct PCB next = ready_queue[lowestBurstTimeIndex];
        //ready_queue[lowestBurstTimeIndex] = NULLPCB;
        remove_element(ready_queue, lowestBurstTimeIndex, *readyQueueSize);
        --*readyQueueSize;
        return next;
    }
    else {
        return NULLPCB;
    }
}

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *readyQueueSize, struct PCB currentPCB, struct PCB arrivingPCB, int timestamp, int timequantum)
{
    //int readyQueueSize = *readyQueueCount;
    struct PCB pcb = currentPCB;
    if(check_if_null_pcb(currentPCB) == 1){
        if(arrivingPCB.remaining_bursttime < timequantum) {
            arrivingPCB.execution_endtime = timestamp + arrivingPCB.remaining_bursttime;
        }
        else {
            arrivingPCB.execution_endtime = timestamp + timequantum;
        }
        arrivingPCB.execution_starttime = timestamp;
        arrivingPCB.arrival_timestamp = timestamp;
        arrivingPCB.execution_endtime = arrivingPCB.execution_starttime + arrivingPCB.total_bursttime;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        pcb = arrivingPCB;
        //assign current PCB to be arriving PCB
    }
    else {
        ready_queue[*readyQueueSize] = arrivingPCB;
        //increment the readyqueue size
        ++*readyQueueSize;
        arrivingPCB.execution_starttime = 0;
        arrivingPCB.execution_endtime = 0;
        arrivingPCB.remaining_bursttime = arrivingPCB.total_bursttime;
        //return the currentPCB since the arriving PCB burst time is not less than the current's
        pcb = currentPCB;
    }
    return pcb;
}

struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *readyQueueSize, int timestamp, int timequantum)
{
    int earliestArrivalTimeIndex = 0;
    if(*readyQueueSize != 0)
    {
        for (int i = 0; i < *readyQueueSize; i++)
        {
            if(ready_queue[i].arrival_timestamp < ready_queue[earliestArrivalTimeIndex].arrival_timestamp)
            {
                earliestArrivalTimeIndex = i;
            }
        }
        ready_queue[earliestArrivalTimeIndex].execution_starttime = timestamp;
        ready_queue[earliestArrivalTimeIndex].execution_endtime = timestamp + ready_queue[earliestArrivalTimeIndex].remaining_bursttime;
        struct PCB next = ready_queue[earliestArrivalTimeIndex];
        remove_element(ready_queue, earliestArrivalTimeIndex, *readyQueueSize);
//        ready_queue[earliestArrivalTimeIndex] = NULLPCB;
        --*readyQueueSize;
        return next;
    }
    else {
        return NULLPCB;
    }
}

