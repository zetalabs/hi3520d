/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent_master.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-08-01   0.1       Modify it from thread/master.c
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/*
 * ezcfg_agent_master:
 *
 * Opaque object handling one event source.
 * Multi-threads model - master part.
 */
struct ezcfg_agent_master {
  pthread_t thread_id;
  sigset_t *sigset;
  int sem_id; /* for IPC semaphore control */
  int shm_id; /* for IPC shared memory access */
  size_t shm_size; /* for IPC shared memory access */
  void *shm_addr; /* for IPC shared memory access */
  size_t nvram_queue_length; /* for nvram operation queue length */
  size_t rc_queue_length; /* for rc operation queue length */

  struct ezcfg *ezcfg;
  int stop_flag; /* Should we stop event loop */
  int threads_max; /* MAX number of threads */
  int num_threads; /* Number of threads */
  int num_idle; /* Number of idle threads */

  pthread_mutex_t thread_mutex; /* Protects (max|num)_threads */
  pthread_rwlock_t thread_rwlock; /* Protects options, callbacks */
  pthread_cond_t thread_sync_cond; /* Condvar for thread sync */

  struct ezcfg_socket *listening_sockets;
  pthread_mutex_t ls_mutex; /* Protects listening_sockets */

  struct ezcfg_socket *queue; /* Accepted sockets */
  int sq_len; /* Length of the socket queue */
  int sq_head; /* Head of the socket queue */
  int sq_tail; /* Tail of the socket queue */
  pthread_cond_t sq_empty_cond; /* Socket queue empty condvar */
  pthread_cond_t sq_full_cond;  /* Socket queue full condvar */

  struct ezcfg_agent_core *core; /* Agent core */
  struct ezcfg_agent_worker *workers; /* Worker list */
};

/*
 * Deallocate agent master context, free up the resources
 * only delete master_new() allocated resources before pthread_mutex initialized
 * other resources should be deleted in master_finish()
 */
static void master_delete(struct ezcfg_agent_master *master)
{
  ASSERT (master != NULL);

  if (master->sem_id != -1) {
    /* remove system V semaphore */
    if (semctl(master->sem_id, 0, IPC_RMID) == -1) {
      EZDBG("<6>pid=[%d] semctl IPC_RMID error\n", getpid());
    }
    else {
      EZDBG("<6>pid=[%d] remove sem OK.\n", getpid());
    }
  }

  if (master->shm_addr != (void *)-1) {
    /* detach system V shared memory from system */
    if (shmdt(master->shm_addr) == -1) {
      EZDBG("<6>pid=[%d] shmdt error\n", getpid());
    }
    else {
      EZDBG("<6>pid=[%d] shm detach OK.\n", getpid());
    }
  }
  if (master->shm_id != -1) {
    /* remove system V shared memory from system */
    if (shmctl(master->shm_id, IPC_RMID, NULL) == -1) {
      EZDBG("<6>pid=[%d] shmctl IPC_RMID error\n", getpid());
    }
    else {
      EZDBG("<6>pid=[%d] remove shm OK.\n", getpid());
    }
  }

  if (master->queue != NULL) {
    free(master->queue);
  }
  free(master);
}

/*
 * Deallocate agent master context, free up the resources
 * when master_new() success, this function will be called before master_delete()
 */
