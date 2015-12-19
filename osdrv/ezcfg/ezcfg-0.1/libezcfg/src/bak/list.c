/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/list.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/**
 * SECTION:libezcfg-list
 * @short_description: list operation
 *
 * Libezcfg list operations.
 */

/**
 * ezcfg_list_entry:
 *
 * Opaque object representing one entry in a list. An entry
 * contains a name, and optionally a value.
 */
struct ezcfg_list_entry {
	struct ezcfg *ezcfg;
	struct ezcfg_list_node node;
	struct ezcfg_list_node *list;
	char *name;
	char *value;
	unsigned int flags;
};

/* list head point to itself if empty */
void ezcfg_list_init(struct ezcfg_list_node *list)
{
	list->next = list;
	list->prev = list;
}

bool ezcfg_list_is_empty(struct ezcfg_list_node *list)
{
	return (list->next == list);
}

int ezcfg_list_get_num_nodes(struct ezcfg_list_node *list)
{
	struct ezcfg_list_node *next;
	int count;

	next = list->next;
	count = 0;

	while(next != list) {
		count++;
		next = next->next;
	}
	return count;
}

static void ezcfg_list_node_insert_between(
	struct ezcfg_list_node *new,
	struct ezcfg_list_node *prev,
	struct ezcfg_list_node *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void ezcfg_list_node_append(
	struct ezcfg_list_node *new,
	struct ezcfg_list_node *list)
{
	ezcfg_list_node_insert_between(new, list->prev, list);
}

void ezcfg_list_node_remove(struct ezcfg_list_node *entry)
{
	struct ezcfg_list_node *prev = entry->prev;
	struct ezcfg_list_node *next = entry->next;

	next->prev = prev;
	prev->next = next;

	entry->prev = NULL;
	entry->next = NULL;
}

/* return list entry which embeds this node */
static struct ezcfg_list_entry *list_node_to_entry(struct ezcfg_list_node *node)
{
	char *list;

	list = (char *)node;
	list -= offsetof(struct ezcfg_list_entry, node);
	return (struct ezcfg_list_entry *)list;
}

/* insert entry into a list as the last element  */
void ezcfg_list_entry_append(struct ezcfg_list_entry *new, struct ezcfg_list_node *list)
{
	/* inserting before the list head make the node the last node in the list */
	ezcfg_list_node_insert_between(&new->node, list->prev, list);
	new->list = list;
}

/* remove entry from a list */
void ezcfg_list_entry_remove(struct ezcfg_list_entry *entry)
{
	ezcfg_list_node_remove(&entry->node);
	entry->list = NULL;
}

/* insert entry into a list, before a given existing entry */
void ezcfg_list_entry_insert_before(
	struct ezcfg_list_entry *new,
	struct ezcfg_list_entry *entry)
{
	ezcfg_list_node_insert_between(&new->node, entry->node.prev, &entry->node);
	new->list = entry->list;
}

struct ezcfg_list_entry *ezcfg_list_entry_add(
	struct ezcfg *ezcfg,
	struct ezcfg_list_node *list,
	const char *name,
	const char *value,
	int unique, int sort)
{
	struct ezcfg_list_entry *entry_loop = NULL;
	struct ezcfg_list_entry *entry_new;

	if (unique) {
		ezcfg_list_entry_foreach(entry_loop, ezcfg_list_get_entry(list)) {
			if (strcmp(entry_loop->name, name) == 0) {
				free(entry_loop->value);
				if (value == NULL) {
					entry_loop->value = NULL;
					return entry_loop;
				}
				entry_loop->value = strdup(value);
				if (entry_loop->value == NULL)
					return NULL;
				return entry_loop;
			}
		}
	}

	if (sort) {
		ezcfg_list_entry_foreach(entry_loop, ezcfg_list_get_entry(list)) {
			if (strcmp(entry_loop->name, name) > 0)
				break;
		}
	}

	entry_new = malloc(sizeof(struct ezcfg_list_entry));
	if (entry_new == NULL) {
		return NULL;
	}
	memset(entry_new, 0, sizeof(struct ezcfg_list_entry));
	entry_new->ezcfg = ezcfg;
	entry_new->name = strdup(name);
	if (entry_new->name == NULL) {
		free(entry_new);
		return NULL;
	}
	if (value != NULL) {
		entry_new->value = strdup(value);
		if (entry_new->value == NULL) {
			free(entry_new->name);
			free(entry_new);
			return NULL;
		}
	}
	if (entry_loop != NULL) {
		ezcfg_list_entry_insert_before(entry_new, entry_loop);
	}
	else {
		ezcfg_list_entry_append(entry_new, list);
	}
	return entry_new;
}

void ezcfg_list_entry_delete(struct ezcfg_list_entry *entry)
{
	ASSERT(entry != NULL);

	ezcfg_list_node_remove(&entry->node);
	free(entry->name);
	free(entry->value);
	free(entry);
}

struct ezcfg_list_entry *ezcfg_list_get_entry(struct ezcfg_list_node *list)
{
	if (ezcfg_list_is_empty(list))
		return NULL;
	return list_node_to_entry(list->next);
}

/**
 * ezcfg_list_entry_get_next:
 * @list_entry: current entry
 *
 * Returns: the next entry from the list, #NULL is no more entries are found.
 */
struct ezcfg_list_entry *ezcfg_list_entry_get_next(struct ezcfg_list_entry *list_entry)
{
	struct ezcfg_list_node *next;

	if (list_entry == NULL)
		return NULL;
	next = list_entry->node.next;
	/* empty list or no more entries */
	if (next == list_entry->list)
		return NULL;
	return list_node_to_entry(next);
}

/**
 * ezcfg_list_entry_get_by_name:
 * @list_entry: current entry
 * @name: name string to match
 *
 * Returns: the entry where @name matched, #NULL if no matching entry is found.
 */
struct ezcfg_list_entry *ezcfg_list_entry_get_by_name(
	struct ezcfg_list_entry *list_entry,
	const char *name)
{
	struct ezcfg_list_entry *entry;

	ezcfg_list_entry_foreach(entry, list_entry) {
		if (strcmp(ezcfg_list_entry_get_name(entry), name) == 0) {
			return entry;
		}
	}
	return NULL;
}

/**
 * ezcfg_list_entry_get_name:
 * @list_entry: current entry
 *
 * Returns: the name string of this entry.
 */
const char *ezcfg_list_entry_get_name(struct ezcfg_list_entry *list_entry)
{
	if (list_entry == NULL)
		return NULL;
	return list_entry->name;
}

/**
 * ezcfg_list_entry_get_value:
 * @list_entry: current entry
 *
 * Returns: the value string of this entry.
 */
const char *ezcfg_list_entry_get_value(struct ezcfg_list_entry *list_entry)
{
	if (list_entry == NULL)
		return NULL;
	return list_entry->value;
}
