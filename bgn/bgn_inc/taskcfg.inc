/******************************************************************************
*
* Copyright (C) Chaoyong Zhou
* Email: bgnvendor@gmail.com 
* QQ: 312230917
*
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif/*__cplusplus*/

#ifndef _TASKCFG_INC
#define _TASKCFG_INC

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "type.h"
#include "clist.h"
#include "cstring.h"
#include "cvector.h"
#include "crouter.inc"


typedef fd_set FD_CSET;

#define SOCKFD_SET(fd_cset) (fd_cset)

#define BITS_TO_MASK(nbits)  (0 == (nbits)? 0 : ((0xffffffff) << (32 - (nbits))))

#define TASKS_CFG_DEFAULT_MASKI     ((UINT32) 32)
#define TASKS_CFG_DEFAULT_MASKE     ((UINT32) 16)

/*des_tcid belong to intranet of src_tcid: src & maski(src) == des & maski(src)*/
#define DES_TCID_IS_INTRANET(src_tcid, src_maski, des_tcid, des_maske) (((src_tcid) & (src_maski)) == ((des_tcid) & (des_maske)))

/*des_tcid belong to lannet of src_tcid: src & maske(src) == des & maske(des)*/
#define DES_TCID_IS_LANNET(src_tcid, src_maske, des_tcid, des_maske) (((src_tcid) & (src_maske)) == ((des_tcid) & (des_maske)))

/*des_tcid belong to extranet of src_tcid: src & maske(src) == des & maske(src)*/
#define DES_TCID_IS_EXTERNET(src_tcid, src_maske, des_tcid, des_maski) (((src_tcid) & (src_maske)) == ((des_tcid) & (des_maski)))

/*remote task server node*/
typedef struct
{
    UINT32  srvipaddr;   /*remote tasks node srv ipaddr*/
    UINT32  srvport;     /*remote tasks node srv port*/
    UINT32  tcid;        /*remote tasks node tcid*/
    UINT32  comm;        /*remote tasks node communicator*/
    UINT32  size;        /*remote tasks node communicator size*/

    UINT32  load;        /*load of sending direction*/

    CTIMET  last_update_time;   /*last update time. update when send or recv heartbeat successfully*/
    CTIMET  last_send_time;     /*last send heartbeat time*/

    CVECTOR csocket_cnode_vec;/*csocket cnodes connecting to remote tasks node*/
}TASKS_NODE;

#define TASKS_NODE_SRVIPADDR(tasks_node)                    ((tasks_node)->srvipaddr)
#define TASKS_NODE_SRVIPADDR_STR(tasks_node)                (c_word_to_ipv4(TASKS_NODE_SRVIPADDR(tasks_node)))
#define TASKS_NODE_SRVPORT(tasks_node)                      ((tasks_node)->srvport)
#define TASKS_NODE_TCID(tasks_node)                         ((tasks_node)->tcid)
#define TASKS_NODE_TCID_STR(tasks_node)                     (c_word_to_ipv4(TASKS_NODE_TCID(tasks_node)))
#define TASKS_NODE_COMM(tasks_node)                         ((tasks_node)->comm)
#define TASKS_NODE_SIZE(tasks_node)                         ((tasks_node)->size)
#define TASKS_NODE_LOAD(tasks_node)                         ((tasks_node)->load)
#define TASKS_NODE_LAST_UPDATE_TIME(tasks_node)             (((tasks_node)->last_update_time))
#define TASKS_NODE_LAST_SEND_TIME(tasks_node)               (((tasks_node)->last_send_time))
#define TASKS_NODE_CSOCKET_CNODE_VEC(tasks_node)            (&((tasks_node)->csocket_cnode_vec))

/*TASK SERVER CONFIG*/
typedef struct
{
    UINT32   tcid;        /*taskcomm id              */
    UINT32   maski;       /*taskcomm mask for internal network,including sub network(sons) and LAN network(brothers)*/
    UINT32   maske;       /*taskcomm mask for external network,including upper network(parent)*/

    UINT32   srvipaddr;   /*taskcomm srv ipaddr, for local or remote*/
    UINT32   srvport;     /*taskcomm srv port  , for local or remote*/

    UINT32   csrvport;    /*when valid, will start csrv on it*/
    CVECTOR  cluster_vec; /*belong to these clusters, item is cluster id (UINT32)*/   
#if 0
    union
    {
        UINT32      remote; /*reserved*/

        struct
        {
            int      srvsockfd;          /*[ONLY FOR LOCAL TASKS CFG] local taskcomm srv listening sockfd */
            int      rsved;
            
            CVECTOR  tasks_node_work;    /*[ONLY FOR LOCAL TASKS CFG] working clients: remote taskcomm connection to local taskcomm : tasks_node vector  */
            CVECTOR  tasks_node_monitor; /*[ONLY FOR LOCAL TASKS CFG] monitoring clients: remote taskcomm connection to local taskcomm: csocket_cnode vector*/
        }local;
    }u;
#endif    

#if 1
    int      srvsockfd;          /*[ONLY FOR LOCAL TASKS CFG] local taskcomm srv listening sockfd */
    int      rsved;
    
    CVECTOR  tasks_node_work;    /*[ONLY FOR LOCAL TASKS CFG] working clients: remote taskcomm connection to local taskcomm : tasks_node vector  */
    CVECTOR  tasks_node_monitor; /*[ONLY FOR LOCAL TASKS CFG] monitoring clients: remote taskcomm connection to local taskcomm: csocket_cnode vector*/
#endif

    CVECTOR  taskr_cfg_vec;
}TASKS_CFG;