static void master_finish(struct ezcfg_agent_master *master)
{
  struct ezcfg_agent_worker *worker;

  pthread_mutex_lock(&(master->thread_mutex));

  /* Close all listening sockets */
  pthread_mutex_lock(&(master->ls_mutex));
  if (master->listening_sockets != NULL) {
    ezcfg_socket_list_delete(&(master->listening_sockets));
    master->listening_sockets = NULL;
  }
  pthread_mutex_unlock(&(master->ls_mutex));

  /* Close all workers' socket */
  worker = master->workers;
  while (worker != NULL) {
    ezcfg_agent_worker_close_connection(worker);
    worker = ezcfg_agent_worker_get_next(worker);
  }

  /* Wait until all threads finish */
  while (master->num_threads > 0)
    pthread_cond_wait(&(master->thread_sync_cond), &(master->thread_mutex));
  master->threads_max = 0;

  pthread_mutex_unlock(&(master->thread_mutex));

  pthread_cond_destroy(&(master->sq_empty_cond));
  pthread_cond_destroy(&(master->sq_full_cond));

  pthread_mutex_destroy(&(master->ls_mutex));

  pthread_cond_destroy(&(master->thread_sync_cond));
  pthread_rwlock_destroy(&(master->thread_rwlock));
  pthread_mutex_destroy(&(master->thread_mutex));

  /* signal ezcd_stop() that we're done */
  master->stop_flag = 2;
}

static int master_init_sem(struct ezcfg_agent_master *master)
{
  int i, key;
  struct sembuf res[EZCFG_SEM_NUMBER];
  char buf[EZCFG_PATH_MAX];
  int ret = EZCFG_RET_FAIL;

  /* prepare semaphore */
  ret = ezcfg_common_get_meta_nvram(master->ezcfg, NVRAM_NAME(META, SEM_EZCFG_PATH), buf, sizeof(buf));
  if (ret != EZCFG_RET_OK) {
    return ret;
  }
  key = ftok(buf, EZCFG_SEM_PROJID_EZCFG);
  if (key == -1) {
    EZDBG("<6>pid=[%d] ftok error.\n", getpid());
    return EZCFG_RET_FAIL;
  }

  /* create a semaphore set */
  /* this is the first place to create the semaphore, must IPC_EXCL */
  master->sem_id = semget(key, EZCFG_SEM_NUMBER, IPC_CREAT|IPC_EXCL|00666);
  if (master->sem_id < 0) {
    EZDBG("<6>pid=[%d] %s(%d) try to create sem error.\n", getpid(), __func__, __LINE__);
    return EZCFG_RET_FAIL;
  }

  /* initialize semaphore */
  for (i=0; i<EZCFG_SEM_NUMBER; i++) {
    res[i].sem_num = i;
    res[i].sem_op = 1;
    res[i].sem_flg = 0;
  }

  if (semop(master->sem_id, res, EZCFG_SEM_NUMBER) == -1) {
    EZDBG("<6>pid=[%d] semop release_res error\n", getpid());
    return EZCFG_RET_FAIL;
  }

  return EZCFG_RET_OK;
}

