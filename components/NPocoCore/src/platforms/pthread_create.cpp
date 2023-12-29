/*
	pthread_create.cpp - Basic extension for the ESP-IDF pthread implementation to add 
							static stack support.
							
*/

#include <time.h>
#include <errno.h>

#include <string.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "sys/queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/soc_memory_layout.h"

//#include "pthread_internal.h"
#include "esp_pthread.h"

#include "esp_log.h"
const static char *TAG = "pthreadx";

/** task state */
enum esp_pthread_task_state {
    PTHREAD_TASK_STATE_RUN,
    PTHREAD_TASK_STATE_EXIT
};

/** pthread thread FreeRTOS wrapper */
typedef struct esp_pthread_entry {
    SLIST_ENTRY(esp_pthread_entry)  list_node;  ///< Tasks list node struct.
    TaskHandle_t                handle;         ///< FreeRTOS task handle
    TaskHandle_t                join_task;      ///< Handle of the task waiting to join
    enum esp_pthread_task_state state;          ///< pthread task state
    bool                        detached;       ///< True if pthread is detached
    void                       *retval;         ///< Value supplied to calling thread during join
    void                       *task_arg;       ///< Task arguments
} esp_pthread_t;

/** pthread wrapper task arg */
typedef struct {
    void *(*func)(void *);  ///< user task entry
    void *arg;              ///< user task argument
    esp_pthread_cfg_t cfg;  ///< pthread configuration
	StaticTask_t*	xTaskBuffer;
	StackType_t*  	xStack;
} esp_pthread_task_arg_t;

/** pthread mutex FreeRTOS wrapper */
typedef struct {
    SemaphoreHandle_t   sem;        ///< Handle of the task waiting to join
    int                 type;       ///< Mutex type. Currently supported PTHREAD_MUTEX_NORMAL and PTHREAD_MUTEX_RECURSIVE
} esp_pthread_mutex_t;

//extern pthread_key_t s_pthread_cfg_key;
//extern SemaphoreHandle_t s_threads_mux;
//extern SLIST_HEAD(esp_thread_list_head, esp_pthread_entry) s_threads_list;

/* StaticTask_t* xTaskBuffer;
StackType_t* xStack = 0; */


static void pthread_task_func(void *arg) {
    void *rval = NULL;
    esp_pthread_task_arg_t *task_arg = (esp_pthread_task_arg_t *)arg;

    ESP_LOGV(TAG, "%s ENTER %p", __FUNCTION__, task_arg->func);

    // wait for start
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

    if (task_arg->cfg.inherit_cfg) {
        /* If inherit option is set, then do a set_cfg() ourselves for future forks,
        but first set thread_name to NULL to enable inheritance of the name too.
        (This also to prevents dangling pointers to name of tasks that might
        possibly have been deleted when we use the configuration).*/
        esp_pthread_cfg_t *cfg = &task_arg->cfg;
        cfg->thread_name = NULL;
        esp_pthread_set_cfg(cfg);
    }
	
    ESP_LOGV(TAG, "%s START %p", __FUNCTION__, task_arg->func);
    rval = task_arg->func(task_arg->arg);
    ESP_LOGV(TAG, "%s END %p", __FUNCTION__, task_arg->func);
	
	// Free TCB and stack.
	free(task_arg->xTaskBuffer);
	free(task_arg->xStack);
	task_arg->xTaskBuffer = NULL;
	task_arg->xStack = NULL;

    //pthread_exit(rval);
	vTaskDelete(NULL); // FIXME: does not handle detached threads. Add suspend.

    ESP_LOGV(TAG, "%s EXIT", __FUNCTION__);
}


static int get_default_pthread_core(void) {
    return CONFIG_PTHREAD_TASK_CORE_DEFAULT == -1 ? tskNO_AFFINITY : CONFIG_PTHREAD_TASK_CORE_DEFAULT;
}


