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

#ifndef _CSOCKET_H
#define _CSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "type.h"
#include "cstring.h"
#include "crbuff.h"
#include "cbytes.h"
#include "task.inc"

#include "taskcfg.h"
#include "taskcfg.inc"
#include "mod.inc"

#define CSOCKET_RECV_ONCE_MAX_SIZE       (32 * 1024) /*32KB*/
#define CSOCKET_SEND_ONCE_MAX_SIZE       (32 * 1024) /*32KB*/

#if (CSOCKET_RECV_ONCE_MAX_SIZE > CSOCKET_SORCVBUFF_SIZE)
#error "fatal error: CSOCKET_RECV_ONCE_MAX_SIZE > CSOCKET_SORCVBUFF_SIZE. pls check settting again!"
#endif

#if (CSOCKET_SEND_ONCE_MAX_SIZE > CSOCKET_SOSNDBUFF_SIZE)
#error "fatal error: CSOCKET_SEND_ONCE_MAX_SIZE > CSOCKET_SORCVBUFF_SIZE. pls check settting again!"
#endif

/*note: CSOCKET_CNODE_XCHG_TASKC_NODE = CSOCKET_CNODE_SENT_TASKC_NODE | CSOCKET_CNODE_RCVD_TASKC_NODE*/
#define CSOCKET_CNODE_NONA_TASKC_NODE       ((UINT32) 0)
#define CSOCKET_CNODE_SENT_TASKC_NODE       ((UINT32) 1)
#define CSOCKET_CNODE_RCVD_TASKC_NODE       ((UINT32) 2)
#define CSOCKET_CNODE_XCHG_TASKC_NODE       ((UINT32) 3)

#define CSOCKET_IS_BLOCK_MODE               ((UINT32) 1)
#define CSOCKET_IS_NONBLOCK_MODE            ((UINT32) 2)

#define CSOCKET_READING_FLAG                ((UINT32) 1)
#define CSOCKET_WRITING_FLAG                ((UINT32) 2)

#define CSOCKET_UDP_PACKET_DATA_SIZE        ((uint16_t)256)/*256 bytes*/
#define CSOCKET_UDP_PACKET_HEADER_SIZE      (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t))

struct _CDFS_BUFF;
struct _TASK_NODE;
struct _TASK_FUNC;

typedef struct _CSOCKET_CNODE
{    
    UINT32  ipaddr;
    UINT32  srvport;
    UINT32  tcid;
    UINT32  comm;
    UINT32  size;
    int     sockfd;
    int     rsvd;

    /*the status to exchange taskc_node with remote*/
    UINT32  status; /*counter for monitor client up to 2: send local taskc_node info to remote, recv remote taskc_node info from remote*/

    UINT32  load;  /*load of sending direction*/
    
    UINT8   pkt_hdr[32];
    UINT32  pkt_pos;/*when pos reaches csocket_encode_actual_size, header is ready*/

    struct _TASK_NODE *incoming_task_node; /*be incoming data*/
    struct _TASK_NODE *incomed_task_node; /*when no save_to_list recv it, mount here. used by tasks_monitor*/
    CLIST    sending_list;     /*be sending data*/
    CLIST    recving_list;     /*be recving data */
    
    TASKS_NODE *tasks_node; /*shortcut refer to its tasks node which is be appointed when move from monitor to work*/
}CSOCKET_CNODE;

#define CSOCKET_CNODE_IPADDR(csocket_cnode)              ((csocket_cnode)->ipaddr)
#define CSOCKET_CNODE_SRVPORT(csocket_cnode)             ((csocket_cnode)->srvport)
#define CSOCKET_CNODE_TCID(csocket_cnode)                ((csocket_cnode)->tcid)
#define CSOCKET_CNODE_TCID_STR(csocket_cnode)            (c_word_to_ipv4(CSOCKET_CNODE_TCID(csocket_cnode)))
#define CSOCKET_CNODE_COMM(csocket_cnode)                ((csocket_cnode)->comm)
#define CSOCKET_CNODE_SIZE(csocket_cnode)                ((csocket_cnode)->size)
#define CSOCKET_CNODE_SOCKFD(csocket_cnode)              ((csocket_cnode)->sockfd)
#define CSOCKET_CNODE_STATUS(csocket_cnode)              ((csocket_cnode)->status)