static bool master_init_shm(struct ezcfg_agent_master *master)
{
  int key;
  int shm_id;
  size_t shm_size;
  void *shm_addr;
  size_t nvram_queue_length; /* the length of nvram operation queue */
  size_t rc_queue_length; /* the length of rc operation queue */
  char buf[EZCFG_PATH_MAX];
  int ret = EZCFG_RET_FAIL;

  /* prepare shared memory */
  ret = ezcfg_common_get_meta_nvram(master->ezcfg, NVRAM_NAME(META, SHM_EZCFG_PATH), buf, sizeof(buf));
  if (ret != EZCFG_RET_OK) {
    return ret;
  }
  key = ftok(buf, EZCFG_SHM_PROJID_EZCFG);
  if (key == -1) {
    EZDBG("<6>pid=[%d] ftok error with errno[%d].\n", getpid(), errno);
    return EZCFG_RET_FAIL;
  }
  ret = ezcfg_common_get_meta_nvram(master->ezcfg, NVRAM_NAME(META, SHM_EZCFG_PATH), buf, sizeof(buf));
  if (ret != EZCFG_RET_OK) {
    return ret;
  }
  shm_size = atoi(buf);
  if (shm_size < ezcfg_shm_get_size()) {
    EZDBG("<6>pid=[%d] shm_size is too small.\n", getpid());
    return EZCFG_RET_FAIL;
  }

  ret = ezcfg_common_get_meta_nvram(master->ezcfg, NVRAM_NAME(META, SHM_EZCFG_NVRAM_QUEUE_LENGTH), buf, sizeof(buf));
  if (ret != EZCFG_RET_OK) {
    return ret;
  }
  nvram_queue_length = atoi(buf);
  if ((nvram_queue_length < EZCFG_SHM_EZCFG_NVRAM_QUEUE_MIN)||
      (nvram_queue_length > EZCFG_SHM_EZCFG_NVRAM_QUEUE_MAX)) {
    return EZCFG_RET_FAIL;
  }

  ret = ezcfg_common_get_meta_nvram(master->ezcfg, NVRAM_NAME(META, SHM_EZCFG_RC_QUEUE_LENGTH), buf, sizeof(buf));
  if (ret != EZCFG_RET_OK) {
    return ret;
  }
  rc_queue_length = atoi(buf);
  if ((rc_queue_length < EZCFG_SHM_EZCFG_RC_QUEUE_MIN) ||
      (rc_queue_length > EZCFG_SHM_EZCFG_RC_QUEUE_MAX)) {
    return EZCFG_RET_FAIL;
  }

  /* create a shared memory */
  /* this is the first place to create the shared memory, must IPC_EXCL */
  shm_id = shmget(key, shm_size, IPC_CREAT|IPC_EXCL|00666);
  if (shm_id < 0) {
    EZDBG("<6>pid=[%d] %s(%d) try to create shm error.\n", getpid(), __func__, __LINE__);
    return EZCFG_RET_FAIL;
  }

  shm_addr = shmat(shm_id, NULL, 0);
  if ((void *) -1 == shm_addr) {
    EZDBG("<6>pid=[%d] %s(%d) shmat error with errno=[%d]\n", getpid(), __func__, __LINE__, errno);
    return EZCFG_RET_FAIL;
  }

  master->shm_id = shm_id;
  master->shm_size = shm_size;
  master->shm_addr = shm_addr;
  master->nvram_queue_length = nvram_queue_length;
  master->rc_queue_length = rc_queue_length;

  /* initialize shared memory */
  memset(shm_addr, 0, shm_size);

  ezcfg_shm_set_ezcfg_sem_id(shm_addr, master->sem_id);
  ezcfg_shm_set_ezcfg_shm_id(shm_addr, master->shm_id);
  ezcfg_shm_set_ezcfg_shm_size(shm_addr, master->shm_size);
  ezcfg_shm_set_ezcfg_nvram_queue_length(shm_addr, master->nvram_queue_length);
  ezcfg_shm_set_ezcfg_rc_queue_length(shm_addr, master->rc_queue_length);

  return EZCFG_RET_OK;
}

/**
 * master_new:
 *
 * Create ezcfg master.
 *
 * Returns: a new ezcfg master
 **/
static struct ezcfg_agent_master *master_new(struct ezcfg *ezcfg)
{
  struct ezcfg_agent_master *master = NULL;

  ASSERT(ezcfg != NULL);

  master = calloc(1, sizeof(struct ezcfg_agent_master));
  if (master == NULL) {
    err(ezcfg, "calloc ezcfg_agent_master fail: %m\n");
    return NULL;
  }

  /* set ezcfg library context */
  master->ezcfg = ezcfg;

  /* must first set sem_id to -1 */
  master->sem_id = -1;

  /* must first set shm_id to -1 */
  master->shm_id = -1;
  master->shm_size = 0;
  master->shm_addr = (void *)-1;

  /* must initialize semaphore first */
  if (master_init_sem(master) == false) {
    EZDBG("<6>pid=[%d] master_init_sem() failed\n", getpid());
    goto fail_exit;
  }

