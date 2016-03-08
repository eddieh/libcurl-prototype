/*
 * threaded-ssl.c
 */

#include <stdio.h>
#include <pthread.h>
#include <curl/curl.h>
#include <openssl/crypto.h>

#define NUMT 4

static pthread_mutex_t *lockarray;

static void
lock_callback(int mode, int type, char *file, int line)
{
     (void)file;
     (void)line;
     if (mode & CRYPTO_LOCK)
	  pthread_mutex_lock(&(lockarray[type]));
     else
	  pthread_mutex_unlock(&(lockarray[type]));
}

static unsigned long
thread_id(void)
{
     return (unsigned long)pthread_self();
}

static void
init_locks(void)
{
     lockarray = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));

     for (int i = 0; i < CRYPTO_num_locks(); i++)
	  pthread_mutex_init(&(lockarray[i]), NULL);

     CRYPTO_set_id_callback((unsigned long (*)())thread_id);
     CRYPTO_set_locking_callback((void (*)())lock_callback);
}

static void
kill_locks(void)
{
     CRYPTO_set_locking_callback(NULL);
     for (int i = 0; i < CRYPTO_num_locks(); i++)
	  pthread_mutex_destroy(&(lockarray[i]));

     OPENSSL_free(lockarray);
}

const char * const urls[NUMT] = {
     "http://www.opensource.apple.com/source/curl/curl-90/curl/COPYING?txt",
     "http://www.opensource.apple.com/source/hfs/hfs-305.10.1/fsck_hfs/fsck_keys.h?txt",
     "http://www.opensource.apple.com/source/sudo/sudo-77/src/toke.h?txt",
     "http://www.opensource.apple.com/source/bmalloc/bmalloc-7601.3.4/bmalloc/Zone.h?txt"
};

static void *
pull_one_url(void *url)
{
     CURL *curl = curl_easy_init();

     curl_easy_init();
     curl_easy_setopt(curl, CURLOPT_URL, url);

     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

     curl_easy_perform(curl);
     curl_easy_cleanup(curl);

     return NULL;
}

int
main(int argc, char **argv)
{
     pthread_t tid[NUMT];
     int error;

     curl_global_init(CURL_GLOBAL_ALL);

     init_locks();

     for (int i = 0; i < NUMT; i++) {
	  error = pthread_create(&tid[i], NULL, pull_one_url, (void *)urls[i]);

	  if (0 != error)
	       fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
	  else
	       fprintf(stderr, "Thread %d, gets %s\n", i, urls[i]);
     }

     for (int i = 0; i < NUMT; i++) {
	  error = pthread_join(tid[i], NULL);
	  fprintf(stderr, "Thread %d terminated\n", i);
     }

     kill_locks();

     return 0;
}