#define CSOCKET_CNODE_LOAD(csocket_cnode)                ((csocket_cnode)->load)

#define CSOCKET_CNODE_IPADDR_STR(csocket_cnode)          (c_word_to_ipv4(CSOCKET_CNODE_IPADDR(csocket_cnode)))
#define CSOCKET_CNODE_PKT_HDR(csocket_cnode)             ((csocket_cnode)->pkt_hdr)
#define CSOCKET_CNODE_PKT_HDR_BYTE(csocket_cnode, pos)   ((csocket_cnode)->pkt_hdr[pos])
#define CSOCKET_CNODE_PKT_POS(csocket_cnode)             ((csocket_cnode)->pkt_pos)
#define CSOCKET_CNODE_TASKS_NODE(csocket_cnode)          ((csocket_cnode)->tasks_node)

#define CSOCKET_CNODE_INCOMING_TASK_NODE(csocket_cnode)  ((csocket_cnode)->incoming_task_node)
#define CSOCKET_CNODE_INCOMED_TASK_NODE(csocket_cnode)   ((csocket_cnode)->incomed_task_node)
#define CSOCKET_CNODE_SENDING_LIST(csocket_cnode)        (&((csocket_cnode)->sending_list))
#define CSOCKET_CNODE_RECVING_LIST(csocket_cnode)        (&((csocket_cnode)->recving_list))

void csocket_tcpi_stat_print(LOG *log, const int sockfd);

void sockfd_print(LOG *log, const void *data);

void csocket_cnode_init(CSOCKET_CNODE *csocket_cnode);

void csocket_cnode_clean(CSOCKET_CNODE *csocket_cnode);

void csocket_cnode_init_0(const UINT32 md_id, CSOCKET_CNODE *csocket_cnode);

void csocket_cnode_clean_0(const UINT32 md_id, CSOCKET_CNODE *csocket_cnode);

void csocket_cnode_free_0(const UINT32 md_id, CSOCKET_CNODE *csocket_cnode);

CSOCKET_CNODE * csocket_cnode_new_0();

UINT32 csocket_cnode_clone_0(const CSOCKET_CNODE *csocket_cnode_src, CSOCKET_CNODE *csocket_cnode_des);

CSOCKET_CNODE * csocket_cnode_new(const UINT32 tcid, const int sockfd, const UINT32 ipaddr, const UINT32 srvport);

void csocket_cnode_free(CSOCKET_CNODE *csocket_cnode);

void csocket_cnode_close(CSOCKET_CNODE *csocket_cnode);

EC_BOOL csocket_cnode_cmp(const CSOCKET_CNODE *csocket_cnode_1, const CSOCKET_CNODE *csocket_cnode_2);

EC_BOOL csocket_cnode_irecv(CSOCKET_CNODE *csocket_cnode);

EC_BOOL csocket_cnode_is_connected(const CSOCKET_CNODE *csocket_cnode);

void csocket_cnode_print(LOG *log, const CSOCKET_CNODE *csocket_cnode);

EC_BOOL csocket_fd_clean(FD_CSET *sockfd_set);

EC_BOOL csocket_fd_set(const int sockfd, FD_CSET *sockfd_set, int *max_sockfd);

EC_BOOL csocket_fd_isset(const int sockfd, FD_CSET *sockfd_set);

EC_BOOL csocket_fd_clr(const int sockfd, FD_CSET *sockfd_set);

EC_BOOL csocket_fd_clone(FD_CSET *src_sockfd_set, FD_CSET *des_sockfd_set);

EC_BOOL csocket_client_addr_init( const UINT32 srv_ipaddr, const UINT32 srv_port, struct sockaddr_in *srv_addr);

