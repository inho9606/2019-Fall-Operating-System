#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "limits.h"
#include "round.h"
#include "debug.h"
int list_cnt = 0, hash_cnt = 0, bitmap_cnt = 0;
struct list ds_list[10];
struct hash ds_hash[10];
struct bitmap_with_name ds_bitmap[10];

int check_list(char* target) {
	int i;
	for(i=0; i<list_cnt; i++) {
		if(strcmp(ds_list[i].name, target) == 0) return i;
	}
	return -1;
}
bool compare(const struct list_elem *a, const struct list_elem *b, void *aux) {
	return list_entry (a, struct list_item, elem)->data < list_entry (b, struct list_item, elem)->data;
}
void unique() {
	char *temp, ds_name[20] = "", ds_name2[20] = "", ds[40];
	int i, j;
	scanf("%[^\n]s", ds);
	temp = strtok(ds, " ");
	strcpy(ds_name, temp);
	temp = strtok(NULL, " ");
	if(temp != NULL) strcpy(ds_name2, temp);
	i = check_list(ds_name);
	j = check_list(ds_name2);
	if(i >= 0 && j >= 0) list_unique(&ds_list[i], &ds_list[j], compare, NULL);
	else if(i >= 0 && j < 0) list_unique(&ds_list[i], NULL, compare, NULL);
}

unsigned hash_function(const struct hash_elem *e, void *aux) {
	return hash_int(hash_entry(e, struct hash_item, elem)->data);
}
bool hash_compare(const struct hash_elem *a, const struct hash_elem *b, void *aux) {
	return hash_entry(a, struct hash_item, elem)->data < hash_entry(b, struct hash_item, elem)->data;
}
int check_hash(char* target) {
	int i;
	for(i=0; i<hash_cnt; i++)
		if(strcmp(ds_hash[i].name, target) == 0) return i;
	return -2;
}
void clear(struct hash_elem *e, void *aux) {
	free(hash_entry(e, struct hash_item, elem));
}
void square(struct hash_elem *e, void *aux) {
	int data = hash_entry(e, struct hash_item, elem)->data;
	hash_entry(e, struct hash_item, elem)->data = data*data;
}
void triple(struct hash_elem *e, void *aux) {
	int data = hash_entry(e, struct hash_item, elem)->data;
	hash_entry(e, struct hash_item, elem)->data = data*data*data;
}

int check_bitmap(char* target) {
	int i;
	for(i=0; i<bitmap_cnt; i++)
		if(strcmp(ds_bitmap[i].name, target) == 0) return i;
	return -3;
}

