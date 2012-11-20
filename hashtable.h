#ifndef HASHTABLE_H
#define HASHTABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef unsigned int (*hashtable_hash_function)(const void *key, unsigned int size);

	typedef struct hashtableentry
	{
    void *key;
    unsigned int keylen;
		void *v;
    struct hashtableentry *next;
    struct hashtableentry *up;
    struct hashtableentry *down;
	} hashtableentry_t;

	typedef struct
	{
		hashtableentry_t *entries;
    hashtableentry_t head;
		hashtable_hash_function hash;
		int entry_count;
    int tablesize;
	} hashtable_t;

	// Hashtable functions
	void hashtable_init(hashtable_t *hashtable, unsigned int size, hashtable_hash_function hash);
	void hashtable_destroy(hashtable_t *hashtable);

	void hashtable_set(hashtable_t *hashtable, const void *key, unsigned int keylen, void *value);
	int hashtable_get(hashtable_t *hashtable, const void *key, unsigned int keylen, void **value);
	void hashtable_remove(hashtable_t *hashtable, const void *key, unsigned int keylen);

	int hashtable_count(hashtable_t *hashtable);
	void hashtable_clear(hashtable_t *hashtable);

	typedef int (*hashtable_iter_func)(hashtableentry_t *e, void* priv);
  void hashtable_foreach(hashtable_t *ht, hashtable_iter_func cb, void * priv);

	// General hash functions
	unsigned int hashtable_hash_fnv(const void *key, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
