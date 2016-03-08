/*
 * multithread.c
 */

#include <stdio.h>
#include <pthread.h>
#include <curl/curl.h>

#define NUMT 4

const char * const urls[NUMT] = {
     "http://www.opensource.apple.com/source/curl/curl-90/curl/COPYING?txt",
     "http://www.opensource.apple.com/source/hfs/hfs-305.10.1/fsck_hfs/fsck_keys.h?txt",
     "http://www.opensource.apple.com/source/sudo/sudo-77/src/toke.h?txt",
     "http://www.opensource.apple.com/source/bmalloc/bmalloc-7601.3.4/bmalloc/Zone.h?txt"
};

static void *pull_one_url(void *url)
{
     CURL *curl = curl_easy_init();

     curl_easy_init();
     curl_easy_setopt(curl, CURLOPT_URL, url);
     curl_easy_perform(curl);
     curl_easy_cleanup(curl);

     return NULL;
}

int main(int argc, char **argv)
{
     pthread_t tid[NUMT];
     int i, error;

     curl_global_init(CURL_GLOBAL_ALL);

     for (i = 0; i < NUMT; i++) {
	  error = pthread_create(&tid[i], NULL, pull_one_url, (void *)urls[i]);

	  if (0 != error)
	       fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
	  else
	       fprintf(stderr, "Thread %d, gets %s\n", i, urls[i]);
     }

     for (i = 0; i < NUMT; i++) {
	  error = pthread_join(tid[i], NULL);
	  fprintf(stderr, "Thread %d terminated\n", i);
     }

     return 0;
}