  /* initialize shared memory */
  if (master_init_shm(master) == false) {
    EZDBG("<6>pid=[%d] master_init_shm() failed\n", getpid());
    goto fail_exit;
  }

#if 0
  /* get nvram memory */
  master->nvram = ezcfg_nvram_new(ezcfg);
  if(master->nvram == NULL) {
    err(ezcfg, "alloc nvram fail: %m\n");
    goto fail_exit;
  }

  /* initialize nvram */
  ezcfg_nvram_fill_storage_info(master->nvram, ezcfg_common_get_config_file(ezcfg));
  if (ezcfg_nvram_initialize(master->nvram) == false) {
    err(ezcfg, "init nvram fail: %m\n");
    goto fail_exit;
  }
#endif

  /* initialize socket queue */
  master->sq_len = EZCFG_MASTER_SOCKET_QUEUE_LENGTH;
  master->queue = ezcfg_socket_calloc(ezcfg, master->sq_len);
  if(master->queue == NULL) {
    err(ezcfg, "calloc socket queue.");
    goto fail_exit;
  }

  /*
   * ignore SIGPIPE signal, so if client cancels the request, it
   * won't kill the whole process.
   */
  signal(SIGPIPE, SIG_IGN);

  /* initialize thread mutex */
  pthread_mutex_init(&(master->thread_mutex), NULL);
  pthread_rwlock_init(&(master->thread_rwlock), NULL);
  pthread_cond_init(&(master->thread_sync_cond), NULL);
  pthread_mutex_init(&(master->ls_mutex), NULL);
  pthread_cond_init(&(master->sq_empty_cond), NULL);
  pthread_cond_init(&(master->sq_full_cond), NULL);

  /* Successfully create master thread */
  return master;

fail_exit:
  master_delete(master);
  return NULL;
}

/**
 * master_add_socket:
 * @master: ezcfg master context
 * @socket_path: unix socket path
 *
 * Add a specified socket to the master thread. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * caller must lock the ls_mutex
 *
 * Returns: socket, or NULL, in case of an error
 **/
static struct ezcfg_socket *master_add_socket(struct ezcfg_agent_master *master, int family, int type, int proto, const char *socket_path)
{
  struct ezcfg_socket *listener = NULL;
  struct ezcfg *ezcfg = NULL;

  ASSERT(master != NULL);
  ezcfg = master->ezcfg;

  ASSERT(socket_path != NULL);

  /* initialize unix domain socket */
  listener = ezcfg_socket_new(ezcfg, EZCFG_META_NVRAM_PREFIX);
  if (listener == NULL) {
    err(ezcfg, "init socket fail: %m\n");
    return NULL;
  }

  if ((family == AF_LOCAL) &&
      (socket_path[0] != '@')) {
    ezcfg_socket_set_need_unlink(listener, true);
  }

  if (ezcfg_socket_list_insert(&(master->listening_sockets), listener) < 0) {
    err(ezcfg, "insert listener socket fail: %m\n");
    ezcfg_socket_delete(listener);
    listener = NULL;
  }

  return listener;
}

/**
 * master_new_from_socket:
 * @ezcfg: ezcfg library context
 * @socket_path: unix socket path
 *
 * Create new ezcfg master and connect to a specified socket. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * Returns: a new ezcfg master, or #NULL, in case of an error
 **/
static struct ezcfg_agent_master *master_new_from_socket(struct ezcfg *ezcfg, const char *socket_path)
{
  struct ezcfg_agent_master *master = NULL;
  struct ezcfg_socket *sp = NULL;

  ASSERT(ezcfg != NULL);
  ASSERT(socket_path != NULL);

  master = master_new(ezcfg);
  if (master == NULL) {
    err(ezcfg, "new master fail: %m\n");
    return NULL;
  }

  sp = master_add_socket(master, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_NV_JSON_HTTP, socket_path);
  if (sp == NULL) {
    err(ezcfg, "add socket [%s] fail: %m\n", socket_path);
    goto fail_exit;
  }