int main() {
	char command[20], ds[20], ds_name[20], ds_name2[20];
	int data, i, j;
	struct list_item *new;
	struct list_elem* ptr, *ptr2, *ptr3;
	struct hash_iterator h_ptr;
	struct hash_item *h_new;
	while(1) {
		scanf("%s", command);
		if(strcmp(command, "quit") == 0) break;
		else if(strcmp(command, "create") == 0) {
			scanf("%s %s", ds, ds_name);
			if(strcmp(ds, "list") == 0 && list_cnt <= 10)
				list_init(&(ds_list[list_cnt++]), ds_name);
			else if(strcmp(ds, "hashtable") == 0)
				hash_init(&ds_hash[hash_cnt++], ds_name, hash_function, hash_compare, NULL);
			else if(strcmp(ds, "bitmap") == 0) {
				scanf("%d", &data);
				strcpy(ds_bitmap[bitmap_cnt].name, ds_name);
				ds_bitmap[bitmap_cnt++].bitmap = bitmap_create(data);
			}
		}
		else if(strcmp(command, "delete") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0) {
				for(; i<list_cnt-1; i++)
					ds_list[i] = ds_list[i+1];
				list_cnt -= 1;
			}
			else if(i == -1) {
				i = check_hash(ds_name);
				if(i >= 0) {
					for(j=i; j<hash_cnt-1; j++)
						ds_hash[j] = ds_hash[j+1];
					hash_clear(&ds_hash[hash_cnt-1], clear);
					hash_cnt -= 1;
				}
				else if(i == -2) {
				i = check_bitmap(ds_name);
				bitmap_destroy(ds_bitmap[i].bitmap);
				for(j=i; j<bitmap_cnt-1; j++)
					ds_bitmap[j] = ds_bitmap[j+1];
				bitmap_destroy(ds_bitmap[bitmap_cnt-1].bitmap);
				strcpy(ds_bitmap[bitmap_cnt-1].name, "");
				bitmap_cnt -= 1;
				}
			}
		}
		else if(strcmp(command, "dumpdata") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0 && list_empty(&ds_list[i]) == 0) {
				for(ptr = list_begin(&ds_list[i]); ptr != &ds_list[i].tail; ptr = ptr->next)
					printf("%d ", list_entry(ptr, struct list_item, elem)->data);
				printf("\n");
			}
			else if(i == -1) {
				i = check_hash(ds_name);
				if(i >= 0) {
					hash_first(&h_ptr, &ds_hash[i]);
					while(hash_next(&h_ptr))
						printf("%d ", hash_entry(hash_cur(&h_ptr), struct hash_item, elem)->data);
					printf("\n");
				}
				else if(i == -2) {
					i = check_bitmap(ds_name);
					if(i >= 0) {
						for(j=0; j<(int)bitmap_size(ds_bitmap[i].bitmap); j++)
							printf("%d", bitmap_test(ds_bitmap[i].bitmap, j));
							printf("\n");
					}
				}
			}
		}
		else if(strcmp(command, "list_push_front") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				list_push_front(&ds_list[i], &new->elem);
			}
		}
		else if(strcmp(command, "list_pop_front") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_list(ds_name);
			if(i >= 0)
				list_pop_front(&ds_list[i]);
		}
		else if(strcmp(command, "list_push_back") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				list_push_back(&ds_list[i], &new->elem);
			}
		}
		else if(strcmp(command, "list_pop_back") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_list(ds_name);
			if(i >= 0)
				list_pop_back(&ds_list[i]);
		}
		else if(strcmp(command, "list_front") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0)
				printf("%d\n", list_entry(list_front(&ds_list[i]), struct list_item, elem)->data);
		}
		else if(strcmp(command, "list_back") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0)
				printf("%d\n", list_entry(list_back(&ds_list[i]), struct list_item, elem)->data);
		}
		else if(strcmp(command, "list_insert") == 0) {
			scanf("%s %d %d", ds_name, &j, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr = list_begin(&ds_list[i]);
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				for(i=0; i<j; i++) ptr = ptr->next;
				list_insert(ptr, &new->elem);
			}
		}
		else if(strcmp(command, "list_insert_ordered") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				new = (struct list_item*)malloc(sizeof(struct list_item));
				new->data = data;
				list_insert_ordered(&ds_list[i], &new->elem, compare, NULL);
			}
		}
		else if(strcmp(command, "list_remove") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr = list_begin(&ds_list[i]);
				for(j=0; j<data; j++) ptr = ptr->next;
				list_remove(ptr);
			}
		}
		else if(strcmp(command, "list_splice") == 0) {
			scanf("%s %d", ds_name, &j);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr = list_begin(&ds_list[i]);
				for(i=0; i<j; i++) ptr = ptr->next;
			}
			else {
				scanf("%s %d %d", ds_name, &j, &data);
				continue;
			}
			scanf("%s %d %d", ds_name, &j, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr2 = list_begin(&ds_list[i]);
				for(i=0; i<j; i++) ptr2 = ptr2->next;
				ptr3 = ptr2;
				for(i=j; i<data; i++) ptr3 = ptr3->next;
				list_splice(ptr, ptr2, ptr3);
			}
		}
		else if(strcmp(command, "list_unique") == 0) {
			unique();
		}
		else if(strcmp(command, "list_empty") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0)
				if(list_empty(&ds_list[i]) == 1) printf("true\n");
				else printf("false\n");
		}
		else if(strcmp(command, "list_size") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0)
				printf("%zu\n", list_size(&ds_list[i]));
		}
		else if(strcmp(command, "list_max") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr = list_max(&ds_list[i], compare, NULL);
				printf("%d\n", list_entry(ptr, struct list_item, elem)->data);
			}
		}
		else if(strcmp(command, "list_min") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr = list_min(&ds_list[i], compare, NULL);
				printf("%d\n", list_entry(ptr, struct list_item, elem)->data);
			}
		}
		else if(strcmp(command, "list_sort") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0)
				list_sort(&ds_list[i], compare, NULL);
		}
		else if(strcmp(command, "list_reverse") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0)
				list_reverse(&ds_list[i]);
		}
		else if(strcmp(command, "list_swap") == 0) {
			scanf("%s %d %d", ds_name, &j, &data);
			i = check_list(ds_name);
			if(i >= 0) {
				ptr = list_begin(&ds_list[i]);
				ptr2 = list_begin(&ds_list[i]);
				for(i=0; i<j; i++) ptr = ptr->next;
				for(i=0; i<data; i++) ptr2 = ptr2->next;
				list_swap(ptr, ptr2);
			}
		}
		else if(strcmp(command, "list_shuffle") == 0) {
			scanf("%s", ds_name);
			i = check_list(ds_name);
			if(i >= 0) list_shuffle(&ds_list[i]);
		}
		else if(strcmp(command, "hash_insert") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_hash(ds_name);
			if(i >= 0) {
				h_new = (struct hash_item*)malloc(sizeof(struct hash_item));
				h_new->data = data;
				hash_insert(&ds_hash[i], &h_new->elem);
			}
		}
		else if(strcmp(command, "hash_replace") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_hash(ds_name);
			if(i >= 0) {
				h_new = (struct hash_item*)malloc(sizeof(struct hash_item));
				h_new->data = data;
				hash_replace(&ds_hash[i], &h_new->elem);
			}
		}
		else if(strcmp(command, "hash_apply") == 0) {
			scanf("%s %s", ds_name, command);
			i = check_hash(ds_name);
			if(i >= 0) {
				if(strcmp(command, "square") == 0) hash_apply(&ds_hash[i], square);
				else if(strcmp(command, "triple") == 0) hash_apply(&ds_hash[i], triple);
			}
		}
		else if(strcmp(command, "hash_delete") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_hash(ds_name);
			if(i >= 0) {
				hash_first(&h_ptr, &ds_hash[i]);
				while(hash_next(&h_ptr))
					if(hash_entry(hash_cur(&h_ptr), struct hash_item, elem)->data == data) {
						hash_delete(&ds_hash[i], &(hash_entry(hash_cur(&h_ptr), struct hash_item, elem)->elem));
						break;
					}
			}
		}
		else if(strcmp(command, "hash_find") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_hash(ds_name);
			if(i >= 0) {
				hash_first(&h_ptr, &ds_hash[i]);
				while(hash_next(&h_ptr))
					if(hash_entry(hash_cur(&h_ptr), struct hash_item, elem)->data == data) {
						if(hash_find(&ds_hash[i], &(hash_entry(hash_cur(&h_ptr), struct hash_item, elem)->elem)) != 0) printf("%d\n", data);
						break;
					}
			}
		}
		else if(strcmp(command, "hash_empty") == 0) {
			scanf("%s", ds_name);
			i = check_hash(ds_name);
			if(i >= 0) {
				if(hash_empty(&ds_hash[i]) == 1) printf("true\n");
				else printf("false\n");
			}
		}
		else if(strcmp(command, "hash_size") == 0) {
			scanf("%s", ds_name);
			i = check_hash(ds_name);
			if(i >= 0)
				printf("%zu\n", hash_size(&ds_hash[i]));
		}
		else if(strcmp(command, "hash_clear") == 0) {
			scanf("%s", ds_name);
			i = check_hash(ds_name);
			if(i >= 0)
				hash_clear(&ds_hash[i], clear);
		}
		else if(strcmp(command, "bitmap_mark") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_bitmap(ds_name);
			if(i >= 0)
				bitmap_mark(ds_bitmap[i].bitmap, data);
		}
		else if(strcmp(command, "bitmap_test") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(bitmap_test(ds_bitmap[i].bitmap, data) == 1) printf("true\n");
				else printf("false\n");
		}
		else if(strcmp(command, "bitmap_size") == 0) {
			scanf("%s", ds_name);
			i = check_bitmap(ds_name);
			if(i >= 0)
				printf("%zu\n", bitmap_size(ds_bitmap[i].bitmap));
		}
		else if(strcmp(command, "bitmap_set_all") == 0) {
			scanf("%s %s", ds_name, command);
			i = check_bitmap(ds_name);
			if(i >= 0) {
				if(strcmp(command, "true") == 0) bitmap_set_all(ds_bitmap[i].bitmap, 1);
				else if(strcmp(command, "false") == 0) bitmap_set_all(ds_bitmap[i].bitmap, 0);
			}
		}
		else if(strcmp(command, "bitmap_reset") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_bitmap(ds_name);
			if( i>= 0)
				bitmap_reset(ds_bitmap[i].bitmap, data);
		}
		else if(strcmp(command, "bitmap_set") == 0) {
			scanf("%s %d %s", ds_name, &data, command);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(strcmp(command, "true") == 0) bitmap_set(ds_bitmap[i].bitmap, data, 1);
				else if(strcmp(command, "false") == 0) bitmap_set(ds_bitmap[i].bitmap, data, 0);
		}
		else if(strcmp(command, "bitmap_set_multiple") == 0) {
			scanf("%s %d %d %s", ds_name, &data, &j, command);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(strcmp(command, "true") == 0) bitmap_set_multiple(ds_bitmap[i].bitmap, data, j, 1);
				else if(strcmp(command, "false") == 0) bitmap_set_multiple(ds_bitmap[i].bitmap, data, j, 0);
		}
		else if(strcmp(command, "bitmap_all") == 0) {
			scanf("%s %d %d", ds_name, &data, &j);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(bitmap_all(ds_bitmap[i].bitmap, data, j) == 1) printf("true\n");
				else printf("false\n");
		}
		else if(strcmp(command, "bitmap_contains") == 0) {
			scanf("%s %d %d %s", ds_name, &data, &j, command);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(strcmp(command, "true") == 0) printf("%s\n", bitmap_contains(ds_bitmap[i].bitmap, data, j, 1) ? "true" : "false");
				else if(strcmp(command, "false") == 0) printf("%s\n", bitmap_contains(ds_bitmap[i].bitmap, data, j, 0) ? "true" : "false");
		}
		else if(strcmp(command, "bitmap_none") == 0) {
			scanf("%s %d %d", ds_name, &data, &j);
			if( i>= 0)
			printf("%s\n", bitmap_none(ds_bitmap[i].bitmap, data, j) ? "true" : "false");
		}
		else if(strcmp(command, "bitmap_any") == 0) {
			scanf("%s %d %d", ds_name, &data, &j);
			if( i>= 0)
			printf("%s\n", bitmap_any(ds_bitmap[i].bitmap, data, j) ? "true" : "false");
		}
		else if(strcmp(command, "bitmap_dump") == 0) {
			scanf("%s", ds_name);
			if( i>= 0)
				bitmap_dump(ds_bitmap[i].bitmap);
		}
		else if(strcmp(command, "bitmap_flip") == 0) {
			scanf("%s %d", ds_name, &data);
			i = check_bitmap(ds_name);
			if(i >= 0)
				bitmap_flip(ds_bitmap[i].bitmap, data);
		}
		else if(strcmp(command, "bitmap_scan") == 0) {
			scanf("%s %d %d %s", ds_name, &data, &j, command);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(strcmp(command, "true") == 0) printf("%u\n", bitmap_scan(ds_bitmap[i].bitmap, data, j, 1));
				else if(strcmp(command, "false") == 0) printf("%u\n", bitmap_scan(ds_bitmap[i].bitmap, data, j, 0));
		}
		else if(strcmp(command, "bitmap_scan_and_flip") == 0) {
			scanf("%s %d %d %s", ds_name, &data, &j, command);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(strcmp(command, "true") == 0) printf("%u\n", bitmap_scan_and_flip(ds_bitmap[i].bitmap, data, j, 1));
				else if(strcmp(command, "false") == 0) printf("%u\n", bitmap_scan_and_flip(ds_bitmap[i].bitmap, data, j, 0));
		}
		else if(strcmp(command, "bitmap_count") == 0) {
			scanf("%s %d %d %s", ds_name, &data, &j, command);
			i = check_bitmap(ds_name);
			if(i >= 0)
				if(strcmp(command, "true") == 0) printf("%zu\n", bitmap_count(ds_bitmap[i].bitmap, data, j, 1));
				else if(strcmp(command, "false") == 0) printf("%zu\n", bitmap_count(ds_bitmap[i].bitmap, data, j, 0));
		}
		else if(strcmp(command, "bitmap_expand") == 0) {
			scanf("%s %d", ds_name, &j);
			i = check_bitmap(ds_name);
			if(i >= 0)
				bitmap_expand(ds_bitmap[i].bitmap, j);
		}
	}
	return 0;
}