#define TASKS_CFG_TCID(tasks_cfg)                  ((tasks_cfg)->tcid)
#define TASKS_CFG_TCID_STR(tasks_cfg)              (c_word_to_ipv4(TASKS_CFG_TCID(tasks_cfg)))
#define TASKS_CFG_MASKI(tasks_cfg)                 ((tasks_cfg)->maski)
#define TASKS_CFG_MASKI_STR(tasks_cfg)             (c_word_to_ipv4(TASKS_CFG_MASKI(tasks_cfg)))
#define TASKS_CFG_MASKE(tasks_cfg)                 ((tasks_cfg)->maske)
#define TASKS_CFG_MASKE_STR(tasks_cfg)             (c_word_to_ipv4(TASKS_CFG_MASKE(tasks_cfg)))
#define TASKS_CFG_SRVIPADDR(tasks_cfg)             ((tasks_cfg)->srvipaddr)
#define TASKS_CFG_SRVIPADDR_STR(tasks_cfg)         (c_word_to_ipv4(TASKS_CFG_SRVIPADDR(tasks_cfg)))
#define TASKS_CFG_SRVPORT(tasks_cfg)               ((tasks_cfg)->srvport)
#define TASKS_CFG_CSRVPORT(tasks_cfg)              ((tasks_cfg)->csrvport)
#define TASKS_CFG_CLUSTER_VEC(tasks_cfg)           (&((tasks_cfg)->cluster_vec))
#if 0
#define TASKS_CFG_SRVSOCKFD(tasks_cfg)             ((tasks_cfg)->u.local.srvsockfd)
#define TASKS_CFG_WORK(tasks_cfg)                  (&((tasks_cfg)->u.local.tasks_node_work))
#define TASKS_CFG_MONITOR(tasks_cfg)               (&((tasks_cfg)->u.local.tasks_node_monitor))
#endif
#if 1
#define TASKS_CFG_SRVSOCKFD(tasks_cfg)             ((tasks_cfg)->srvsockfd)
#define TASKS_CFG_WORK(tasks_cfg)                  (&((tasks_cfg)->tasks_node_work))
#define TASKS_CFG_MONITOR(tasks_cfg)               (&((tasks_cfg)->tasks_node_monitor))
#endif
#define TASKS_CFG_TASKR_CFG_VEC(tasks_cfg)         (&(tasks_cfg->taskr_cfg_vec))

#if 0
#define TASKS_WORK_BASE_TASKS_CFG_ENTRY(tasks_work_ptr) \
    ((TASKS_CFG *)((char *)(tasks_work_ptr)-(unsigned long)(&((TASKS_CFG *)0)->u.local.tasks_node_work)))
#endif
#if 1
#define TASKS_WORK_BASE_TASKS_CFG_ENTRY(tasks_work_ptr) \
    ((TASKS_CFG *)((char *)(tasks_work_ptr)-(unsigned long)(&((TASKS_CFG *)0)->tasks_node_work)))

#endif

/*TASK ROUTE CONFIG*/
typedef struct
{
    UINT32  des_tcid;        /*des remote taskcomm id   */
    UINT32  maskr;           /*route mask               */
    UINT32  next_tcid;       /*next remote taskcomm id  */
}TASKR_CFG;

#define TASKR_CFG_DES_TCID(taskr_cfg)              ((taskr_cfg)->des_tcid)
#define TASKR_CFG_DES_TCID_STR(taskr_cfg)          (c_word_to_ipv4(TASKR_CFG_DES_TCID(taskr_cfg)))
#define TASKR_CFG_NEXT_TCID(taskr_cfg)             ((taskr_cfg)->next_tcid)
#define TASKR_CFG_NEXT_TCID_STR(taskr_cfg)         (c_word_to_ipv4(TASKR_CFG_NEXT_TCID(taskr_cfg)))
#define TASKR_CFG_MASKR(taskr_cfg)                 ((taskr_cfg)->maskr)
#define TASKR_CFG_MASKR_STR(taskr_cfg)             (c_word_to_ipv4(TASKR_CFG_MASKR(taskr_cfg)))