  if (ezcfg_socket_enable_receiving(sp) < 0) {
    err(ezcfg, "enable socket [%s] receiving fail: %m\n", socket_path);
    ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
    goto fail_exit;
  }

  if (ezcfg_socket_enable_listening(sp, master->sq_len) < 0) {
    err(ezcfg, "enable socket [%s] listening fail: %m\n", socket_path);
    ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
    goto fail_exit;
  }

  ezcfg_socket_set_close_on_exec(sp);

  return master;

fail_exit:
  /* first clean up all resources in master */
  master_finish(master);
  /* don't delete sp, master_delete will do it! */
  master_delete(master);
  return NULL;
}

static void add_to_set(int fd, fd_set *set, int *max_fd)
{
  FD_SET(fd, set);
  if (fd > *max_fd) {
    *max_fd = (int) fd;
  }
}

/* Master thread adds accepted socket to a queue */
static void put_socket(struct ezcfg_agent_master *master, const struct ezcfg_socket *sp)
{
  struct ezcfg *ezcfg;
  int stacksize;
	
  ASSERT(master != NULL);
  ASSERT(sp != NULL);

  ezcfg = master->ezcfg;
  stacksize = 0;

  pthread_mutex_lock(&(master->thread_mutex));

  /* If the queue is full, wait */
  while (master->sq_head - master->sq_tail >= master->sq_len) {
    pthread_cond_wait(&(master->sq_full_cond), &(master->thread_mutex));
  }
  ASSERT(master->sq_head - master->sq_tail < master->sq_len);

  /* Copy socket to the queue and increment head */
  ezcfg_socket_queue_set_socket(master->queue, master->sq_head % master->sq_len, sp);
  master->sq_head++;

  /* If there are no idle threads, start one */
  if (master->num_idle == 0 && master->num_threads < master->threads_max) {
    struct ezcfg_agent_worker *worker;

    worker = ezcfg_agent_worker_new(master);
    if (worker != NULL) {
      if (ezcfg_thread_start(ezcfg, stacksize, ezcfg_agent_worker_get_p_thread_id(worker), (ezcfg_thread_func_t) ezcfg_agent_worker_routine, worker) != 0) {
        err(ezcfg, "Cannot start thread: %m\n");
      } else {
        master->num_threads++;
        /* add to worker list */
        ezcfg_agent_worker_set_next(worker, master->workers);
        master->workers = worker;
      }
    } else {
      err(ezcfg, "Cannot prepare worker thread: %m\n");
    }
  }

  pthread_cond_signal(&(master->sq_empty_cond));
  pthread_mutex_unlock(&(master->thread_mutex));
}

static bool accept_new_connection(struct ezcfg_agent_master *master,
                                  struct ezcfg_socket *listener)
{
  struct ezcfg *ezcfg;
  struct ezcfg_socket *accepted;
  bool allowed;

  ASSERT(master != NULL);
  ASSERT(listener != NULL);

  ezcfg = master->ezcfg;

  accepted = ezcfg_socket_new_accepted_socket(listener);
  if (accepted == NULL) {
    err(ezcfg, "new accepted socket fail.\n");
    return false;
  }

  allowed = true;

  if (allowed == true) {
    put_socket(master, accepted);
    /*FIXME: don't ezcfg_socket_delete(), it has been copy to queue */
    free(accepted);
  }
  else {
    ezcfg_socket_delete(accepted);
  }

  return true;
}

pthread_t *ezcfg_agent_master_get_p_thread_id(struct ezcfg_agent_master *master)
{
  return &(master->thread_id);
}

