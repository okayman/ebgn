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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>

#include <sys/stat.h>

#include "type.h"
#include "mm.h"
#include "log.h"
#include "cstring.h"

#include "carray.h"
#include "cvector.h"

#include "cbc.h"

#include "cmisc.h"

#include "task.h"

#include "csocket.h"

#include "cmpie.h"

#include "chfs.h"
#include "cload.h"

#include "cfuse.h"

#include "findex.inc"

#define CHFS_MD_CAPACITY()                  (cbc_md_capacity(MD_CHFS))

#define CHFS_MD_GET(chfs_md_id)     ((CHFS_MD *)cbc_md_get(MD_CHFS, (chfs_md_id)))

#define CHFS_MD_ID_CHECK_INVALID(chfs_md_id)  \
    ((CMPI_ANY_MODI != (chfs_md_id)) && ((NULL_PTR == CHFS_MD_GET(chfs_md_id)) || (0 == (CHFS_MD_GET(chfs_md_id)->usedcounter))))

/**
*   for test only
*
*   to query the status of CHFS Module
*
**/
void chfs_print_module_status(const UINT32 chfs_md_id, LOG *log)
{
    CHFS_MD *chfs_md;
    UINT32 this_chfs_md_id;

    for( this_chfs_md_id = 0; this_chfs_md_id < CHFS_MD_CAPACITY(); this_chfs_md_id ++ )
    {
        chfs_md = CHFS_MD_GET(this_chfs_md_id);

        if ( NULL_PTR != chfs_md && 0 < chfs_md->usedcounter )
        {
            sys_log(log,"CHFS Module # %u : %u refered\n",
                    this_chfs_md_id,
                    chfs_md->usedcounter);
        }
    }

    return ;
}

/**
*
*   free all static memory occupied by the appointed CHFS module
*
*
**/
UINT32 chfs_free_module_static_mem(const UINT32 chfs_md_id)
{
    CHFS_MD  *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_free_module_static_mem: chfs module #0x%lx not started.\n",
                chfs_md_id);
        /*note: here do not exit but return only*/
        return ((UINT32)-1);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    free_module_static_mem(MD_CHFS, chfs_md_id);

    return 0;
}

/**
*
* start CHFS module
*
**/
UINT32 chfs_start(const CSTRING *chfsnp_root_dir, const CSTRING *crfsdn_root_dir)
{
    CHFS_MD *chfs_md;
    UINT32   chfs_md_id;

    TASK_BRD *task_brd;
    EC_BOOL   ret;

    task_brd = task_brd_default_get();
    
    chfs_md_id = cbc_md_new(MD_CHFS, sizeof(CHFS_MD));
    if(ERR_MODULE_ID == chfs_md_id)
    {
        return (ERR_MODULE_ID);
    }

    /* initilize new one CHFS module */
    chfs_md = (CHFS_MD *)cbc_md_get(MD_CHFS, chfs_md_id);
    chfs_md->usedcounter   = 0;

    /* create a new module node */
    init_static_mem();    

    CHFS_MD_DN_MOD_MGR(chfs_md)  = mod_mgr_new(chfs_md_id, /*LOAD_BALANCING_LOOP*//*LOAD_BALANCING_MOD*/LOAD_BALANCING_QUE);
    CHFS_MD_NPP_MOD_MGR(chfs_md) = mod_mgr_new(chfs_md_id, /*LOAD_BALANCING_LOOP*//*LOAD_BALANCING_MOD*/LOAD_BALANCING_QUE);
    
    CHFS_MD_DN(chfs_md)  = NULL_PTR;
    CHFS_MD_NPP(chfs_md) = NULL_PTR;

    ret = EC_TRUE;
    if(EC_TRUE  == ret && NULL_PTR != chfsnp_root_dir 
    && EC_FALSE == cstring_is_empty(chfsnp_root_dir) 
    && EC_TRUE  == chfsnp_mgr_exist(chfsnp_root_dir))
    {
        CHFS_MD_NPP(chfs_md) = chfsnp_mgr_open(chfsnp_root_dir);
        if(NULL_PTR == CHFS_MD_NPP(chfs_md))
        {
            sys_log(LOGSTDOUT, "error:chfs_start: open npp from root dir %s failed\n", (char *)cstring_get_str(chfsnp_root_dir));
            ret = EC_FALSE;
        }
    }

    if(EC_TRUE  == ret && NULL_PTR != crfsdn_root_dir 
    && EC_FALSE == cstring_is_empty(crfsdn_root_dir) 
    && EC_TRUE  == crfsdn_exist((char *)cstring_get_str(crfsdn_root_dir)))
    {
        CHFS_MD_DN(chfs_md) = crfsdn_open((char *)cstring_get_str(crfsdn_root_dir));
        if(NULL_PTR == CHFS_MD_DN(chfs_md))
        {
            sys_log(LOGSTDOUT, "error:chfs_start: open dn with root dir %s failed\n", (char *)cstring_get_str(crfsdn_root_dir));
            ret = EC_FALSE;
        }    
    }

    if(EC_FALSE == ret)
    {
        if(NULL_PTR != CHFS_MD_DN(chfs_md))
        {
            crfsdn_close(CHFS_MD_DN(chfs_md));
            CHFS_MD_DN(chfs_md) = NULL_PTR;
        }

        if(NULL_PTR != CHFS_MD_NPP(chfs_md))
        {
            chfsnp_mgr_close(CHFS_MD_NPP(chfs_md));
            CHFS_MD_NPP(chfs_md) = NULL_PTR;
        }    
        cbc_md_free(MD_CHFS, chfs_md_id);
        return (ERR_MODULE_ID);
    }

    chfs_md->usedcounter = 1;

    sys_log(LOGSTDOUT, "chfs_start: start CHFS module #%u\n", chfs_md_id);

    return ( chfs_md_id );
}

