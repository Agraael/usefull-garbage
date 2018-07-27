// main.c
#include <stdio.h>
#include "hash_table.h"

int main() {
	ht_hash_table* ht = ht_new();

	ht_insert(ht, "toto", "data1");
	ht_insert(ht, "paul", "data2");
	ht_insert(ht, "pierre", "data3");

	printf("%s\n",  ht_search(ht, "toto"));
	ht_del_hash_table(ht);
}