void ezcfg_agent_master_routine(struct ezcfg_agent_master *master) 
{
  fd_set read_set;
  struct ezcfg *ezcfg;
  struct ezcfg_socket *sp;
  //struct timeval tv;
  sigset_t orig_sigmask;
  int max_fd;
  int retval;

  ASSERT(master != NULL);

  ezcfg = master->ezcfg;

  while (master->stop_flag == 0) {
    FD_ZERO(&read_set);
    max_fd = -1;

    /* Add listening sockets to the read set */
    /* lock mutex before handling listening_sockets */
    pthread_mutex_lock(&(master->ls_mutex));

    for (sp = master->listening_sockets; sp != NULL; sp = ezcfg_socket_list_next(&sp)) {
      add_to_set(ezcfg_socket_get_sock(sp), &read_set, &max_fd);
    }

    /* unlock mutex after handling listening_sockets */
    pthread_mutex_unlock(&(master->ls_mutex));

#if 0
    /* wait up to AGENT_MASTER_WAIT_TIME seconds. */
    tv.tv_sec = AGENT_MASTER_WAIT_TIME;
    tv.tv_usec = 0;

    retval = pselect(max_fd + 1, &read_set, NULL, NULL, &tv, &orig_sigmask);
#else
    /* wait till there's one or more of the file descriptors become "ready" */
    retval = pselect(max_fd + 1, &read_set, NULL, NULL, NULL, &orig_sigmask);
#endif

    if (retval == -1) {
      perror("select()");
      err(ezcfg, "select() %m\n");
    }
    else if (retval == 0) {
      /* no data arrived, do nothing */
      do {} while(0);
    }
    else {
      /* lock mutex before handling listening_sockets */
      pthread_mutex_lock(&(master->ls_mutex));

      for (sp = master->listening_sockets;
           sp != NULL;
           sp = ezcfg_socket_list_next(&sp)) {
        if (FD_ISSET(ezcfg_socket_get_sock(sp), &read_set)) {
          if (accept_new_connection(master, sp) == false) {
            /* re-enable the socket */
            err(ezcfg, "accept_new_connection() failed\n");

            if (ezcfg_socket_enable_again(sp) < 0) {
              err(ezcfg, "ezcfg_socket_enable_again() failed\n");
              ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
            }
          }
        }
      }

      /* unlock mutex after handling listening_sockets */
      pthread_mutex_unlock(&(master->ls_mutex));
    }
  }

  /* Stop signal received: somebody called ezcfg_agent_master_stop. Quit. */
  master_finish(master);
}

struct ezcfg_agent_master *ezcfg_agent_master_start(struct ezcfg *ezcfg, struct ezcfg_agent_core *core)
{
  struct ezcfg_agent_master *master = NULL;
  int stacksize;
  //struct ezcfg_socket * sp;
  //char *p;
  char buf[EZCFG_PATH_MAX];
  int ret = EZCFG_RET_FAIL;


  ASSERT(ezcfg != NULL);
  ASSERT(core != NULL);

  EZDBG("%s(%d)\n", __func__, __LINE__);
  /* increase ezcfg library context reference */
  if (ezcfg_inc_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("%s(%d) <6>pid=[%d] ezcfg_inc_ref() failed\n", __func__, __LINE__, getpid());
    return NULL;
  }

  /* stacksize = sizeof(struct ezcfg_agent_master) * 2; */
  stacksize = 0;

  /* There must be a nvram socket */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  ret = ezcfg_common_get_meta_nvram(ezcfg, NVRAM_NAME(META, SOCK_NVRAM_PATH), buf, sizeof(buf));
  if (ret != EZCFG_RET_OK) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not get nvram socket path");
    /* decrease ezcfg library context reference */
    if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
      EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
    }
    return NULL;
  }

  EZDBG("%s(%d)\n", __func__, __LINE__);
  master = master_new_from_socket(ezcfg, buf);
  if (master == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not initialize control socket");
    /* decrease ezcfg library context reference */
    if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
      EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
    }
    return NULL;
  }

  /* link agent core state with act executor */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  master->core = core;

  /* lock mutex before handling listening_sockets */
  pthread_mutex_lock(&(master->ls_mutex));
  //ezcfg_agent_master_load_socket_conf(master);
  /* unlock mutex after handling listening_sockets */
  pthread_mutex_unlock(&(master->ls_mutex));

  /* Start master (listening) thread */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (ezcfg_thread_start(ezcfg, stacksize, &(master->thread_id), (ezcfg_thread_func_t) ezcfg_agent_master_routine, master) != 0) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    master_finish(master);
    ASSERT(master->num_threads == 0);
    master_delete(master);
    master = NULL;
    /* decrease ezcfg library context reference */
    if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
      EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
    }
  }
  EZDBG("%s(%d)\n", __func__, __LINE__);
  return master;
}