/**
*
* end CHFS module
*
**/
void chfs_end(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

    chfs_md = CHFS_MD_GET(chfs_md_id);
    if(NULL_PTR == chfs_md)
    {
        sys_log(LOGSTDOUT,"error:chfs_end: chfs_md_id = %u not exist.\n", chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
    /* if the module is occupied by others,then decrease counter only */
    if ( 1 < chfs_md->usedcounter )
    {
        chfs_md->usedcounter --;
        return ;
    }

    if ( 0 == chfs_md->usedcounter )
    {
        sys_log(LOGSTDOUT,"error:chfs_end: chfs_md_id = %u is not started.\n", chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }

    /* if nobody else occupied the module,then free its resource */
    if(NULL_PTR != CHFS_MD_DN(chfs_md))
    {
        crfsdn_close(CHFS_MD_DN(chfs_md));
        CHFS_MD_DN(chfs_md) = NULL_PTR;
    }

    if(NULL_PTR != CHFS_MD_NPP(chfs_md))
    {
        chfsnp_mgr_close(CHFS_MD_NPP(chfs_md));
        CHFS_MD_NPP(chfs_md) = NULL_PTR;
    }

    if(NULL_PTR != CHFS_MD_DN_MOD_MGR(chfs_md))
    {
        mod_mgr_free(CHFS_MD_DN_MOD_MGR(chfs_md));
        CHFS_MD_DN_MOD_MGR(chfs_md)  = NULL_PTR;
    }

    if(NULL_PTR != CHFS_MD_NPP_MOD_MGR(chfs_md))
    {
        mod_mgr_free(CHFS_MD_NPP_MOD_MGR(chfs_md));
        CHFS_MD_NPP_MOD_MGR(chfs_md)  = NULL_PTR;
    }
    
    /* free module : */
    //chfs_free_module_static_mem(chfs_md_id);

    chfs_md->usedcounter = 0;

    sys_log(LOGSTDOUT, "chfs_end: stop CHFS module #%u\n", chfs_md_id);
    cbc_md_free(MD_CHFS, chfs_md_id);

    return ;
}

/**
*
* initialize mod mgr of CHFS module
*
**/
UINT32 chfs_set_npp_mod_mgr(const UINT32 chfs_md_id, const MOD_MGR * src_mod_mgr)
{
    CHFS_MD *chfs_md;
    MOD_MGR  *des_mod_mgr;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_set_npp_mod_mgr: chfs module #0x%lx not started.\n",
                chfs_md_id);
        chfs_print_module_status(chfs_md_id, LOGSTDOUT);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);
    des_mod_mgr = CHFS_MD_NPP_MOD_MGR(chfs_md);

    sys_log(LOGSTDOUT, "chfs_set_npp_mod_mgr: md_id %d, input src_mod_mgr %lx\n", chfs_md_id, src_mod_mgr);
    mod_mgr_print(LOGSTDOUT, src_mod_mgr);

    /*figure out mod_nodes with tcid belong to set of chfsnp_tcid_vec and chfsnp_tcid_vec*/
    mod_mgr_limited_clone(chfs_md_id, src_mod_mgr, des_mod_mgr);

    sys_log(LOGSTDOUT, "====================================chfs_set_npp_mod_mgr: des_mod_mgr %lx beg====================================\n", des_mod_mgr);
    mod_mgr_print(LOGSTDOUT, des_mod_mgr);
    sys_log(LOGSTDOUT, "====================================chfs_set_npp_mod_mgr: des_mod_mgr %lx end====================================\n", des_mod_mgr);

    return (0);
}

UINT32 chfs_set_dn_mod_mgr(const UINT32 chfs_md_id, const MOD_MGR * src_mod_mgr)
{
    CHFS_MD *chfs_md;
    MOD_MGR  *des_mod_mgr;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_set_dn_mod_mgr: chfs module #0x%lx not started.\n",
                chfs_md_id);
        chfs_print_module_status(chfs_md_id, LOGSTDOUT);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);
    des_mod_mgr = CHFS_MD_DN_MOD_MGR(chfs_md);

    sys_log(LOGSTDOUT, "chfs_set_dn_mod_mgr: md_id %d, input src_mod_mgr %lx\n", chfs_md_id, src_mod_mgr);
    mod_mgr_print(LOGSTDOUT, src_mod_mgr);

    /*figure out mod_nodes with tcid belong to set of chfsnp_tcid_vec and chfsnp_tcid_vec*/
    mod_mgr_limited_clone(chfs_md_id, src_mod_mgr, des_mod_mgr);

    sys_log(LOGSTDOUT, "====================================chfs_set_dn_mod_mgr: des_mod_mgr %lx beg====================================\n", des_mod_mgr);
    mod_mgr_print(LOGSTDOUT, des_mod_mgr);
    sys_log(LOGSTDOUT, "====================================chfs_set_dn_mod_mgr: des_mod_mgr %lx end====================================\n", des_mod_mgr);

    return (0);
}

/**
*
* get mod mgr of CHFS module
*
**/
MOD_MGR * chfs_get_npp_mod_mgr(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        return (MOD_MGR *)0;
    }

    chfs_md = CHFS_MD_GET(chfs_md_id);
    return CHFS_MD_NPP_MOD_MGR(chfs_md);
}

MOD_MGR * chfs_get_dn_mod_mgr(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        return (MOD_MGR *)0;
    }

    chfs_md = CHFS_MD_GET(chfs_md_id);
    return CHFS_MD_DN_MOD_MGR(chfs_md);
}

CHFSNP_FNODE *chfs_fnode_new(const UINT32 chfs_md_id)
{
    return chfsnp_fnode_new();
}

EC_BOOL chfs_fnode_init(const UINT32 chfs_md_id, CHFSNP_FNODE *chfsnp_fnode)
{
    return chfsnp_fnode_init(chfsnp_fnode);
}