/*TASK CONFIG*/
typedef struct
{
    UINT32   default_tasks_port;/*default server port for all tasks_cfg*/
    CVECTOR  tasks_cfg_vec;/*item type is TASKS_CFG*/
}TASK_CFG;

#define TASK_CFG_DEFAULT_TASKS_PORT(task_cfg)      ((task_cfg)->default_tasks_port)
#define TASK_CFG_TASKS_CFG_VEC(task_cfg)           (&((task_cfg)->tasks_cfg_vec))

/*TASK COMM*/
typedef struct
{
    UINT32 tcid;
    UINT32 comm;
    UINT32 rank;

    UINT32 modi;
}TASKC_ADDR;

#define TASKC_ADDR_TCID(taskc_addr)            ((taskc_addr)->tcid)
#define TASKC_ADDR_COMM(taskc_addr)            ((taskc_addr)->comm)
#define TASKC_ADDR_RANK(taskc_addr)            ((taskc_addr)->rank)
#define TASKC_ADDR_MODI(taskc_addr)            ((taskc_addr)->modi)

#define SEND_TASKC_ADDR_TCID(send_taskc_addr)       (TASKC_ADDR_TCID(send_taskc_addr))
#define SEND_TASKC_ADDR_TCID_STR(send_taskc_addr)   (c_word_to_ipv4(SEND_TASKC_ADDR_TCID(send_taskc_addr)))
#define SEND_TASKC_ADDR_COMM(send_taskc_addr)       (TASKC_ADDR_COMM(send_taskc_addr))
#define SEND_TASKC_ADDR_RANK(send_taskc_addr)       (TASKC_ADDR_RANK(send_taskc_addr))
#define SEND_TASKC_ADDR_MODI(send_taskc_addr)       (TASKC_ADDR_MODI(send_taskc_addr))

/*
#define TASKS_WORK_INCOMING_REQUESTS(tasks_work)         (&((tasks_work)->incoming_list))
#define TASKS_WORK_INCOMED_REQUESTS(tasks_work)          (&((tasks_work)->incomed_list))
#define TASKS_WORK_SENDING_REQUESTS(tasks_work)          (&((tasks_work)->sending_list))
#define TASKS_WORK_RECVING_REQUESTS(tasks_work)          (&((tasks_work)->recving_list))
#define TASKS_WORK_INCOMING_NODES(tasks_work)            (&((tasks_work)->incoming_list))
#define TASKS_WORK_INCOMED_NODES(tasks_work)             (&((tasks_work)->incomed_list))
#define TASKS_WORK_SENDING_NODES(tasks_work)             (&((tasks_work)->sending_list))
#define TASKS_WORK_RECVING_NODES(tasks_work)             (&((tasks_work)->recving_list))
#define TASKS_WORK_TASKS_NODE_VEC(tasks_work)            (&((tasks_work)->tasks_node_vec))

#define TASKS_MONITOR_INCOMING_REQUESTS(tasks_monitor)         (&((tasks_monitor)->incoming_list))
#define TASKS_MONITOR_INCOMED_REQUESTS(tasks_monitor)          (&((tasks_monitor)->incomed_list))
#define TASKS_MONITOR_SENDING_REQUESTS(tasks_monitor)          (&((tasks_monitor)->sending_list))
#define TASKS_MONITOR_RECVING_REQUESTS(tasks_monitor)          (&((tasks_monitor)->recving_list))
#define TASKS_MONITOR_CSOCKET_CNODE_VEC(tasks_monitor)         (&((tasks_monitor)->csocket_cnode_vec))
*/


/*
#define RECV_TASKC_ADDR_TCID(recv_taskc_addr)       (TASKC_ADDR_TCID(recv_taskc_addr))
#define RECV_TASKC_ADDR_TCID_STR(recv_taskc_addr)   (c_word_to_ipv4(RECV_TASKC_ADDR_TCID(recv_taskc_addr)))
#define RECV_TASKC_ADDR_COMM(recv_taskc_addr)       (TASKC_ADDR_COMM(recv_taskc_addr))
#define RECV_TASKC_ADDR_RANK(recv_taskc_addr)       (TASKC_ADDR_RANK(recv_taskc_addr))
#define RECV_TASKC_ADDR_MODI(recv_taskc_addr)       (TASKC_ADDR_MODI(recv_taskc_addr))
*/

#endif /*_TASKCFG_INC*/

#ifdef __cplusplus
}
#endif/*__cplusplus*/

