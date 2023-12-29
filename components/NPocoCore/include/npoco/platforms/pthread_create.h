

#include <pthread.h>

extern "C" {
int pthread_create_esp32(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg, const char* name,
				   StaticTask_t* xTaskBuffer, StackType_t* xStack);
int pthread_join_extram(pthread_t thread, void **retval,
				   StaticTask_t* xTaskBuffer, StackType_t* xStack, int coreId);
}