EC_BOOL chfs_fnode_clean(const UINT32 chfs_md_id, CHFSNP_FNODE *chfsnp_fnode)
{
    return chfsnp_fnode_clean(chfsnp_fnode);
}

EC_BOOL chfs_fnode_free(const UINT32 chfs_md_id, CHFSNP_FNODE *chfsnp_fnode)
{
    return chfsnp_fnode_free(chfsnp_fnode);
}

/**
*
*  get name node pool of the module
*
**/
CHFSNP_MGR *chfs_get_npp(const UINT32 chfs_md_id)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_get_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);
    return CHFS_MD_NPP(chfs_md);
}

/**
*
*  get data node of the module
*
**/
CRFSDN *chfs_get_dn(const UINT32 chfs_md_id)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_get_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);
    return CHFS_MD_DN(chfs_md);
}

/**
*
*  open name node pool
*
**/
EC_BOOL chfs_open_npp(const UINT32 chfs_md_id, const CSTRING *chfsnp_db_root_dir)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_open_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR != CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_open_npp: npp was open\n");
        return (EC_FALSE);
    }

    CHFS_MD_NPP(chfs_md) = chfsnp_mgr_open(chfsnp_db_root_dir);
    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_open_npp: open npp from root dir %s failed\n", (char *)cstring_get_str(chfsnp_db_root_dir));
        return (EC_FALSE);
    }
    return (EC_TRUE);
}

/**
*
*  close name node pool
*
**/
EC_BOOL chfs_close_npp(const UINT32 chfs_md_id)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_close_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_close_npp: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_close(CHFS_MD_NPP(chfs_md));
    CHFS_MD_NPP(chfs_md) = NULL_PTR;
    return (EC_TRUE);
}

/**
*
*  check this CHFS is name node pool or not
*
*
**/
EC_BOOL chfs_is_npp(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_is_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  check this CHFS is data node or not
*
*
**/
EC_BOOL chfs_is_dn(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_is_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  check this CHFS is data node and namenode or not
*
*
**/
EC_BOOL chfs_is_npp_and_dn(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_is_npp_and_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md) || NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

static EC_BOOL __chfs_check_is_uint8_t(const UINT32 num)
{
    if(0 == (num >> 8))
    {
        return (EC_TRUE);
    }
    return (EC_FALSE);
}

static EC_BOOL __chfs_check_is_uint16_t(const UINT32 num)
{
    if(0 == (num >> 16))
    {
        return (EC_TRUE);
    }
    return (EC_FALSE);
}

static EC_BOOL __chfs_check_is_uint32_t(const UINT32 num)
{
#if (32 == WORDSIZE)
    return (EC_TRUE);
#endif /*(32 == WORDSIZE)*/

#if (64 == WORDSIZE)
    if(32 == WORDSIZE || 0 == (num >> 32))
    {
        return (EC_TRUE);
    }
    return (EC_FALSE);
#endif /*(64 == WORDSIZE)*/
}
 
/**
*
*  create name node pool
*
**/
EC_BOOL chfs_create_npp(const UINT32 chfs_md_id, 
                             const UINT32 chfsnp_model, 
                             const UINT32 chfsnp_max_num, 
                             const UINT32 chfsnp_1st_chash_algo_id, 
                             const UINT32 chfsnp_2nd_chash_algo_id, 
                             const UINT32 chfsnp_bucket_max_num, 
                             const CSTRING *chfsnp_db_root_dir)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_create_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR != CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: npp already exist\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == __chfs_check_is_uint8_t(chfsnp_model))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: chfsnp_model %u is invalid\n", chfsnp_model);
        return (EC_FALSE);
    }

    if(EC_FALSE == __chfs_check_is_uint32_t(chfsnp_max_num))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: chfsnp_disk_max_num %u is invalid\n", chfsnp_max_num);
        return (EC_FALSE);
    }   

    if(EC_FALSE == __chfs_check_is_uint8_t(chfsnp_1st_chash_algo_id))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: chfsnp_1st_chash_algo_id %u is invalid\n", chfsnp_1st_chash_algo_id);
        return (EC_FALSE);
    }    

    if(EC_FALSE == __chfs_check_is_uint8_t(chfsnp_2nd_chash_algo_id))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: chfsnp_2nd_chash_algo_id %u is invalid\n", chfsnp_2nd_chash_algo_id);
        return (EC_FALSE);
    }   

    if(EC_FALSE == __chfs_check_is_uint32_t(chfsnp_bucket_max_num))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: chfsnp_bucket_max_num %u is invalid\n", chfsnp_bucket_max_num);
        return (EC_FALSE);
    }     

    CHFS_MD_NPP(chfs_md) = chfsnp_mgr_create((uint8_t ) chfsnp_model, 
                                             (uint32_t) chfsnp_max_num, 
                                             (uint8_t ) chfsnp_1st_chash_algo_id, 
                                             (uint8_t ) chfsnp_2nd_chash_algo_id, 
                                             (uint32_t) chfsnp_bucket_max_num,
                                             chfsnp_db_root_dir);
    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_npp: create npp failed\n");
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

EC_BOOL chfs_add_npp(const UINT32 chfs_md_id, const UINT32 chfsnpp_tcid, const UINT32 chfsnpp_rank)
{
    CHFS_MD   *chfs_md;

    TASK_BRD *task_brd;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_add_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    task_brd = task_brd_default_get();
#if 1
    if(EC_FALSE == task_brd_check_tcid_connected(task_brd, chfsnpp_tcid))
    {
        sys_log(LOGSTDOUT, "warn:chfs_add_npp: chfsnpp_tcid %s not connected\n", c_word_to_ipv4(chfsnpp_tcid));
        return (EC_FALSE);
    }
#endif
    mod_mgr_incl(chfsnpp_tcid, CMPI_ANY_COMM, chfsnpp_rank, 0, CHFS_MD_NPP_MOD_MGR(chfs_md));
    cload_mgr_set_que(TASK_BRD_CLOAD_MGR(task_brd), chfsnpp_tcid, chfsnpp_rank, 0);

    return (EC_TRUE);
}