EC_BOOL csocket_nonblock_enable(int sockfd);

EC_BOOL csocket_nonblock_disable(int sockfd);

EC_BOOL csocket_is_nonblock(const int sockfd);

EC_BOOL csocket_optimize(int sockfd);

EC_BOOL csocket_listen( const UINT32 srv_port, int *srv_sockfd );

EC_BOOL csocket_name(const int sockfd, CSTRING *ipaddr);

EC_BOOL csocket_connect( const UINT32 srv_ipaddr, const UINT32 srv_port, const UINT32 csocket_block_mode, int *client_sockfd );

UINT32 csocket_state(const int sockfd);

EC_BOOL csocket_is_established(const int sockfd);

EC_BOOL csocket_is_connected(const int sockfd);

EC_BOOL csocket_is_closed(const int sockfd);

EC_BOOL csocket_accept(const int srv_sockfd, int *conn_sockfd, UINT32 *client_ipaddr);

EC_BOOL csocket_start_udp_bcast_sender( const UINT32 bcast_fr_ipaddr, const UINT32 bcast_port, int *srv_sockfd );

EC_BOOL csocket_stop_udp_bcast_sender( const int sockfd );

EC_BOOL csocket_start_udp_bcast_recver( const UINT32 bcast_to_ipaddr, const UINT32 bcast_port, int *srv_sockfd );

EC_BOOL csocket_stop_udp_bcast_recver( const int sockfd );

EC_BOOL csocket_udp_bcast_send(const UINT32 bcast_fr_ipaddr, const UINT32 bcast_to_ipaddr, const UINT32 bcast_port, const UINT8 *data, const UINT32 dlen);

EC_BOOL csocket_udp_bcast_sendto(const int sockfd, const UINT32 bcast_to_ipaddr, const UINT32 bcast_port, const UINT8 *data, const UINT32 dlen);

EC_BOOL csocket_udp_bcast_recvfrom(const int sockfd, const UINT32 bcast_fr_ipaddr, const UINT32 bcast_port, UINT8 *data, const UINT32 max_dlen, UINT32 *dlen);

EC_BOOL csocket_start_udp_mcast_sender( const UINT32 mcast_ipaddr, const UINT32 srv_port, int *srv_sockfd );

EC_BOOL csocket_stop_udp_mcast_sender( const int sockfd, const UINT32 mcast_ipaddr );

EC_BOOL csocket_start_udp_mcast_recver( const UINT32 mcast_ipaddr, const UINT32 srv_port, int *srv_sockfd );

EC_BOOL csocket_stop_udp_mcast_recver( const int sockfd, const UINT32 mcast_ipaddr );

EC_BOOL csocket_join_mcast(const int sockfd, const UINT32 mcast_ipaddr);

EC_BOOL csocket_drop_mcast(const int sockfd, const UINT32 mcast_ipaddr);

EC_BOOL csocket_udp_mcast_sendto(const int sockfd, const UINT32 mcast_ipaddr, const UINT32 mcast_port, const UINT8 *data, const UINT32 dlen);

EC_BOOL csocket_udp_mcast_recvfrom(const int sockfd, const UINT32 mcast_ipaddr, const UINT32 mcast_port, UINT8 *data, const UINT32 max_dlen, UINT32 *dlen);

EC_BOOL csocket_udp_sendto(const int sockfd, const UINT32 mcast_ipaddr, const UINT32 mcast_port, const UINT8 *data, const UINT32 dlen);

EC_BOOL csocket_send_confirm(const int srv_sockfd);

EC_BOOL csocket_recv_confirm(const int srv_sockfd);

EC_BOOL csocket_select(const int sockfd_boundary, FD_CSET *read_sockfd_set, FD_CSET *write_sockfd_set, FD_CSET *except_sockfd_set, struct timeval *timeout, int *retval);

EC_BOOL csocket_shutdown( const int sockfd, const int flag );

