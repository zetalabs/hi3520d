/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/ezcfg_local.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-09   0.1       Move some local definitions here.
 * ============================================================================
 */

#ifndef _EZCFG_LOCAL_COMMON_EZCFG_LOCAL_H_
#define _EZCFG_LOCAL_COMMON_EZCFG_LOCAL_H_

#include "ezcfg-types.h"

/* commom/nv_pair.c */
struct nv_pair {
  char *name;
  char *value;
};
int local_nv_pair_del_handler(void *data);
int local_nv_pair_cmp_handler(const void *d1, const void *d2);


/* commom/linked_list.c */
struct linked_list;
struct linked_list *local_linked_list_new(
  int (*del_hdl)(void *),
  int (*cmp_hdl)(const void *, const void *));
void local_linked_list_del(struct linked_list *list);
bool local_linked_list_insert(struct linked_list *list, void *data);
bool local_linked_list_append(struct linked_list *list, void *data);
int local_linked_list_get_length(struct linked_list *list);
void *local_linked_list_get_node_data_by_index(struct linked_list *list, int i);


/* common/stack_list.c */
struct stack_list;
struct stack_list *local_stack_list_new(void);
void local_stack_list_del(struct stack_list *list);
void local_stack_list_set_data_delete_handler(struct stack_list *list, int (*handler)(void *));
bool local_stack_list_push(struct stack_list *list, void *data);
void *local_stack_list_pop(struct stack_list *list);
int local_stack_list_get_length(struct stack_list *list);
bool local_stack_list_is_empty(struct stack_list *list);


/* common/binary_tree.c */
struct binary_tree_node;
struct binary_tree;
struct binary_tree_node *local_binary_tree_node_new(void *data);
int local_binary_tree_node_del(struct binary_tree *tree, struct binary_tree_node *np);
void *local_binary_tree_node_get_data(struct binary_tree_node *np);
struct binary_tree_node *local_binary_tree_node_get_left(struct binary_tree_node *np);
struct binary_tree_node *local_binary_tree_node_get_right(struct binary_tree_node *np);
int local_binary_tree_node_append_left(struct binary_tree_node *np, struct binary_tree_node *np_left);
int local_binary_tree_node_append_right(struct binary_tree_node *np, struct binary_tree_node *np_right);

struct binary_tree *local_binary_tree_new(void);
int local_binary_tree_del(struct binary_tree *tree);
int local_binary_tree_reset(struct binary_tree *tree);
int local_binary_tree_set_data_delete_handler(struct binary_tree *tree, int (*handler)(void *));
struct binary_tree_node *local_binary_tree_get_root(struct binary_tree *tree);
int local_binary_tree_set_root(struct binary_tree *tree, struct binary_tree_node *np);


/* commom/json.c */
struct json_value;
struct json_value *local_json_value_create(char *buf);
int local_json_value_delete(struct json_value *vp);
int local_json_value_delete_wrap(void *data);

struct json;
void local_json_del(struct json *json);
struct json *local_json_new(void);
void local_json_reset(struct json *json);
int local_json_get_text_type(struct json *json);
int local_json_parse_text(struct json *json, char *text, int len);
int local_json_get_msg_len(struct json *json);
int local_json_write_message(struct json *json, char *buf, int len);
struct linked_list *local_json_build_nvram_node_list(struct json *json);
bool local_json_is_nvram_representation(struct json *json);


/* commom/meta_nvram.c */
struct nvram_header {
  char magic[4]; /* must be 'N','V','R','M' */
  char version[4];
  char coding[4];
  char backend[4];
  int32_t data_size;
  int32_t data_used;
  uint32_t data_crc;
  uint32_t header_crc;
};

#define NVRAM_VERSOIN_MAJOR 0x00 /* version[0] */
#define NVRAM_VERSOIN_MINOR 0x02 /* version[1] */
#define NVRAM_VERSOIN_MICRO 0x00 /* version[2] */
#define NVRAM_VERSOIN_REV   0x00 /* version[3] */ 

int local_meta_nvram_del(char *buffer);
char *local_meta_nvram_new(int size);
int local_meta_nvram_set_version(char *buffer, char version[4]);
int local_meta_nvram_set_coding_type(char *buffer, char coding[4]);
int local_meta_nvram_set_backend_type(char *buffer, char backend[4]);
int local_meta_nvram_update_data_crc(char *buffer);
int local_meta_nvram_update_header_crc(char *buffer);
int local_meta_nvram_set_entry(char *buffer, const char *name, const char *value);
int local_meta_nvram_unset_entry(char *buffer, const char *name);
int local_meta_nvram_get_entry_value(char *buffer, const char *name, char **value);
int local_meta_nvram_match_entry(char *buffer, const char *name1, const char *name2);
int local_meta_nvram_match_entry_value(char *buffer, const char *name, char *value);


/* common/nvram.c */
struct nvram;
int local_nvram_del(struct nvram *nvram);
struct nvram *local_nvram_new(char *buffer);
int local_nvram_set_backend_type(struct nvram *nvram, const int index, const int type);
int local_nvram_set_coding_type(struct nvram *nvram, const int index, const int type);
int local_nvram_set_total_space(struct nvram *nvram, const int total_space);
int local_nvram_get_total_space(struct nvram *nvram);
int local_nvram_get_free_space(struct nvram *nvram);
int local_nvram_get_used_space(struct nvram *nvram);
int local_nvram_get_version_string(struct nvram *nvram, char *buf, size_t len);
int local_nvram_set_entry(struct nvram *nvram, const char *name, const char *value);
int local_nvram_get_entry_value(struct nvram *nvram, const char *name, char **value);
int local_nvram_unset_entry(struct nvram *nvram, const char *name);
int local_nvram_commit(struct nvram *nvram);
int local_nvram_initialize(struct nvram *nvram);
int local_nvram_match_entry(struct nvram *nvram, char *name1, char *name2);
int local_nvram_match_entry_value(struct nvram *nvram, char *name, char *value);
int local_nvram_is_valid_entry_value(struct nvram *nvram, char *name, char *value);

#endif /* _EZCFG_LOCAL_COMMON_H_ */