EC_BOOL chfs_add_dn(const UINT32 chfs_md_id, const UINT32 chfsdn_tcid, const UINT32 chfsdn_rank)
{
    CHFS_MD   *chfs_md;

    TASK_BRD *task_brd;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_add_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    task_brd = task_brd_default_get();
#if 1
    if(EC_FALSE == task_brd_check_tcid_connected(task_brd, chfsdn_tcid))
    {
        sys_log(LOGSTDOUT, "warn:chfs_add_dn: chfsdn_tcid %s not connected\n", c_word_to_ipv4(chfsdn_tcid));
        return (EC_FALSE);
    }
#endif
    mod_mgr_incl(chfsdn_tcid, CMPI_ANY_COMM, chfsdn_rank, (UINT32)0, CHFS_MD_DN_MOD_MGR(chfs_md));
    cload_mgr_set_que(TASK_BRD_CLOAD_MGR(task_brd), chfsdn_tcid, chfsdn_rank, 0);

    return (EC_TRUE);
}


/**
*
*  check existing of a file
*
**/
EC_BOOL chfs_find_file(const UINT32 chfs_md_id, const CSTRING *file_path)
{
    CHFS_MD   *chfs_md;
    EC_BOOL    ret;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_find_file: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_find_file: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0001);
    ret = chfsnp_mgr_find(CHFS_MD_NPP(chfs_md), file_path);
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0002);
    return (ret);
}

/**
*
*  check existing of a file or a dir
*
**/
EC_BOOL chfs_find(const UINT32 chfs_md_id, const CSTRING *path)
{
    CHFS_MD   *chfs_md;
    EC_BOOL    ret;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_find: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_find: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0003);
    ret = chfsnp_mgr_find(CHFS_MD_NPP(chfs_md), path);
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0004);

    return (ret);
}

/**
*
*  check existing of a file or a dir
*
**/
EC_BOOL chfs_exists(const UINT32 chfs_md_id, const CSTRING *path)
{    
#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_exists: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    return chfs_find(chfs_md_id, path);
}

/**
*
*  check existing of a file
*
**/
EC_BOOL chfs_is_file(const UINT32 chfs_md_id, const CSTRING *file_path)
{
#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_is_file: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    return chfs_find_file(chfs_md_id, file_path);;
}

/**
*
*  set/bind home dir of a name node
*
**/
EC_BOOL chfs_set(const UINT32 chfs_md_id, const CSTRING *home_dir, const UINT32 chfsnp_id)
{
    CHFS_MD      *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_set: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(EC_FALSE == __chfs_check_is_uint32_t(chfsnp_id))
    {
        sys_log(LOGSTDOUT, "error:chfs_set: chfsnp_id %u is invalid\n", chfsnp_id);
        return (EC_FALSE);
    } 

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_set: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_wrlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0005);
    if(EC_FALSE == chfsnp_mgr_bind(CHFS_MD_NPP(chfs_md), home_dir, chfsnp_id))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0006);
        sys_log(LOGSTDOUT, "error:chfs_set: bind home '%s' to np %u failed\n", (char *)cstring_get_str(home_dir), chfsnp_id);
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0007);
    return (EC_TRUE);
}

/**
*
*  write a file
*
**/
EC_BOOL chfs_write(const UINT32 chfs_md_id, const CSTRING *file_path, const CBYTES *cbytes)
{
    CHFSNP_FNODE  chfsnp_fnode;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_write: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfsnp_fnode_init(&chfsnp_fnode);

    if(EC_FALSE == chfs_write_dn(chfs_md_id, cbytes, &chfsnp_fnode))
    {
        sys_log(LOGSTDOUT, "error:chfs_write: write to dn failed\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == chfs_write_npp(chfs_md_id, file_path, &chfsnp_fnode))
    {
        sys_log(LOGSTDOUT, "error:chfs_write: write file %s to npp failed\n", (char *)cstring_get_str(file_path));
        chfs_delete_dn(chfs_md_id, &chfsnp_fnode);
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  read a file
*
**/
EC_BOOL chfs_read(const UINT32 chfs_md_id, const CSTRING *file_path, CBYTES *cbytes)
{
    CHFSNP_FNODE  chfsnp_fnode;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_read: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfsnp_fnode_init(&chfsnp_fnode);
    
    if(EC_FALSE == chfs_read_npp(chfs_md_id, file_path, &chfsnp_fnode))
    {
        sys_log(LOGSTDOUT, "error:chfs_read: read file %s from npp failed\n", (char *)cstring_get_str(file_path));
        return (EC_FALSE);
    }

    if(EC_FALSE == chfs_read_dn(chfs_md_id, &chfsnp_fnode, cbytes))
    {
        sys_log(LOGSTDOUT, "error:chfs_read: read file %s from dn failed where fnode is\n", (char *)cstring_get_str(file_path));
        chfsnp_fnode_print(LOGSTDOUT, &chfsnp_fnode);
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  create data node
*
**/
EC_BOOL chfs_create_dn(const UINT32 chfs_md_id, const CSTRING *root_dir, const UINT32 max_gb_num_of_disk_space)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_create_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);
    if(NULL_PTR != CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_dn: dn already exist\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == __chfs_check_is_uint16_t(max_gb_num_of_disk_space))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_dn: max_gb_num_of_disk_space %u is invalid\n", max_gb_num_of_disk_space);
        return (EC_FALSE);
    }

    CHFS_MD_DN(chfs_md) = crfsdn_create((char *)cstring_get_str(root_dir), max_gb_num_of_disk_space);
    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_create_dn: create dn failed\n");
        return (EC_FALSE);
    }

    return (EC_TRUE);
}

/**
*
*  open data node
*
**/
EC_BOOL chfs_open_dn(const UINT32 chfs_md_id, const CSTRING *root_dir)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_open_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/
    sys_log(LOGSTDOUT, "[DEBUG] chfs_open_dn: try to open dn %s  ...\n", (char *)cstring_get_str(root_dir));

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR != CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_open_dn: dn was open\n");
        return (EC_FALSE);
    }

    CHFS_MD_DN(chfs_md) = crfsdn_open((char *)cstring_get_str(root_dir));
    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_open_dn: open dn with root dir %s failed\n", (char *)cstring_get_str(root_dir));
        return (EC_FALSE);
    }
    sys_log(LOGSTDOUT, "[DEBUG] chfs_open_dn: open dn %s\n", (char *)cstring_get_str(root_dir));
    return (EC_TRUE);
}