int csocket_open(int domain, int type, int protocol);

EC_BOOL csocket_close( const int sockfd );

int csocket_errno();

EC_BOOL csocket_is_eagain();

EC_BOOL csocket_no_ierror();

EC_BOOL csocket_can_write(const int sockfd, int *ret);

EC_BOOL csocket_can_read(const int sockfd, int *ret);

EC_BOOL csocket_isend(const int sockfd, const UINT8 *out_buff, const UINT32 out_buff_max_len, UINT32 *position);

EC_BOOL csocket_irecv(const int sockfd, UINT8 *in_buff, const UINT32 in_buff_max_len, UINT32 *position);

EC_BOOL csocket_send(const int sockfd, const UINT8 *out_buff, const UINT32 out_buff_expect_len);

EC_BOOL csocket_recv(const int sockfd, UINT8 *in_buff, const UINT32 in_buff_expect_len);

EC_BOOL csocket_write(const int sockfd, const UINT8 *out_buff, const UINT32 out_buff_max_len, UINT32 *pos);

EC_BOOL csocket_read(const int sockfd, UINT8 *in_buff, const UINT32 in_buff_expect_len, UINT32 *pos);

EC_BOOL csocket_isend_task_node(CSOCKET_CNODE *csocket_cnode, struct _TASK_NODE *task_node);

EC_BOOL csocket_irecv_task_node(CSOCKET_CNODE *csocket_cnode, struct _TASK_NODE *task_node);

EC_BOOL csocket_send_uint32(const int sockfd, const UINT32 num);

EC_BOOL csocket_recv_uint32(const int sockfd, UINT32 *num);

EC_BOOL csocket_send_uint8(const int sockfd, const UINT8 num);

EC_BOOL csocket_recv_uint8(const int sockfd, UINT8 *num);

EC_BOOL csocket_send_cstring(const int sockfd, const CSTRING *cstring);

EC_BOOL csocket_recv_cstring(const int sockfd, CSTRING *cstring);

EC_BOOL csocket_send_cbytes(const int sockfd, const CBYTES *cbytes);

EC_BOOL csocket_recv_cbytes(const int sockfd, CBYTES *cbytes);


/*to fix a incomplete csocket_request, when complete, return EC_TRUE, otherwise, return EC_FALSE yet*/
EC_BOOL csocket_fix_task_node(const int sockfd, struct _TASK_NODE *task_node);

UINT32 xmod_node_encode_actual_size();

/*fetch a complete or incomplete csocket_request, caller should check the result*/
struct _TASK_NODE *csocket_fetch_task_node(CSOCKET_CNODE *csocket_cnode);

EC_BOOL csocket_discard_task_node_from(CLIST *clist, const UINT32 broken_tcid);

EC_BOOL csocket_discard_task_node_to(CLIST *clist, const UINT32 broken_tcid);

EC_BOOL csocket_srv_start( const UINT32 srv_port, const UINT32 csocket_block_mode, int *srv_sockfd );

EC_BOOL csocket_srv_end(const int srv_sockfd);

EC_BOOL csocket_client_start( const UINT32 srv_ipaddr, const UINT32 srv_port, const UINT32 csocket_block_mode, int *client_sockfd );

EC_BOOL csocket_client_end(const int client_sockfd);

UINT32 csocket_encode_actual_size();
UINT32 xmod_node_encode_actual_size();


EC_BOOL csocket_task_req_func_send(const int sockfd, struct _TASK_FUNC *task_req_func);

EC_BOOL csocket_task_req_func_recv(const int sockfd, struct _TASK_FUNC *task_req_func);

EC_BOOL csocket_task_rsp_func_send(const int sockfd, struct _TASK_FUNC *task_rsp_func);

EC_BOOL csocket_task_rsp_func_recv(const int sockfd, struct _TASK_FUNC *task_rsp_func);


#endif/*_CSOCKET_H*/

#ifdef __cplusplus
}
#endif/*__cplusplus*/