int pthread_create_esp32(pthread_t *thread, const pthread_attr_t *attr,
								void *(*start_routine) (void *), void *arg, const char* name,
								StaticTask_t* xTaskBuffer, StackType_t* xStack, int coreId) {
    TaskHandle_t xHandle = NULL;

    ESP_LOGV(TAG, "%s", __FUNCTION__);
    esp_pthread_task_arg_t* task_arg = (esp_pthread_task_arg_t*) calloc(1, sizeof(esp_pthread_task_arg_t));
    if (task_arg == NULL) {
        ESP_LOGE(TAG, "Failed to allocate task args!");
        return ENOMEM;
    }

    esp_pthread_t *pthread = (esp_pthread_t*) calloc(1, sizeof(esp_pthread_t));
    if (pthread == NULL) {
        ESP_LOGE(TAG, "Failed to allocate pthread data!");
        free(task_arg);
        return ENOMEM;
    }

    uint32_t stack_size = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT;
    BaseType_t prio = CONFIG_PTHREAD_TASK_PRIO_DEFAULT;
    BaseType_t core_id = get_default_pthread_core();
    //const char *task_name = CONFIG_PTHREAD_TASK_NAME_DEFAULT;
    const char *task_name = name;
	
	// Pin the task to a specific core, limited to ESP32 core 0 or 1.
	if (coreId > -1) {
		if (coreId == 0 || coreId == 1) {
			core_id = coreId;
		}
	}

    //esp_pthread_cfg_t *pthread_cfg = (esp_pthread_cfg_t*) pthread_getspecific(s_pthread_cfg_key);
    /* if (pthread_cfg) {
        if (pthread_cfg->stack_size) {
            stack_size = pthread_cfg->stack_size;
        }
        if (pthread_cfg->prio && pthread_cfg->prio < configMAX_PRIORITIES) {
            prio = pthread_cfg->prio;
        }

        if (pthread_cfg->inherit_cfg) {
            if (pthread_cfg->thread_name == NULL) {
                // Inherit task name from current task.
                task_name = pcTaskGetName(NULL);
            } else {
                // Inheriting, but new task name.
                task_name = pthread_cfg->thread_name;
            }
        } else if (pthread_cfg->thread_name == NULL) {
            task_name = CONFIG_PTHREAD_TASK_NAME_DEFAULT;
        } else {
            task_name = pthread_cfg->thread_name;
        }

        if (pthread_cfg->pin_to_core >= 0 && pthread_cfg->pin_to_core < portNUM_PROCESSORS) {
            core_id = pthread_cfg->pin_to_core;
        }

        task_arg->cfg = *pthread_cfg;
    } */

    if (attr) {
        /* Overwrite attributes */
        stack_size = attr->stacksize;

        switch (attr->detachstate) {
        case PTHREAD_CREATE_DETACHED:
            pthread->detached = true;
            break;
        case PTHREAD_CREATE_JOINABLE:
        default:
            pthread->detached = false;
        }
    }

    task_arg->func = start_routine;
    task_arg->arg = arg;
	task_arg->xTaskBuffer = xTaskBuffer;
	task_arg->xStack = xStack;
    pthread->task_arg = task_arg;
	// ---
    /* BaseType_t res = xTaskCreatePinnedToCore(&pthread_task_func,
                                             task_name,
                                             // stack_size is in bytes. This transformation ensures that the units are
                                             // transformed to the units used in FreeRTOS.
                                             // Note: float division of ceil(m / n) ==
                                             //       integer division of (m + n - 1) / n
                                             (stack_size + sizeof(StackType_t) - 1) / sizeof(StackType_t),
                                             task_arg,
                                             prio,
                                             &xHandle,
                                             core_id); */
	// ---
	
	// Allocate task structure (TCB) and stack. Stack is explicitly allocated in PSRAM.
	const uint32_t stack_bytes = (stack_size + sizeof(StackType_t) - 1) / sizeof(StackType_t);
	xTaskBuffer = (StaticTask_t*) calloc(1, sizeof(StaticTask_t));
	//xStack = (StackType_t*) malloc(stack_bytes);
	xStack = (StackType_t*) heap_caps_calloc(1, stack_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	
	xHandle = xTaskCreateStaticPinnedToCore(&pthread_task_func,
													task_name,
													stack_bytes,
													task_arg, 
													prio, 
													xStack, 
													xTaskBuffer,
													core_id);

    /* if (res != pdPASS) {
        ESP_LOGE(TAG, "Failed to create task!");
        free(pthread);
        free(task_arg);
        if (res == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
            return ENOMEM;
        } else {
            return EAGAIN;
        }
    }*/
    pthread->handle = xHandle;

    /* if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
    SLIST_INSERT_HEAD(&s_threads_list, pthread, list_node);
    xSemaphoreGive(s_threads_mux); */

    // start task
    xTaskNotify(xHandle, 0, eNoAction);

    *thread = (pthread_t)pthread; // pointer value fit into pthread_t (uint32_t)

    ESP_LOGV(TAG, "Created task %x", (unsigned int) xHandle);

    return 0;
}


int pthread_join_extram(pthread_t thread, void **retval,
				   StaticTask_t* xTaskBuffer, StackType_t* xStack) {
	//
	esp_pthread_t *pthread = (esp_pthread_t*) thread;
    int ret = 0;
    bool wait = false;
    void* child_task_retval = 0;

    ESP_LOGV(TAG, "%s %p", __FUNCTION__, pthread);

    // find task
    /* if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
        assert(false && "Failed to lock threads list!");
    }
	
    TaskHandle_t handle = pthread_find_handle(thread); */
	
    /* if (!handle) {
        // not found
        ret = ESRCH;
    } else */ if (pthread->detached) {
        // Thread is detached
        ret = EDEADLK;
    } else if (pthread->join_task) {
        // already have waiting task to join
        ret = EINVAL;
    /* } else if (handle == xTaskGetCurrentTaskHandle()) {
        // join to self not allowed
        ret = EDEADLK; */
    } else {
        //esp_pthread_t *cur_pthread = pthread_find(xTaskGetCurrentTaskHandle());
        /* if (cur_pthread && cur_pthread->join_task == handle) {
            // join to each other not allowed
            ret = EDEADLK;
        } else { */
            if (pthread->state == PTHREAD_TASK_STATE_RUN) {
                pthread->join_task = xTaskGetCurrentTaskHandle();
                wait = true;
            } else { // thread has exited and task is already suspended, or about to be suspended
                child_task_retval = pthread->retval;
                //pthread_delete(pthread);
            }
        //}
    }
    //xSemaphoreGive(s_threads_mux);

    if (ret == 0) {
        if (wait) {
            xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
            /* if (xSemaphoreTake(s_threads_mux, portMAX_DELAY) != pdTRUE) {
                assert(false && "Failed to lock threads list!");
            } */
            child_task_retval = pthread->retval;
            //pthread_delete(pthread);
        }
		
	
		// Free TCB and stack.
		free(xTaskBuffer);
		free(xStack);
		xTaskBuffer = NULL;
		xStack = NULL;
        vTaskDelete(pthread->handle);
    }

    if (retval) {
        *retval = child_task_retval;
    }

    ESP_LOGV(TAG, "%s %p EXIT %d", __FUNCTION__, pthread, ret);
    return ret;
}