/**
*
*  close data node
*
**/
EC_BOOL chfs_close_dn(const UINT32 chfs_md_id)
{
    CHFS_MD   *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_close_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_close_dn: no dn was open\n");
        return (EC_FALSE);
    }

    crfsdn_close(CHFS_MD_DN(chfs_md));
    CHFS_MD_DN(chfs_md) = NULL_PTR;
    sys_log(LOGSTDOUT, "[DEBUG] chfs_close_dn: dn was closed\n");

    return (EC_TRUE);
}

/**
*
*  write data node
*
**/
EC_BOOL chfs_write_dn(const UINT32 chfs_md_id, const CBYTES *cbytes, CHFSNP_FNODE *chfsnp_fnode)
{
    CHFS_MD      *chfs_md;
    CHFSNP_INODE *chfsnp_inode;

    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;    

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_write_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(CPGB_CACHE_MAX_BYTE_SIZE <= CBYTES_LEN(cbytes))
    {
        sys_log(LOGSTDOUT, "error:chfs_write_dn: buff len (or file size) %u overflow\n", CBYTES_LEN(cbytes));
        return (EC_FALSE);
    }

    crfsdn_wrlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0008);
    if(EC_FALSE == crfsdn_write_p(CHFS_MD_DN(chfs_md), cbytes_len(cbytes), cbytes_buf(cbytes), &disk_no, &block_no, &page_no))
    {
        crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0009);
        
        sys_log(LOGSTDOUT, "error:chfs_write_dn: write %u bytes to dn failed\n", CBYTES_LEN(cbytes));
        return (EC_FALSE);
    }
    crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0010);

    chfsnp_fnode_init(chfsnp_fnode);
    chfsnp_inode = CHFSNP_FNODE_INODE(chfsnp_fnode, 0);
    CHFSNP_INODE_DISK_NO(chfsnp_inode)  = disk_no;
    CHFSNP_INODE_BLOCK_NO(chfsnp_inode) = block_no;
    CHFSNP_INODE_PAGE_NO(chfsnp_inode)  = page_no;

    CHFSNP_FNODE_FILESZ(chfsnp_fnode) = CBYTES_LEN(cbytes);
    CHFSNP_FNODE_REPNUM(chfsnp_fnode) = 1;

    return (EC_TRUE);
}

/**
*
*  read data node
*
**/
EC_BOOL chfs_read_dn(const UINT32 chfs_md_id, const CHFSNP_FNODE *chfsnp_fnode, CBYTES *cbytes)
{
    CHFS_MD *chfs_md;
    const CHFSNP_INODE *chfsnp_inode;

    uint32_t file_size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_read_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_read_dn: dn is null\n");
        return (EC_FALSE);
    }

    if(0 == CHFSNP_FNODE_REPNUM(chfsnp_fnode))
    {
        sys_log(LOGSTDOUT, "error:chfs_read_dn: no replica\n");
        return (EC_FALSE);
    }

    file_size    = CHFSNP_FNODE_FILESZ(chfsnp_fnode);
    chfsnp_inode = CHFSNP_FNODE_INODE(chfsnp_fnode, 0);
    disk_no  = CHFSNP_INODE_DISK_NO(chfsnp_inode) ;
    block_no = CHFSNP_INODE_BLOCK_NO(chfsnp_inode);
    page_no  = CHFSNP_INODE_PAGE_NO(chfsnp_inode) ;

    sys_log(LOGSTDOUT, "[DEBUG] chfs_read_dn: file file %u, disk %u, block %u, page %u\n", file_size, disk_no, block_no, page_no);

    if(CBYTES_LEN(cbytes) < file_size)
    {
        if(NULL_PTR != CBYTES_BUF(cbytes))
        {
            SAFE_FREE(CBYTES_BUF(cbytes), LOC_CHFS_0011);
        }
        CBYTES_BUF(cbytes) = (UINT8 *)SAFE_MALLOC(file_size, LOC_CHFS_0012);
        CBYTES_LEN(cbytes) = 0;
    }

    crfsdn_rdlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0013);
    if(EC_FALSE == crfsdn_read_p(CHFS_MD_DN(chfs_md), disk_no, block_no, page_no, file_size, CBYTES_BUF(cbytes), &(CBYTES_LEN(cbytes))))
    {
        crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0014);
        
        sys_log(LOGSTDOUT, "error:chfs_read_dn: read %u bytes from disk %u, block %u, page %u failed\n", 
                           file_size, disk_no, block_no, page_no);
        return (EC_FALSE);
    }
    crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0015);
    return (EC_TRUE);
}