void ezcfg_agent_master_stop(struct ezcfg_agent_master *master)
{
  struct ezcfg *ezcfg = NULL;
  struct sembuf res[EZCFG_SEM_NUMBER];
  bool sem_required = true;
  int i;

  ASSERT (master != NULL);

  ezcfg = master->ezcfg;

  /* now require available resource */
  for (i=0; i<EZCFG_SEM_NUMBER; i++) {
    res[i].sem_num = i;
    res[i].sem_op = -1;
    res[i].sem_flg = 0;
  }

  if (semop(master->sem_id, res, EZCFG_SEM_NUMBER) == -1) {
    EZDBG("<6>pid=[%d] semop require_res error\n", getpid());
    sem_required = false;
  }

  master->stop_flag = 1;

  /* Wait until master_finish() stops */
  while (master->stop_flag != 2)
    sleep(1);

  /* now release resource */
  if (sem_required == true) {
    for (i=0; i<EZCFG_SEM_NUMBER; i++) {
      res[i].sem_num = i;
      res[i].sem_op = 1;
      res[i].sem_flg = 0;
    }

    if (semop(master->sem_id, res, EZCFG_SEM_NUMBER) == -1) {
      EZDBG("<6>pid=[%d] semop release_res error\n", getpid());
    }
  }

  ASSERT(master->num_threads == 0);
  master_delete(master);

  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("%s(%d) <6>pid=[%d] ezcfg_dec_ref() failed\n", __func__, __LINE__, getpid());
  }
}

void ezcfg_agent_master_reload(struct ezcfg_agent_master *master)
{
  //struct ezcfg *ezcfg;
  struct sembuf res[EZCFG_SEM_NUMBER];
  int i;

  if (master == NULL)
    return;

  /* now require available resource */
  for (i=0; i<EZCFG_SEM_NUMBER; i++) {
    res[i].sem_num = i;
    res[i].sem_op = -1;
    res[i].sem_flg = 0;
  }

  if (semop(master->sem_id, res, EZCFG_SEM_NUMBER) == -1) {
    EZDBG("<6>pid=[%d] semop require_res error\n", getpid());
    return;
  }

  //ezcfg = master->ezcfg;

  /* lock thread mutex */
  pthread_mutex_lock(&(master->thread_mutex));

  /* lock listening sockets mutex */
  pthread_mutex_lock(&(master->ls_mutex));

  /* wait all wokers stop first */
  while (master->num_threads > 0)
    pthread_cond_wait(&(master->thread_sync_cond), &(master->thread_mutex));

  /* unlock listening sockets mutex */
  pthread_mutex_unlock(&(master->ls_mutex));

  /* unlock thread mutex */
  pthread_mutex_unlock(&(master->thread_mutex));

  /* now release resource */
  for (i=0; i<EZCFG_SEM_NUMBER; i++) {
    res[i].sem_num = i;
    res[i].sem_op = 1;
    res[i].sem_flg = 0;
  }

  if (semop(master->sem_id, res, EZCFG_SEM_NUMBER) == -1) {
    EZDBG("<6>pid=[%d] semop release_res error\n", getpid());
    return;
  }
}

void ezcfg_agent_master_set_threads_max(struct ezcfg_agent_master *master, int threads_max)
{
  if (master == NULL)
    return;
  master->threads_max = threads_max;
}

