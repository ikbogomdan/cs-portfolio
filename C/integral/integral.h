#pragma once

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wait.h"

typedef double field_t;

typedef field_t func_t(field_t);

#define MIN(A, B) (((A) < (B) ? (A) : (B)))

enum integrator_state { STATE_IDLE, STATE_RUNNING, STATE_TERMINATE };

typedef struct task_info {
  func_t* func;
  field_t left_bound;
  field_t right_bound;
} task_info_t;

struct thread_info;

typedef struct par_integrator {
  size_t threads_num;

  _Atomic uint32_t threads_done;

  task_info_t task_info;

  pthread_t* threads;
  struct thread_info* thread_infos;
  field_t* results;
} par_integrator_t;

typedef struct thread_info {
  par_integrator_t* integrator;
  int thread_id;
  _Atomic uint32_t state;
  field_t* result;

} thread_info_t;

const field_t kEpsilon = 1e-7;

void* thread_routine(void* argument) {
  thread_info_t* thread_info = (thread_info_t*)argument;

  uint32_t current_state;
  while (true) {
    while ((current_state = thread_info->state) == STATE_IDLE) {
      atomic_wait(&thread_info->state, STATE_IDLE);
    }

    if (current_state == STATE_TERMINATE) {
      break;
    }

    task_info_t current_task;
    memcpy(&current_task, &thread_info->integrator->task_info,
           sizeof(task_info_t));

    field_t current_result = 0.0;

    field_t current_left_bound =
        current_task.left_bound +
        (current_task.right_bound - current_task.left_bound) /
            thread_info->integrator->threads_num * thread_info->thread_id;
    field_t current_right_bound =
        current_task.left_bound +
        (current_task.right_bound - current_task.left_bound) /
            thread_info->integrator->threads_num * (thread_info->thread_id + 1);

    for (field_t argument = current_left_bound; argument < current_right_bound;
         argument += kEpsilon) {
      current_result += current_task.func(argument) *
                        MIN(kEpsilon, current_right_bound - argument);
    }

    *thread_info->result = current_result;
    ++thread_info->integrator->threads_done;
    thread_info->state = STATE_IDLE;

    atomic_notify_one(&thread_info->integrator->threads_done);
  }

  return NULL;
}

int par_integrator_init(par_integrator_t* self, size_t threads_num) {
  self->threads_num = threads_num;

  self->threads_done = threads_num;

  self->threads = (pthread_t*)malloc(self->threads_num * sizeof(pthread_t));
  self->thread_infos =
      (thread_info_t*)malloc(self->threads_num * sizeof(thread_info_t));
  self->results = (field_t*)malloc(self->threads_num * sizeof(field_t));

  for (uint32_t thread_index = 0; thread_index < threads_num; ++thread_index) {
    self->thread_infos[thread_index].integrator = self;
    self->thread_infos[thread_index].thread_id = thread_index;
    self->thread_infos[thread_index].state = STATE_IDLE;
    self->thread_infos[thread_index].result = &self->results[thread_index];

    if (pthread_create(&self->threads[thread_index], NULL, thread_routine,
                       &self->thread_infos[thread_index]) != 0) {
      for (uint32_t cancelled_thread_index = 0;
           cancelled_thread_index < thread_index; ++cancelled_thread_index) {
        pthread_cancel(self->threads[cancelled_thread_index]);
        pthread_join(self->threads[cancelled_thread_index], NULL);
      }

      free(self->threads);
      free(self->thread_infos);
      free(self->results);

      return -1;
    }
  }

  return 0;
}

int par_integrator_start_calc(par_integrator_t* self, func_t* func,
                              field_t left_bound, field_t right_bound) {
  if (self->threads_done != self->threads_num) {
    return -1;
  }

  self->threads_done = 0;

  self->task_info.func = func;
  self->task_info.left_bound = left_bound;
  self->task_info.right_bound = right_bound;

  for (size_t thread_index = 0; thread_index < self->threads_num;
       ++thread_index) {
    self->thread_infos[thread_index].state = STATE_RUNNING;

    atomic_notify_one(&self->thread_infos[thread_index].state);
  }

  return 0;
}

int par_integrator_get_result(par_integrator_t* self, field_t* result) {
  for (uint32_t threads_done = self->threads_done;
       (threads_done = self->threads_done) < self->threads_num;
       atomic_wait(&self->threads_done, threads_done)) {
  }

  *result = 0.0;
  for (uint32_t thread_index = 0; thread_index < self->threads_num;
       ++thread_index) {
    *result += self->results[thread_index];
  }

  return 0;
}

int par_integrator_destroy(par_integrator_t* self) {
  for (uint32_t thread_index = 0; thread_index < self->threads_num;
       ++thread_index) {
    self->thread_infos[thread_index].state = STATE_TERMINATE;

    atomic_notify_one(&self->thread_infos[thread_index].state);

    pthread_join(self->threads[thread_index], NULL);
  }

  free(self->threads);
  free(self->thread_infos);
  free(self->results);

  return 0;
}