/**
*
*  write a fnode to name node
*
**/
EC_BOOL chfs_write_npp(const UINT32 chfs_md_id, const CSTRING *file_path, const CHFSNP_FNODE *chfsnp_fnode)
{
    CHFS_MD      *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_write_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_write_npp: npp was not open\n");
        return (EC_FALSE);
    }

    if(0 == CHFSNP_FNODE_REPNUM(chfsnp_fnode))
    {
        sys_log(LOGSTDOUT, "error:chfs_write_npp: no valid replica in fnode\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_wrlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0016);
    if(EC_FALSE == chfsnp_mgr_write(CHFS_MD_NPP(chfs_md), file_path, chfsnp_fnode))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0017);
        
        sys_log(LOGSTDOUT, "error:chfs_write_npp: no name node accept file %s with %u replicas\n",
                            (char *)cstring_get_str(file_path), CHFSNP_FNODE_REPNUM(chfsnp_fnode));
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0018);
    return (EC_TRUE);
}

/**
*
*  read a fnode from name node
*
**/
EC_BOOL chfs_read_npp(const UINT32 chfs_md_id, const CSTRING *file_path, CHFSNP_FNODE *chfsnp_fnode)
{
    CHFS_MD      *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_read_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_read_npp: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0019);
    if(EC_FALSE == chfsnp_mgr_read(CHFS_MD_NPP(chfs_md), file_path, chfsnp_fnode))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0020);
        
        sys_log(LOGSTDOUT, "error:chfs_read_npp: chfsnp mgr read %s failed\n", (char *)cstring_get_str(file_path));
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0021);

    return (EC_TRUE);
}

/**
*
*  delete a file from current npp
*
**/
EC_BOOL chfs_delete_npp(const UINT32 chfs_md_id, const CSTRING *path, CVECTOR *chfsnp_fnode_vec)
{
    CHFS_MD      *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_delete_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_delete_npp: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_wrlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0022);
    if(EC_FALSE == chfsnp_mgr_delete(CHFS_MD_NPP(chfs_md), path, chfsnp_fnode_vec))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0023);
        
        sys_log(LOGSTDOUT, "error:chfs_delete_npp: delete '%s' failed\n", (char *)cstring_get_str(path));
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0024);

    return (EC_TRUE);
}

/**
*
*  delete file data from current dn
*
**/
EC_BOOL chfs_delete_dn(const UINT32 chfs_md_id, const CHFSNP_FNODE *chfsnp_fnode)
{
    CHFS_MD *chfs_md;
    const CHFSNP_INODE *chfsnp_inode;

    uint32_t file_size;
    uint16_t disk_no;
    uint16_t block_no;
    uint16_t page_no;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_delete_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_delete_dn: no dn was open\n");
        return (EC_FALSE);
    }

    if(0 == CHFSNP_FNODE_REPNUM(chfsnp_fnode))
    {
        sys_log(LOGSTDOUT, "error:chfs_delete_dn: no replica\n");
        return (EC_FALSE);
    }    

    file_size    = CHFSNP_FNODE_FILESZ(chfsnp_fnode);
    chfsnp_inode = CHFSNP_FNODE_INODE(chfsnp_fnode, 0);
    disk_no  = CHFSNP_INODE_DISK_NO(chfsnp_inode) ;
    block_no = CHFSNP_INODE_BLOCK_NO(chfsnp_inode);
    page_no  = CHFSNP_INODE_PAGE_NO(chfsnp_inode) ;
    
    crfsdn_wrlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0025);
    if(EC_FALSE == crfsdn_remove(CHFS_MD_DN(chfs_md), disk_no, block_no, page_no, file_size))
    {
        crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0026);    
        sys_log(LOGSTDOUT, "error:chfs_delete_dn: remove file fsize %u, disk %u, block %u, page %u failed\n", file_size, disk_no, block_no, page_no);
        return (EC_FALSE);
    }    
    crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0027);

    sys_log(LOGSTDOUT, "[DEBUG] chfs_delete_dn: remove file fsize %u, disk %u, block %u, page %u done\n", file_size, disk_no, block_no, page_no);
    
    return (EC_TRUE);
}

/**
*
*  delete a file or dir from all npp and all dn
*
**/
EC_BOOL chfs_delete(const UINT32 chfs_md_id, const CSTRING *path)
{
    CVECTOR *chfsnp_fnode_vec;
    UINT32   chfsnp_fnode_num;
    UINT32   chfsnp_fnode_pos;
    EC_BOOL  ret;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_delete: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfsnp_fnode_vec = cvector_new(0, MM_CHFSNP_FNODE, LOC_CHFS_0028);
    if(NULL_PTR == chfsnp_fnode_vec)
    {
        sys_log(LOGSTDOUT, "error:chfs_delete: new vector failed\n");
        return (EC_FALSE);
    }

    /*delete inodes*/
    if(EC_FALSE == chfs_delete_npp(chfs_md_id, path, chfsnp_fnode_vec))
    {
        sys_log(LOGSTDOUT, "error:chfs_delete: delete %s from npp failed\n", (char *)cstring_get_str(path));
        return (EC_FALSE);
    }

    /*delete data*/
    ret = EC_TRUE;
    
    chfsnp_fnode_num = cvector_size(chfsnp_fnode_vec);
    for(chfsnp_fnode_pos = 0; chfsnp_fnode_pos < chfsnp_fnode_num; chfsnp_fnode_pos ++)
    {
        CHFSNP_FNODE *chfsnp_fnode;
        
        chfsnp_fnode = (CHFSNP_FNODE *)cvector_get_no_lock(chfsnp_fnode_vec, chfsnp_fnode_pos);
        if(EC_FALSE == chfs_delete_dn(chfs_md_id, chfsnp_fnode))
        {
            sys_log(LOGSTDOUT, "error:chfs_delete: delete %s from dn failed\n", (char *)cstring_get_str(path));
            ret = EC_FALSE;
        }
    }

    cvector_clean(chfsnp_fnode_vec, (CVECTOR_DATA_CLEANER)chfsnp_fnode_free, LOC_CHFS_0029);
    cvector_free(chfsnp_fnode_vec, LOC_CHFS_0030);    
    
    return (ret);
}

