/******************************************************************************
*
* Copyright (C) Chaoyong Zhou
* Email: bgnvendor@gmail.com 
* QQ: 312230917
*
*******************************************************************************/
#ifndef _POLYZN_INC
#define _POLYZN_INC

#include "findex.inc"

FUNC_ADDR_NODE g_polyzn_func_addr_list[ ] = {
{
/*func priority   */    PRIO_BEG,
/*func logic addr */    (UINT32)poly_zn_print_module_status,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_print_module_status",
/*func index      */    FI_poly_zn_print_module_status,
/*func ret type   */    e_dbg_void,
/*func para num   */    2,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IO,},
/*func para type  */    {e_dbg_UINT32,e_dbg_LOG_ptr,},
/*func para val   */    0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_free_module_static_mem,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_free_module_static_mem",
/*func index      */    FI_poly_zn_free_module_static_mem,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    1,
/*func para direct*/    {E_DIRECT_IN,},
/*func para type  */    {e_dbg_UINT32,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_start,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_start",
/*func index      */    FI_poly_zn_start,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    1,
/*func para direct*/    {E_DIRECT_IN,},
/*func para type  */    {e_dbg_BIGINT_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_end,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_end",
/*func index      */    FI_poly_zn_end,
/*func ret type   */    e_dbg_void,
/*func para num   */    1,
/*func para direct*/    {E_DIRECT_IN,},
/*func para type  */    {e_dbg_UINT32,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_item_destory,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_item_destory",
/*func index      */    FI_poly_zn_item_destory,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    2,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ITEM_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_item_clone,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_item_clone",
/*func index      */    FI_poly_zn_item_clone,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ITEM_ptr,e_dbg_POLY_ITEM_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_item_cmp,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_item_cmp",
/*func index      */    FI_poly_zn_item_cmp,
/*func ret type   */    e_dbg_EC_BOOL,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ITEM_ptr,e_dbg_POLY_ITEM_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_item_neg,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_item_neg",
/*func index      */    FI_poly_zn_item_neg,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ITEM_ptr,e_dbg_POLY_ITEM_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_item_move,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_item_move",
/*func index      */    FI_poly_zn_item_move,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ITEM_ptr,e_dbg_POLY_ITEM_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_poly_clean,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_poly_clean",
/*func index      */    FI_poly_zn_poly_clean,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    2,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_poly_destory,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_poly_destory",
/*func index      */    FI_poly_zn_poly_destory,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    2,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_poly_clone,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_poly_clone",
/*func index      */    FI_poly_zn_poly_clone,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_set_zero,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_set_zero",
/*func index      */    FI_poly_zn_set_zero,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    2,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_set_one,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_set_one",
/*func index      */    FI_poly_zn_set_one,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    2,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_set_n,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_set_n",
/*func index      */    FI_poly_zn_set_n,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_BIGINT_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_poly_move,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_poly_move",
/*func index      */    FI_poly_zn_poly_move,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_OUT,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_poly_cmp,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_poly_cmp",
/*func index      */    FI_poly_zn_poly_cmp,
/*func ret type   */    e_dbg_EC_BOOL,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_add_n,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_add_n",
/*func index      */    FI_poly_zn_add_n,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_BIGINT_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_adc_poly,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_adc_poly",
/*func index      */    FI_poly_zn_adc_poly,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_add_poly,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_add_poly",
/*func index      */    FI_poly_zn_add_poly,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_sub_poly_bgn,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_sub_poly_bgn",
/*func index      */    FI_poly_zn_sub_poly_bgn,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_BIGINT_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_sub_bgn_poly,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_sub_bgn_poly",
/*func index      */    FI_poly_zn_sub_bgn_poly,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_BIGINT_ptr,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_poly_neg,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_poly_neg",
/*func index      */    FI_poly_zn_poly_neg,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_sub_poly,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_sub_poly",
/*func index      */    FI_poly_zn_sub_poly,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_mul_poly,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_mul_poly",
/*func index      */    FI_poly_zn_mul_poly,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_squ_poly,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_squ_poly",
/*func index      */    FI_poly_zn_squ_poly,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_sexp,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_sexp",
/*func index      */    FI_poly_zn_sexp,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_UINT32,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_exp,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_exp",
/*func index      */    FI_poly_zn_exp,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_BIGINT_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_dx,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_dx",
/*func index      */    FI_poly_zn_dx,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    3,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
{
/*func priority   */    PRIO_MD_POLYZN,
/*func logic addr */    (UINT32)poly_zn_Dx,
/*func beg addr   */    0,
/*func end addr   */    0,
/*func addr offset*/    0,
/*func name       */    "poly_zn_Dx",
/*func index      */    FI_poly_zn_Dx,
/*func ret type   */    e_dbg_UINT32,
/*func para num   */    4,
/*func para direct*/    {E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_IN,E_DIRECT_OUT,},
/*func para type  */    {e_dbg_UINT32,e_dbg_POLY_ptr,e_dbg_UINT32,e_dbg_POLY_ptr,},
/*func para val   */     0, 0, {0},
},
};

UINT32 g_polyzn_func_addr_list_len = sizeof(g_polyzn_func_addr_list)/sizeof(g_polyzn_func_addr_list[0]);

#endif/*_POLYZN_INC*/
