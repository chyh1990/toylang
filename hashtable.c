
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"


unsigned int hashtable_hash_fnv(const void *key, unsigned int size)
{
	unsigned int hval = 0x811c9dc5;
	unsigned char *bp = (unsigned char *)key;
    unsigned char *be = bp + size;

    while (bp < be) 
	{
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
		hval ^= (unsigned int)*bp++;
    }

    return (unsigned int)hval;
}

int hashtable_count(hashtable_t *hashtable)
{
  return hashtable->entry_count;
}

void hashtable_init(hashtable_t *hashtable, unsigned int size, hashtable_hash_function hash)
{
  if(!hashtable)
    return;
  memset(hashtable, 0, sizeof(hashtable_t));
  hashtable->tablesize = size;
  hashtable->entries = (hashtableentry_t*)calloc(size, sizeof(hashtableentry_t));
  if(!hash)
    hash = hashtable_hash_fnv;
  hashtable->hash = hash;

  hashtable->head.down = &hashtable->head;
  hashtable->head.up   = &hashtable->head;
}

static hashtableentry_t *ht_search(hashtable_t* ht, const void* k, unsigned int kl, unsigned int idx, void ** v)
{
  hashtableentry_t* e = &ht->entries[idx];
  while( (e = e->next) != NULL ){
    if( kl==e->keylen && !memcmp(k, e->key, kl) )
      break;
  }
  if(e && v)
    *v = e->v;
  return e;
}

static void *copy_key(const void *key, unsigned int l)
{
  void *k = malloc(l);
  memcpy(k, key, l);
  return k;
}

void hashtable_set(hashtable_t *ht, const void *key, unsigned int keylen, void *value)
{
  unsigned int hv = ht->hash(key, keylen) % ht->tablesize;
  hashtableentry_t *e = NULL;
  if( (e = ht_search(ht, key, keylen, hv, NULL)) != NULL){
    e->v = value;
    return;
  }
  e = (hashtableentry_t*)calloc(1, sizeof(hashtableentry_t));
  if(!e) return;
  e->key = copy_key(key, keylen);
  e->keylen = keylen;
  e->v = value;
  
  e->next = ht->entries[hv].next;
  ht->entries[hv].next = e;

  e->down = ht->head.down;
  e->up   = &ht->head;
  ht->head.down->up = e;
  ht->head.down = e;
  ht->entry_count++;
}

int hashtable_get(hashtable_t *ht, const void *key, unsigned int keylen, void **value)
{
  unsigned int hv = ht->hash(key, keylen) % ht->tablesize;
  hashtableentry_t* e = ht_search(ht, key, keylen, hv, value);
  return (e != NULL);
}

void hashtable_remove(hashtable_t *ht, const void *key, unsigned int keylen)
{
  unsigned int idx = ht->hash(key, keylen) % ht->tablesize;
  hashtableentry_t* e = &ht->entries[idx];
  hashtableentry_t* p = e; 
   while( (e = e->next) != NULL ){
    if( keylen==e->keylen && !memcmp(key, e->key, keylen) )
      break;
    p = e;
  }
  if(e){
    p->next = e->next;
    e->up->down = e->down;
    e->down->up = e->up;
    free(e->key);
    free(e);
    ht->entry_count --;
  }
}

void hashtable_foreach(hashtable_t *ht, hashtable_iter_func cb, void *priv)
{
  hashtableentry_t *e = &ht->head;
  while( (e = e->down) != &ht->head ){
    if(!cb(e, priv))
      return;
  }
}

static int mycb(hashtableentry_t *e, void *p)
{
  printf("k=%s v=%d\n", (const char*)e->key, (int)e->v);
  return 1;
}

static void hashtable_test()
{
  hashtable_t ht;
  int i, v= 0, ret;
  hashtable_init(&ht, 307, NULL);
  char buf[] = "abcd";
  for(i=0;i<120;i++){
    buf[3] = i;
    hashtable_set(&ht, buf,4,(void*)(i+1));
  }

  ret = hashtable_get(&ht, "abcd",4,(void**)&v);
  printf("ret=%d v=%d cnt=%d\n", ret, v, hashtable_count(&ht));
  hashtable_set(&ht, "abcd",4,(void*)1234);
  ret = hashtable_get(&ht, "abcd",4,(void**)&v);
  printf("ret=%d v=%d cnt=%d\n", ret, v, hashtable_count(&ht));
  hashtable_remove(&ht, "abce",4);
  printf("ret=%d v=%d cnt=%d\n", ret, v, hashtable_count(&ht));

  hashtable_foreach(&ht, mycb, NULL); 
}

#if 0
int main(){
  hashtable_test();
}
#endif