/**
*
*  query a file
*
**/
EC_BOOL chfs_qfile(const UINT32 chfs_md_id, const CSTRING *file_path, CHFSNP_ITEM  *chfsnp_item)
{
    CHFS_MD      *chfs_md;
    CHFSNP_ITEM  *chfsnp_item_src;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_qfile: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_qfile: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0031);
    chfsnp_item_src = chfsnp_mgr_search_item(CHFS_MD_NPP(chfs_md), 
                                             (uint32_t)cstring_get_len(file_path), 
                                             cstring_get_str(file_path));
    if(NULL_PTR == chfsnp_item_src)
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0032);
        sys_log(LOGSTDOUT, "error:chfs_qfile: query file %s from npp failed\n", (char *)cstring_get_str(file_path));
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0033);

    chfsnp_item_clone(chfsnp_item_src, chfsnp_item);

    return (EC_TRUE);
}

/**
*
*  flush name node pool
*
**/
EC_BOOL chfs_flush_npp(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_flush_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_flush_npp: npp was not open\n");
        return (EC_TRUE);
    }

    chfsnp_mgr_wrlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0034);
    if(EC_FALSE == chfsnp_mgr_flush(CHFS_MD_NPP(chfs_md)))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0035);
        
        sys_log(LOGSTDOUT, "error:chfs_flush_npp: flush failed\n");
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0036);
    return (EC_TRUE);
}

/**
*
*  flush data node
*
*
**/
EC_BOOL chfs_flush_dn(const UINT32 chfs_md_id)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_flush_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_flush_dn: dn is null\n");
        return (EC_FALSE);
    }

    crfsdn_wrlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0037);
    if(EC_FALSE == crfsdn_flush(CHFS_MD_DN(chfs_md)))
    {
        crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0038);
        sys_log(LOGSTDOUT, "error:chfs_flush_dn: flush dn failed\n");
        return (EC_FALSE);
    }
    crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0039);

    return (EC_TRUE);
}

/**
*
*  count file num under specific path
*  if path is regular file, return file_num 1
*  if path is directory, return file num under it
*
**/
EC_BOOL chfs_file_num(const UINT32 chfs_md_id, UINT32 *file_num)
{
    CHFS_MD      *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_file_num: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_file_num: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_wrlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0040);
    if(EC_FALSE == chfsnp_mgr_file_num(CHFS_MD_NPP(chfs_md), file_num))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0041);
        sys_log(LOGSTDOUT, "error:chfs_file_num: count total file num failed\n");
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0042);

    return (EC_TRUE);
}

/**
*
*  get file size of specific file given full path name
*
**/
EC_BOOL chfs_file_size(const UINT32 chfs_md_id, UINT32 *file_size)
{
    CHFS_MD      *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_file_size: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_file_size: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_wrlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0043);
    if(EC_FALSE == chfsnp_mgr_file_size(CHFS_MD_NPP(chfs_md), file_size))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0044);
        sys_log(LOGSTDOUT, "error:chfs_file_size: count total file size failed\n");
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0045);
    return (EC_TRUE);
}

/**
*
*  search in current name node pool
*
**/
EC_BOOL chfs_search(const UINT32 chfs_md_id, const CSTRING *path_cstr)
{
    CHFS_MD      *chfs_md;
    uint32_t      chfsnp_id;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_search: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(LOGSTDOUT, "warn:chfs_search: npp was not open\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0046);
    if(EC_FALSE == chfsnp_mgr_search(CHFS_MD_NPP(chfs_md), (uint32_t)cstring_get_len(path_cstr), cstring_get_str(path_cstr), &chfsnp_id))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0047);
        sys_log(LOGSTDOUT, "error:chfs_search: search '%s' failed\n", (char *)cstring_get_str(path_cstr));
        return (EC_FALSE);
    }
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0048);

    return (EC_TRUE);
}

/**
*
*  check file content on data node
*
**/
EC_BOOL chfs_check_file_content(const UINT32 chfs_md_id, const UINT32 disk_no, const UINT32 block_no, const UINT32 page_no, const UINT32 file_size, const CSTRING *file_content_cstr)
{
    CHFS_MD *chfs_md;

    CBYTES *cbytes;

    UINT8 *buff;
    UINT8 *str;

    UINT32 len;
    UINT32 pos;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_check_file_content: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_content: dn is null\n");
        return (EC_FALSE);
    }

    ASSERT(EC_TRUE == __chfs_check_is_uint16_t(disk_no));
    ASSERT(EC_TRUE == __chfs_check_is_uint16_t(block_no));
    ASSERT(EC_TRUE == __chfs_check_is_uint16_t(page_no));

    cbytes = cbytes_new(file_size);
    if(NULL_PTR == cbytes)
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_content: new chfs buff with len %u failed\n", file_size);
        return (EC_FALSE);
    }

    if(EC_FALSE == crfsdn_read_p(CHFS_MD_DN(chfs_md), (uint16_t)disk_no, (uint16_t)block_no, (uint16_t)page_no, file_size, 
                                  CBYTES_BUF(cbytes), &(CBYTES_LEN(cbytes))))
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_content: read %u bytes from disk %u, block %u, page %u failed\n", 
                            file_size, disk_no, block_no, page_no);
        cbytes_free(cbytes, LOC_CHFS_0049);
        return (EC_FALSE);
    }

    if(CBYTES_LEN(cbytes) < cstring_get_len(file_content_cstr))
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_content: read %u bytes from disk %u, block %u, page %u to buff len %u less than cstring len %u to compare\n",
                            file_size, disk_no, block_no, page_no,
                            CBYTES_LEN(cbytes), cstring_get_len(file_content_cstr));
        cbytes_free(cbytes, LOC_CHFS_0050);
        return (EC_FALSE);
    }

    len = cstring_get_len(file_content_cstr);

    buff = CBYTES_BUF(cbytes);
    str  = cstring_get_str(file_content_cstr);

    for(pos = 0; pos < len; pos ++)
    {
        if(buff[ pos ] != str[ pos ])
        {
            sys_log(LOGSTDOUT, "error:chfs_check_file_content: char at pos %u not matched\n", pos);
            sys_print(LOGSTDOUT, "read buff: %.*s\n", len, buff);
            sys_print(LOGSTDOUT, "expected : %.*s\n", len, str);

            cbytes_free(cbytes, LOC_CHFS_0051);
            return (EC_FALSE);
        }
    }

    cbytes_free(cbytes, LOC_CHFS_0052);
    return (EC_TRUE);
}