struct ezcfg *ezcfg_agent_master_get_ezcfg(struct ezcfg_agent_master *master)
{
  return master->ezcfg;
}

struct ezcfg_nvram *ezcfg_agent_master_get_nvram(struct ezcfg_agent_master *master)
{
  return ezcfg_agent_core_get_nvram(master->core);
}

bool ezcfg_agent_master_is_stop(struct ezcfg_agent_master *master)
{
  ASSERT(master != NULL);

  return (master->stop_flag != 0);
}

bool ezcfg_agent_master_get_socket(struct ezcfg_agent_master *master, struct ezcfg_socket *sp, int wait_time)
{
  struct timespec ts;

  ASSERT(master != NULL);
  ASSERT(wait_time >= 0);

  pthread_mutex_lock(&(master->thread_mutex));
  /* If the queue is empty, wait. We're idle at this point. */
  master->num_idle++;
  while (master->sq_head == master->sq_tail) {
    ts.tv_nsec = 0;
    ts.tv_sec = time(NULL) + wait_time;
    if (pthread_cond_timedwait(&(master->sq_empty_cond), &(master->thread_mutex), &ts) != 0) {
      /* Timeout! release the mutex and return */
      pthread_mutex_unlock(&(master->thread_mutex));
      return false;
    }
  }
  ASSERT(master->sq_head > master->sq_tail);

  /* We're going busy now: got a socket to process! */
  master->num_idle--;

  /* Copy socket from the queue and increment tail */
  ezcfg_socket_queue_get_socket(master->queue, master->sq_tail % master->sq_len, sp);
  master->sq_tail++;

  /* Wrap pointers if needed */
  while (master->sq_tail >= master->sq_len) {
    master->sq_tail -= master->sq_len;
    master->sq_head -= master->sq_len;
  }
  pthread_cond_signal(&(master->sq_full_cond));
  pthread_mutex_unlock(&(master->thread_mutex));

  return true;
}

void ezcfg_agent_master_stop_worker(struct ezcfg_agent_master *master, struct ezcfg_agent_worker *worker)
{
  struct ezcfg_agent_worker *cur, *prev;

  ASSERT(master != NULL);
  ASSERT(worker != NULL);

  pthread_mutex_lock(&(master->thread_mutex));

  /* remove worker from worker list */
  cur = master->workers;
  if (cur == worker) {
    master->workers = ezcfg_agent_worker_get_next(cur);
  }

  prev = cur;
  cur = ezcfg_agent_worker_get_next(cur);
  while (cur != NULL) {
    if (cur == worker) {
      cur = ezcfg_agent_worker_get_next(worker);
      ezcfg_agent_worker_set_next(prev, cur);
      break;
    }
    prev = cur;
    cur = ezcfg_agent_worker_get_next(cur);
  }

  /* clean worker resource */
  ezcfg_agent_worker_delete(worker);

  master->num_threads--;
  master->num_idle--;
  pthread_cond_signal(&(master->thread_sync_cond));
  ASSERT(master->num_threads >= 0);

  pthread_mutex_unlock(&(master->thread_mutex));
}

int ezcfg_agent_master_get_shm_id(struct ezcfg_agent_master *master)
{
  ASSERT(master != NULL);

  return master->shm_id;
}

int ezcfg_agent_master_get_sq_len(struct ezcfg_agent_master *master)
{
  ASSERT(master != NULL);

  return master->sq_len;
}

struct ezcfg_socket *ezcfg_agent_master_get_listening_sockets(struct ezcfg_agent_master *master)
{
  ASSERT(master != NULL);

  return master->listening_sockets;
}

struct ezcfg_socket **ezcfg_agent_master_get_p_listening_sockets(struct ezcfg_agent_master *master)
{
  ASSERT(master != NULL);

  return &(master->listening_sockets);
}