/**
*
*  check file content on data node
*
**/
EC_BOOL chfs_check_file_is(const UINT32 chfs_md_id, const CSTRING *file_path, const CBYTES *file_content)
{
    CHFS_MD *chfs_md;

    CBYTES *cbytes;

    UINT8 *buff;
    UINT8 *str;

    UINT32 len;
    UINT32 pos;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_check_file_is: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_is: dn is null\n");
        return (EC_FALSE);
    }
    
    cbytes = cbytes_new(0);
    if(NULL_PTR == cbytes)
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_is: new cbytes failed\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == chfs_read(chfs_md_id, file_path, cbytes))
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_is: read file %s failed\n", (char *)cstring_get_str(file_path));
        cbytes_free(cbytes, LOC_CHFS_0053);
        return (EC_FALSE);
    }

    if(CBYTES_LEN(cbytes) != CBYTES_LEN(file_content))
    {
        sys_log(LOGSTDOUT, "error:chfs_check_file_is: mismatched len: file %s read len %u which should be %u\n",
                            (char *)cstring_get_str(file_path),                            
                            CBYTES_LEN(cbytes), CBYTES_LEN(file_content));
        cbytes_free(cbytes, LOC_CHFS_0054);
        return (EC_FALSE);
    }

    len  = CBYTES_LEN(file_content);

    buff = CBYTES_BUF(cbytes);
    str  = CBYTES_BUF(file_content);

    for(pos = 0; pos < len; pos ++)
    {
        if(buff[ pos ] != str[ pos ])
        {
            sys_log(LOGSTDOUT, "error:chfs_check_file_is: char at pos %u not matched\n", pos);
            sys_print(LOGSTDOUT, "read buff: %.*s\n", len, buff);
            sys_print(LOGSTDOUT, "expected : %.*s\n", len, str);

            cbytes_free(cbytes, LOC_CHFS_0055);
            return (EC_FALSE);
        }
    }

    cbytes_free(cbytes, LOC_CHFS_0056);
    return (EC_TRUE);
}

/**
*
*  show name node pool info if it is npp
*
*
**/
EC_BOOL chfs_show_npp(const UINT32 chfs_md_id, LOG *log)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_show_npp: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0057);
    
    chfsnp_mgr_print(log, CHFS_MD_NPP(chfs_md));
    
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0058);
    
    return (EC_TRUE);
}

/**
*
*  show crfsdn info if it is dn
*
*
**/
EC_BOOL chfs_show_dn(const UINT32 chfs_md_id, LOG *log)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_show_dn: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_DN(chfs_md))
    {
        sys_log(log, "(null)\n");
        return (EC_TRUE);
    }

    crfsdn_rdlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0059);
    crfsdn_print(log, CHFS_MD_DN(chfs_md));
    crfsdn_unlock(CHFS_MD_DN(chfs_md), LOC_CHFS_0060);

    return (EC_TRUE);
}

/*debug*/
EC_BOOL chfs_show_cached_np(const UINT32 chfs_md_id, LOG *log)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_show_cached_np: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(log, "(null)\n");
        return (EC_FALSE);
    }

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0061);    
    if(EC_FALSE == chfsnp_mgr_show_cached_np(log, CHFS_MD_NPP(chfs_md)))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0062);
        sys_log(LOGSTDOUT, "error:chfs_show_cached_np: show cached np but failed\n");
        return (EC_FALSE);
    }    
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0063);

    return (EC_TRUE);
}

EC_BOOL chfs_show_specific_np(const UINT32 chfs_md_id, const UINT32 chfsnp_id, LOG *log)
{
    CHFS_MD *chfs_md;

#if ( SWITCH_ON == CHFS_DEBUG_SWITCH )
    if ( CHFS_MD_ID_CHECK_INVALID(chfs_md_id) )
    {
        sys_log(LOGSTDOUT,
                "error:chfs_show_specific_np: chfs module #0x%lx not started.\n",
                chfs_md_id);
        dbg_exit(MD_CHFS, chfs_md_id);
    }
#endif/*CHFS_DEBUG_SWITCH*/

    chfs_md = CHFS_MD_GET(chfs_md_id);

    if(NULL_PTR == CHFS_MD_NPP(chfs_md))
    {
        sys_log(log, "(null)\n");
        return (EC_FALSE);
    }

    if(EC_FALSE == __chfs_check_is_uint32_t(chfsnp_id))
    {
        sys_log(LOGSTDOUT, "error:chfs_show_specific_np: chfsnp_id %u is invalid\n", chfsnp_id);
        return (EC_FALSE);
    }    

    chfsnp_mgr_rdlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0064);    
    if(EC_FALSE == chfsnp_mgr_show_np(log, CHFS_MD_NPP(chfs_md), (uint32_t)chfsnp_id))
    {
        chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0065);
        sys_log(LOGSTDOUT, "error:chfs_show_cached_np: show np %u but failed\n", chfsnp_id);
        return (EC_FALSE);
    }    
    chfsnp_mgr_unlock(CHFS_MD_NPP(chfs_md), LOC_CHFS_0066);    

    return (EC_TRUE);
}

#ifdef __cplusplus
}
#endif/*__cplusplus*/
