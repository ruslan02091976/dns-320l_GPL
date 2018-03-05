/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996,2007 Oracle.  All rights reserved.
 *
 * $Id: env_open.c,v 1.1.1.1 2009/04/20 07:15:17 jack Exp $
 */

#include "db_config.h"

#include "db_int.h"
#include "dbinc/crypto.h"
#include "dbinc/db_page.h"
#include "dbinc/btree.h"
#include "dbinc/lock.h"
#include "dbinc/log.h"
#include "dbinc/mp.h"
#include "dbinc/txn.h"

static int __env_refresh __P((DB_ENV *, u_int32_t, int));
static int __file_handle_cleanup __P((DB_ENV *));

/*
 * db_version --
 *	Return version information.
 *
 * EXTERN: char *db_version __P((int *, int *, int *));
 */
char *
db_version(majverp, minverp, patchp)
	int *majverp, *minverp, *patchp;
{
	if (majverp != NULL)
		*majverp = DB_VERSION_MAJOR;
	if (minverp != NULL)
		*minverp = DB_VERSION_MINOR;
	if (patchp != NULL)
		*patchp = DB_VERSION_PATCH;
	return ((char *)DB_VERSION_STRING);
}

/*
 * __env_open_pp --
 *	DB_ENV->open pre/post processing.
 *
 * PUBLIC: int __env_open_pp __P((DB_ENV *, const char *, u_int32_t, int));
 */
int
__env_open_pp(dbenv, db_home, flags, mode)
	DB_ENV *dbenv;
	const char *db_home;
	u_int32_t flags;
	int mode;
{
	int ret;

#undef	OKFLAGS
#define	OKFLAGS								\
	(DB_CREATE | DB_INIT_CDB | DB_INIT_LOCK | DB_INIT_LOG |		\
	DB_INIT_MPOOL | DB_INIT_REP | DB_INIT_TXN | DB_LOCKDOWN |	\
	DB_PRIVATE | DB_RECOVER | DB_RECOVER_FATAL | DB_REGISTER |	\
	DB_SYSTEM_MEM | DB_THREAD | DB_USE_ENVIRON | DB_USE_ENVIRON_ROOT)
#undef	OKFLAGS_CDB
#define	OKFLAGS_CDB							\
	(DB_CREATE | DB_INIT_CDB | DB_INIT_MPOOL | DB_LOCKDOWN |	\
	DB_PRIVATE | DB_SYSTEM_MEM | DB_THREAD |			\
	DB_USE_ENVIRON | DB_USE_ENVIRON_ROOT)

	if ((ret = __db_fchk(dbenv, "DB_ENV->open", flags, OKFLAGS)) != 0)
		return (ret);
	if ((ret = __db_fcchk(
	    dbenv, "DB_ENV->open", flags, DB_INIT_CDB, ~OKFLAGS_CDB)) != 0)
		return (ret);
	if (LF_ISSET(DB_REGISTER)) {
		if (!__os_support_db_register()) {
			__db_errx(dbenv,
	     "Berkeley DB library does not support DB_REGISTER on this system");
			return (EINVAL);
		}
		if ((ret = __db_fcchk(dbenv, "DB_ENV->open", flags,
		    DB_PRIVATE, DB_REGISTER | DB_SYSTEM_MEM)) != 0)
			return (ret);
		if (!LF_ISSET(DB_INIT_TXN)) {
			__db_errx(
			    dbenv, "registration requires transaction support");
			return (EINVAL);
		}
	}
	if (LF_ISSET(DB_INIT_REP)) {
		if (!__os_support_replication()) {
			__db_errx(dbenv,
	     "Berkeley DB library does not support replication on this system");
			return (EINVAL);
		}
		if (!LF_ISSET(DB_INIT_LOCK)) {
			__db_errx(dbenv,
			    "replication requires locking support");
			return (EINVAL);
		}
		if (!LF_ISSET(DB_INIT_TXN)) {
			__db_errx(
			    dbenv, "replication requires transaction support");
			return (EINVAL);
		}
	}
	if (LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL)) {
		if ((ret = __db_fcchk(dbenv,
		    "DB_ENV->open", flags, DB_RECOVER, DB_RECOVER_FATAL)) != 0)
			return (ret);
		if ((ret = __db_fcchk(dbenv,
		    "DB_ENV->open", flags, DB_REGISTER, DB_RECOVER_FATAL)) != 0)
			return (ret);
		if (!LF_ISSET(DB_CREATE)) {
			__db_errx(dbenv, "recovery requires the create flag");
			return (EINVAL);
		}
		if (!LF_ISSET(DB_INIT_TXN)) {
			__db_errx(
			    dbenv, "recovery requires transaction support");
			return (EINVAL);
		}
	}

#ifdef HAVE_MUTEX_THREAD_ONLY
	/*
	 * Currently we support one kind of mutex that is intra-process only,
	 * POSIX 1003.1 pthreads, because a variety of systems don't support
	 * the full pthreads API, and our only alternative is test-and-set.
	 */
	if (!LF_ISSET(DB_PRIVATE)) {
		__db_errx(dbenv,
	 "Berkeley DB library configured to support only private environments");
		return (EINVAL);
	}
#endif

#ifdef HAVE_MUTEX_FCNTL
	/*
	 * !!!
	 * We need a file descriptor for fcntl(2) locking.  We use the file
	 * handle from the REGENV file for this purpose.
	 *
	 * Since we may be using shared memory regions, e.g., shmget(2), and
	 * not a mapped-in regular file, the backing file may be only a few
	 * bytes in length.  So, this depends on the ability to call fcntl to
	 * lock file offsets much larger than the actual physical file.  I
	 * think that's safe -- besides, very few systems actually need this
	 * kind of support, SunOS is the only one still in wide use of which
	 * I'm aware.
	 *
	 * The error case is if an application lacks spinlocks and wants to be
	 * threaded.  That doesn't work because fcntl will lock the underlying
	 * process, including all its threads.
	 */
	if (F_ISSET(dbenv, DB_ENV_THREAD)) {
		__db_errx(dbenv,
	    "architecture lacks fast mutexes: applications cannot be threaded");
		return (EINVAL);
	}
#endif

	return (__env_open(dbenv, db_home, flags, mode));
}

/*
 * __env_open --
 *	DB_ENV->open.
 *
 * PUBLIC: int __env_open __P((DB_ENV *, const char *, u_int32_t, int));
 */
int
__env_open(dbenv, db_home, flags, mode)
	DB_ENV *dbenv;
	const char *db_home;
	u_int32_t flags;
	int mode;
{
	DB_THREAD_INFO *ip;
	REGINFO *infop;
	u_int32_t init_flags, orig_flags;
	int create_ok, register_recovery, rep_check, ret, t_ret;

	ip = NULL;
	register_recovery = rep_check = 0;

	/* Initial configuration. */
	if ((ret = __env_config(dbenv, db_home, flags, mode)) != 0)
		return (ret);

	/*
	 * Save the DB_ENV handle's configuration flags as set by user-called
	 * configuration methods and the environment directory's DB_CONFIG
	 * file.  If we use this DB_ENV structure to recover the existing
	 * environment or to remove an environment we created after failure,
	 * we'll restore the DB_ENV flags to these values.
	 */
	orig_flags = dbenv->flags;

	/*
	 * If we're going to register with the environment, that's the first
	 * thing we do.
	 */
	if (LF_ISSET(DB_REGISTER)) {
		if ((ret = __envreg_register(dbenv, &register_recovery)) != 0)
			goto err;
		if (register_recovery) {
			if (!LF_ISSET(DB_RECOVER)) {
				__db_errx(dbenv,
	    "The DB_RECOVER flag was not specified, and recovery is needed");
				ret = DB_RUNRECOVERY;
				goto err;
			}
		} else
			LF_CLR(DB_RECOVER);
	}

	/*
	 * If we're doing recovery, destroy the environment so that we create
	 * all the regions from scratch.  The major concern I have is if the
	 * application stomps the environment with a rogue pointer.  We have
	 * no way of detecting that, and we could be forced into a situation
	 * where we start up and then crash, repeatedly.
	 *
	 * We do not check any flags like DB_PRIVATE before calling remove.
	 * We don't care if the current environment was private or not, we
	 * want to remove files left over for any reason, from any session.
	 */
	if (LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL))
#ifdef HAVE_REPLICATION
		if ((ret = __rep_reset_init(dbenv)) != 0 ||
		    (ret = __env_remove_env(dbenv)) != 0 ||
#else
		if ((ret = __env_remove_env(dbenv)) != 0 ||
#endif
		    (ret = __env_refresh(dbenv, orig_flags, 0)) != 0)
			goto err;

	/* Convert the DB_ENV->open flags to internal flags. */
	create_ok = LF_ISSET(DB_CREATE) ? 1 : 0;
	if (LF_ISSET(DB_LOCKDOWN))
		F_SET(dbenv, DB_ENV_LOCKDOWN);
	if (LF_ISSET(DB_PRIVATE))
		F_SET(dbenv, DB_ENV_PRIVATE);
	if (LF_ISSET(DB_RECOVER_FATAL))
		F_SET(dbenv, DB_ENV_RECOVER_FATAL);
	if (LF_ISSET(DB_SYSTEM_MEM))
		F_SET(dbenv, DB_ENV_SYSTEM_MEM);
	if (LF_ISSET(DB_THREAD))
		F_SET(dbenv, DB_ENV_THREAD);

	/*
	 * Flags saved in the init_flags field of the environment, representing
	 * flags to DB_ENV->set_flags and DB_ENV->open that need to be set.
	 */
#define	DB_INITENV_CDB		0x0001	/* DB_INIT_CDB */
#define	DB_INITENV_CDB_ALLDB	0x0002	/* DB_INIT_CDB_ALLDB */
#define	DB_INITENV_LOCK		0x0004	/* DB_INIT_LOCK */
#define	DB_INITENV_LOG		0x0008	/* DB_INIT_LOG */
#define	DB_INITENV_MPOOL	0x0010	/* DB_INIT_MPOOL */
#define	DB_INITENV_REP		0x0020	/* DB_INIT_REP */
#define	DB_INITENV_TXN		0x0040	/* DB_INIT_TXN */

	/*
	 * Create/join the environment.  We pass in the flags of interest to
	 * a thread subsequently joining an environment we create.  If we're
	 * not the ones to create the environment, our flags will be updated
	 * to match the existing environment.
	 */
	init_flags = 0;
	if (LF_ISSET(DB_INIT_CDB))
		FLD_SET(init_flags, DB_INITENV_CDB);
	if (F_ISSET(dbenv, DB_ENV_CDB_ALLDB))
		FLD_SET(init_flags, DB_INITENV_CDB_ALLDB);
	if (LF_ISSET(DB_INIT_LOCK))
		FLD_SET(init_flags, DB_INITENV_LOCK);
	if (LF_ISSET(DB_INIT_LOG))
		FLD_SET(init_flags, DB_INITENV_LOG);
	if (LF_ISSET(DB_INIT_MPOOL))
		FLD_SET(init_flags, DB_INITENV_MPOOL);
	if (LF_ISSET(DB_INIT_REP))
		FLD_SET(init_flags, DB_INITENV_REP);
	if (LF_ISSET(DB_INIT_TXN))
		FLD_SET(init_flags, DB_INITENV_TXN);
	if ((ret = __env_attach(dbenv, &init_flags, create_ok, 1)) != 0)
		goto err;

	/*
	 * __env_attach will return the saved init_flags field, which contains
	 * the DB_INIT_* flags used when the environment was created.
	 *
	 * We may be joining an environment -- reset our flags to match the
	 * ones in the environment.
	 */
	if (FLD_ISSET(init_flags, DB_INITENV_CDB))
		LF_SET(DB_INIT_CDB);
	if (FLD_ISSET(init_flags, DB_INITENV_LOCK))
		LF_SET(DB_INIT_LOCK);
	if (FLD_ISSET(init_flags, DB_INITENV_LOG))
		LF_SET(DB_INIT_LOG);
	if (FLD_ISSET(init_flags, DB_INITENV_MPOOL))
		LF_SET(DB_INIT_MPOOL);
	if (FLD_ISSET(init_flags, DB_INITENV_REP))
		LF_SET(DB_INIT_REP);
	if (FLD_ISSET(init_flags, DB_INITENV_TXN))
		LF_SET(DB_INIT_TXN);
	if (FLD_ISSET(init_flags, DB_INITENV_CDB_ALLDB) &&
	    (ret = __env_set_flags(dbenv, DB_CDB_ALLDB, 1)) != 0)
		goto err;

	/*
	 * Save the flags matching the database environment: we'll replace
	 * the argument flags with the flags corresponding to the existing,
	 * underlying set of subsystems.
	 */
	dbenv->open_flags = flags;

	/* Initialize for CDB product. */
	if (LF_ISSET(DB_INIT_CDB)) {
		LF_SET(DB_INIT_LOCK);
		F_SET(dbenv, DB_ENV_CDB);
	}

	/*
	 * The DB_ENV structure has now been initialized.  Turn off further
	 * use of the DB_ENV structure and most initialization methods, we're
	 * about to act on the values we currently have.
	 */
	F_SET(dbenv, DB_ENV_OPEN_CALLED);

	/*
	 * Initialize thread tracking and enter the API.
	 */
	infop = dbenv->reginfo;
	if ((ret = __env_thread_init(
	    dbenv, F_ISSET(infop, REGION_CREATE) ? 1 : 0)) != 0)
		goto err;

	ENV_ENTER(dbenv, ip);

	/*
	 * Initialize the subsystems.
	 */
#ifdef HAVE_MUTEX_SUPPORT
	/*
	 * Initialize the mutex regions first.  There's no ordering requirement,
	 * but it's simpler to get this in place so we don't have to keep track
	 * of mutexes for later allocation, once the mutex region is created we
	 * can go ahead and do the allocation for real.
	 */
	if ((ret = __mutex_open(dbenv, create_ok)) != 0)
		goto err;
#endif
	/*
	 * We can now acquire/create mutexes: increment the region's reference
	 * count.
	 */
	if ((ret = __env_ref_increment(dbenv)) != 0)
		goto err;

	/*
	 * Initialize the handle's mutex.
	 */
	if ((ret = __mutex_alloc(dbenv,
	    MTX_ENV_HANDLE, DB_MUTEX_PROCESS_ONLY, &dbenv->mtx_env)) != 0)
		goto err;

	/*
	 * Initialize the replication area next, so that we can lock out this
	 * call if we're currently running recovery for replication.
	 */
	if (LF_ISSET(DB_INIT_REP) && (ret = __rep_open(dbenv)) != 0)
		goto err;

	rep_check = IS_ENV_REPLICATED(dbenv) ? 1 : 0;
	if (rep_check && (ret = __env_rep_enter(dbenv, 0)) != 0)
		goto err;

	if (LF_ISSET(DB_INIT_MPOOL)) {
		if ((ret = __memp_open(dbenv, create_ok)) != 0)
			goto err;

		/*
		 * BDB does do cache I/O during recovery and when starting up
		 * replication.  If creating a new environment, then suppress
		 * any application max-write configuration.
		 */
		if (create_ok)
			(void)__memp_set_config(
			    dbenv, DB_MEMP_SUPPRESS_WRITE, 1);

		/*
		 * Initialize the DB list and its mutex.  If the mpool is
		 * not initialized, we can't ever open a DB handle, which
		 * is why this code lives here.
		 */
		TAILQ_INIT(&dbenv->dblist);
		if ((ret = __mutex_alloc(dbenv, MTX_ENV_DBLIST,
		    DB_MUTEX_PROCESS_ONLY, &dbenv->mtx_dblist)) != 0)
			goto err;

		/* Register DB's pgin/pgout functions.  */
		if ((ret = __memp_register(
		    dbenv, DB_FTYPE_SET, __db_pgin, __db_pgout)) != 0)
			goto err;
	}

	/*
	 * Initialize the ciphering area prior to any running of recovery so
	 * that we can initialize the keys, etc. before recovery, including
	 * the MT mutex.
	 *
	 * !!!
	 * This must be after the mpool init, but before the log initialization
	 * because log_open may attempt to run log_recover during its open.
	 */
	if (LF_ISSET(DB_INIT_MPOOL | DB_INIT_LOG | DB_INIT_TXN) &&
	    (ret = __crypto_region_init(dbenv)) != 0)
		goto err;
	if ((ret = __mutex_alloc(dbenv, MTX_TWISTER,
	    DB_MUTEX_PROCESS_ONLY, &dbenv->mtx_mt)) != 0)
		goto err;

	/*
	 * Transactions imply logging but do not imply locking.  While almost
	 * all applications want both locking and logging, it would not be
	 * unreasonable for a single threaded process to want transactions for
	 * atomicity guarantees, but not necessarily need concurrency.
	 */
	if (LF_ISSET(DB_INIT_LOG | DB_INIT_TXN))
		if ((ret = __log_open(dbenv, create_ok)) != 0)
			goto err;
	if (LF_ISSET(DB_INIT_LOCK))
		if ((ret = __lock_open(dbenv, create_ok)) != 0)
			goto err;

	if (LF_ISSET(DB_INIT_TXN)) {
		if ((ret = __txn_open(dbenv, create_ok)) != 0)
			goto err;

		/*
		 * If the application is running with transactions, initialize
		 * the function tables.
		 */
		if ((ret = __env_init_rec(dbenv, DB_LOGVERSION)) != 0)
			goto err;
	}

	/* Perform recovery for any previous run. */
	if (LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL) &&
	    (ret = __db_apprec(dbenv, NULL, NULL, 1,
	    LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL))) != 0)
		goto err;

	/*
	 * If we've created the regions, are running with transactions, and did
	 * not just run recovery, we need to log the fact that the transaction
	 * IDs got reset.
	 *
	 * If we ran recovery, there may be prepared-but-not-yet-committed
	 * transactions that need to be resolved.  Recovery resets the minimum
	 * transaction ID and logs the reset if that's appropriate, so we
	 * don't need to do anything here in the recover case.
	 */
	if (TXN_ON(dbenv) &&
	    !F_ISSET(dbenv, DB_ENV_LOG_INMEMORY) &&
	    F_ISSET(infop, REGION_CREATE) &&
	    !LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL) &&
	    (ret = __txn_reset(dbenv)) != 0)
		goto err;

	/* The database environment is ready for business. */
	if ((ret = __env_turn_on(dbenv)) != 0)
		goto err;

	if (rep_check)
		ret = __env_db_rep_exit(dbenv);

	/* Turn any application-specific max-write configuration back on. */
	if (LF_ISSET(DB_INIT_MPOOL))
		(void)__memp_set_config(dbenv, DB_MEMP_SUPPRESS_WRITE, 0);

err:	if (ret == 0)
		ENV_LEAVE(dbenv, ip);
	else {
		/*
		 * If we fail after creating the regions, panic and remove them.
		 *
		 * !!!
		 * No need to call __env_db_rep_exit, that work is done by the
		 * calls to __env_refresh.
		 */
		infop = dbenv->reginfo;
		if (infop != NULL && F_ISSET(infop, REGION_CREATE)) {
			ret = __db_panic(dbenv, ret);

			/* Refresh the DB_ENV so can use it to call remove. */
			(void)__env_refresh(dbenv, orig_flags, rep_check);
			(void)__env_remove_env(dbenv);
			(void)__env_refresh(dbenv, orig_flags, 0);
		} else
			(void)__env_refresh(dbenv, orig_flags, rep_check);
	}

	if (register_recovery) {
		/*
		 * If recovery succeeded, release our exclusive lock, other
		 * processes can now proceed.
		 *
		 * If recovery failed, unregister now and let another process
		 * clean up.
		 */
		if (ret == 0 && (t_ret = __envreg_xunlock(dbenv)) != 0)
			ret = t_ret;
		if (ret != 0)
			(void)__envreg_unregister(dbenv, 1);
	}

	return (ret);
}

/*
 * __env_remove --
 *	DB_ENV->remove.
 *
 * PUBLIC: int __env_remove __P((DB_ENV *, const char *, u_int32_t));
 */
int
__env_remove(dbenv, db_home, flags)
	DB_ENV *dbenv;
	const char *db_home;
	u_int32_t flags;
{
	int ret, t_ret;

#undef	OKFLAGS
#define	OKFLAGS								\
	(DB_FORCE | DB_USE_ENVIRON | DB_USE_ENVIRON_ROOT)

	/* Validate arguments. */
	if ((ret = __db_fchk(dbenv, "DB_ENV->remove", flags, OKFLAGS)) != 0)
		return (ret);

	ENV_ILLEGAL_AFTER_OPEN(dbenv, "DB_ENV->remove");

	if ((ret = __env_config(dbenv, db_home, flags, 0)) != 0)
		return (ret);

	/*
	 * Turn the environment off -- if the environment is corrupted, this
	 * could fail.  Ignore any error if we're forcing the question.
	 */
	if ((ret = __env_turn_off(dbenv, flags)) != 0 && !LF_ISSET(DB_FORCE))
		return (ret);

	ret = __env_remove_env(dbenv);

	if ((t_ret = __env_close(dbenv, 0)) != 0 && ret == 0)
		ret = t_ret;

	return (ret);
}

/*
 * __env_config --
 *	Argument-based initialization.
 *
 * PUBLIC: int __env_config __P((DB_ENV *, const char *, u_int32_t, int));
 */
int
__env_config(dbenv, db_home, flags, mode)
	DB_ENV *dbenv;
	const char *db_home;
	u_int32_t flags;
	int mode;
{
	int ret;
	char *home, home_buf[DB_MAXPATHLEN];

	/*
	 * Set the database home.
	 *
	 * Use db_home by default, this allows utilities to reasonably
	 * override the environment either explicitly or by using a -h
	 * option.  Otherwise, use the environment if it's permitted
	 * and initialized.
	 */
	home = (char *)db_home;
	if (home == NULL && (LF_ISSET(DB_USE_ENVIRON) ||
	    (LF_ISSET(DB_USE_ENVIRON_ROOT) && __os_isroot()))) {
		home = home_buf;
		if ((ret = __os_getenv(
		    dbenv, "DB_HOME", &home, sizeof(home_buf))) != 0)
			return (ret);
		/*
		 * home set to NULL if __os_getenv failed to find DB_HOME.
		 */
	}
	if (home != NULL &&
	    (ret = __os_strdup(dbenv, home, &dbenv->db_home)) != 0)
		return (ret);

	/* Default permissions are read-write for both owner and group. */
	dbenv->db_mode = mode == 0 ? __db_omode("rw-rw----") : mode;

	/* Read the DB_CONFIG file. */
	if ((ret = __env_read_db_config(dbenv)) != 0)
		return (ret);

	/*
	 * If no temporary directory path was specified in the config file,
	 * choose one.
	 */
	if (dbenv->db_tmp_dir == NULL && (ret = __os_tmpdir(dbenv, flags)) != 0)
		return (ret);

	return (0);
}

/*
 * __env_close_pp --
 *	DB_ENV->close pre/post processor.
 *
 * PUBLIC: int __env_close_pp __P((DB_ENV *, u_int32_t));
 */
int
__env_close_pp(dbenv, flags)
	DB_ENV *dbenv;
	u_int32_t flags;
{
	DB_THREAD_INFO *ip;
	int rep_check, ret, t_ret;

	ret = 0;

	if (PANIC_ISSET(dbenv)) {
		/* Close all underlying file handles. */
		(void)__file_handle_cleanup(dbenv);

		/* Close all underlying threads and sockets. */
		if (IS_ENV_REPLICATED(dbenv))
			(void)__repmgr_close(dbenv);

		PANIC_CHECK(dbenv);
	}

	ENV_ENTER(dbenv, ip);
	/*
	 * Validate arguments, but as a DB_ENV handle destructor, we can't
	 * fail.
	 */
	if (flags != 0 &&
	    (t_ret = __db_ferr(dbenv, "DB_ENV->close", 0)) != 0 && ret == 0)
		ret = t_ret;

	rep_check = IS_ENV_REPLICATED(dbenv) ? 1 : 0;
	if (rep_check) {
#ifdef HAVE_REPLICATION_THREADS
		/*
		 * Shut down Replication Manager threads first of all.  This
		 * must be done before __env_rep_enter to avoid a deadlock that
		 * could occur if repmgr's background threads try to do a rep
		 * operation that needs __rep_lockout.
		 */
		if ((t_ret = __repmgr_close(dbenv)) != 0 && ret == 0)
			ret = t_ret;
#endif
		if ((t_ret = __env_rep_enter(dbenv, 0)) != 0 && ret == 0)
			ret = t_ret;
	}

	if ((t_ret = __env_close(dbenv, rep_check)) != 0 && ret == 0)
		ret = t_ret;

	/* Don't ENV_LEAVE as we have already detached from the region. */
	return (ret);
}

/*
 * __env_close --
 *	DB_ENV->close.
 *
 * PUBLIC: int __env_close __P((DB_ENV *, int));
 */
int
__env_close(dbenv, rep_check)
	DB_ENV *dbenv;
	int rep_check;
{
	int ret, t_ret;
	char **p;

	ret = 0;

	/*
	 * Check to see if we were in the middle of restoring transactions and
	 * need to close the open files.
	 */
	if (TXN_ON(dbenv) && (t_ret = __txn_preclose(dbenv)) != 0 && ret == 0)
		ret = t_ret;

#ifdef HAVE_REPLICATION
	if ((t_ret = __rep_env_close(dbenv)) != 0 && ret == 0)
		ret = t_ret;
#endif

	/*
	 * Detach from the regions and undo the allocations done by
	 * DB_ENV->open.
	 */
	if ((t_ret = __env_refresh(dbenv, 0, rep_check)) != 0 && ret == 0)
		ret = t_ret;

#ifdef HAVE_CRYPTO
	/*
	 * Crypto comes last, because higher level close functions need
	 * cryptography.
	 */
	if ((t_ret = __crypto_env_close(dbenv)) != 0 && ret == 0)
		ret = t_ret;
#endif

	/* If we're registered, clean up. */
	if (dbenv->registry != NULL) {
		(void)__envreg_unregister(dbenv, 0);
		dbenv->registry = NULL;
	}

	/* Check we've closed all underlying file handles. */
	if ((t_ret = __file_handle_cleanup(dbenv)) != 0 && ret == 0)
		ret = t_ret;

	/* Release any string-based configuration parameters we've copied. */
	if (dbenv->db_log_dir != NULL)
		__os_free(dbenv, dbenv->db_log_dir);
	dbenv->db_log_dir = NULL;
	if (dbenv->db_tmp_dir != NULL)
		__os_free(dbenv, dbenv->db_tmp_dir);
	dbenv->db_tmp_dir = NULL;
	if (dbenv->db_data_dir != NULL) {
		for (p = dbenv->db_data_dir; *p != NULL; ++p)
			__os_free(dbenv, *p);
		__os_free(dbenv, dbenv->db_data_dir);
		dbenv->db_data_dir = NULL;
		dbenv->data_next = 0;
	}
	if (dbenv->db_home != NULL) {
		__os_free(dbenv, dbenv->db_home);
		dbenv->db_home = NULL;
	}

	/* Discard the structure. */
	__db_env_destroy(dbenv);

	return (ret);
}

/*
 * __env_refresh --
 *	Refresh the DB_ENV structure.
 */
static int
__env_refresh(dbenv, orig_flags, rep_check)
	DB_ENV *dbenv;
	u_int32_t orig_flags;
	int rep_check;
{
	DB *ldbp;
	DB_THREAD_INFO *ip;
	int ret, t_ret;

	ret = 0;

	/*
	 * Release resources allocated by DB_ENV->open, and return it to the
	 * state it was in just before __env_open was called.  (This means
	 * state set by pre-open configuration functions must be preserved.)
	 *
	 * Refresh subsystems, in the reverse order they were opened (txn
	 * must be first, it may want to discard locks and flush the log).
	 *
	 * !!!
	 * Note that these functions, like all of __env_refresh, only undo
	 * the effects of __env_open.  Functions that undo work done by
	 * db_env_create or by a configuration function should go in
	 * __env_close.
	 */
	if (TXN_ON(dbenv) &&
	    (t_ret = __txn_env_refresh(dbenv)) != 0 && ret == 0)
		ret = t_ret;

	if (LOGGING_ON(dbenv) &&
	    (t_ret = __log_env_refresh(dbenv)) != 0 && ret == 0)
		ret = t_ret;

	/*
	 * Locking should come after logging, because closing log results
	 * in files closing which may require locks being released.
	 */
	if (LOCKING_ON(dbenv)) {
		if (!F_ISSET(dbenv, DB_ENV_THREAD) &&
		    dbenv->env_lref != NULL && (t_ret = __lock_id_free(dbenv,
		    (DB_LOCKER *)dbenv->env_lref)) != 0 && ret == 0)
			ret = t_ret;
		dbenv->env_lref = NULL;

		if ((t_ret = __lock_env_refresh(dbenv)) != 0 && ret == 0)
			ret = t_ret;
	}

	/* Discard the DB_ENV handle mutex. */
	if ((t_ret = __mutex_free(dbenv, &dbenv->mtx_env)) != 0 && ret == 0)
		ret = t_ret;

	/*
	 * Discard DB list and its mutex.
	 * Discard the MT mutex.
	 *
	 * !!!
	 * This must be done after we close the log region, because we close
	 * database handles and so acquire this mutex when we close log file
	 * handles.
	 */
	if (dbenv->db_ref != 0) {
		__db_errx(dbenv,
		    "Database handles still open at environment close");
		TAILQ_FOREACH(ldbp, &dbenv->dblist, dblistlinks)
			__db_errx(dbenv, "Open database handle: %s%s%s",
			    ldbp->fname == NULL ? "unnamed" : ldbp->fname,
			    ldbp->dname == NULL ? "" : "/",
			    ldbp->dname == NULL ? "" : ldbp->dname);
		if (ret == 0)
			ret = EINVAL;
	}
	TAILQ_INIT(&dbenv->dblist);
	if ((t_ret = __mutex_free(dbenv, &dbenv->mtx_dblist)) != 0 && ret == 0)
		ret = t_ret;
	if ((t_ret = __mutex_free(dbenv, &dbenv->mtx_mt)) != 0 && ret == 0)
		ret = t_ret;

	if (dbenv->mt != NULL) {
		__os_free(dbenv, dbenv->mt);
		dbenv->mt = NULL;
	}

	if (MPOOL_ON(dbenv)) {
		/*
		 * If it's a private environment, flush the contents to disk.
		 * Recovery would have put everything back together, but it's
		 * faster and cleaner to flush instead.
		 *
		 * Ignore application max-write configuration, we're shutting
		 * down.
		 */
		if (F_ISSET(dbenv, DB_ENV_PRIVATE) &&
		    (t_ret = __memp_sync_int(dbenv, NULL, 0,
		    DB_SYNC_CACHE | DB_SYNC_SUPPRESS_WRITE, NULL, NULL)) != 0 &&
		    ret == 0)
			ret = t_ret;

		if ((t_ret = __memp_env_refresh(dbenv)) != 0 && ret == 0)
			ret = t_ret;
	}

	/*
	 * If we're included in a shared replication handle count, this
	 * is our last chance to decrement that count.
	 *
	 * !!!
	 * We can't afford to do anything dangerous after we decrement the
	 * handle count, of course, as replication may be proceeding with
	 * client recovery.  However, since we're discarding the regions
	 * as soon as we drop the handle count, there's little opportunity
	 * to do harm.
	 */
	if (rep_check && (t_ret = __env_db_rep_exit(dbenv)) != 0 && ret == 0)
		ret = t_ret;

	/*
	 * Refresh the replication region.
	 *
	 * Must come after we call __env_db_rep_exit above.
	 */
	if (REP_ON(dbenv) &&
	    (t_ret = __rep_env_refresh(dbenv)) != 0 && ret == 0)
		ret = t_ret;

#ifdef HAVE_CRYPTO
	/*
	 * Crypto comes last, because higher level close functions need
	 * cryptography.
	 */
	if ((t_ret = __crypto_env_refresh(dbenv)) != 0 && ret == 0)
		ret = t_ret;
#endif

	/*
	 * Mark the thread as out of the env before we get rid of the handles
	 * needed to do so.
	 */
	if (dbenv->thr_hashtab != NULL &&
	    (t_ret = __env_set_state(dbenv, &ip, THREAD_OUT)) != 0 && ret == 0)
		ret = t_ret;

	/*
	 * We are about to detach from the mutex region.  This is the last
	 * chance we have to acquire/destroy a mutex -- acquire/destroy the
	 * mutex and release our reference.
	 *
	 * !!!
	 * There are two DbEnv methods that care about environment reference
	 * counts: DbEnv.close and DbEnv.remove.  The DbEnv.close method is
	 * not a problem because it only decrements the reference count and
	 * no actual resources are discarded -- lots of threads of control
	 * can call DbEnv.close at the same time, and regardless of racing
	 * on the reference count mutex, we wouldn't have a problem.  Since
	 * the DbEnv.remove method actually discards resources, we can have
	 * a problem.
	 *
	 * If we decrement the reference count to 0 here, go to sleep, and
	 * the DbEnv.remove method is called, by the time we run again, the
	 * underlying shared regions could have been removed.  That's fine,
	 * except we might actually need the regions to resolve outstanding
	 * operations in the various subsystems, and if we don't have hard
	 * OS references to the regions, we could get screwed.  Of course,
	 * we should have hard OS references to everything we need, but just
	 * in case, we put off decrementing the reference count as long as
	 * possible.
	 */
	if ((t_ret = __env_ref_decrement(dbenv)) != 0 && ret == 0)
		ret = t_ret;

#ifdef HAVE_MUTEX_SUPPORT
	if (MUTEX_ON(dbenv) &&
	    (t_ret = __mutex_env_refresh(dbenv)) != 0 && ret == 0)
		ret = t_ret;
#endif

	if (dbenv->reginfo != NULL && (t_ret = __env_detach(
	    dbenv, F_ISSET(dbenv, DB_ENV_PRIVATE) ? 1 : 0)) != 0 && ret == 0) {
		ret = t_ret;

		/*
		 * !!!
		 * Don't free dbenv->reginfo or set the reference to NULL,
		 * that was done by __env_detach().
		 */
	}

	if (dbenv->mutex_iq != NULL) {
		__os_free(dbenv, dbenv->mutex_iq);
		dbenv->mutex_iq = NULL;
	}

	if (dbenv->recover_dtab != NULL) {
		__os_free(dbenv, dbenv->recover_dtab);
		dbenv->recover_dtab = NULL;
		dbenv->recover_dtab_size = 0;
	}

	dbenv->flags = orig_flags;

	return (ret);
}

/*
 * __file_handle_cleanup --
 *	Close any underlying open file handles so we don't leak system
 *	resources.
 */
static int
__file_handle_cleanup(dbenv)
	DB_ENV *dbenv;
{
	DB_FH *fhp;

	if (TAILQ_FIRST(&dbenv->fdlist) == NULL)
		return (0);

	__db_errx(dbenv, "File handles still open at environment close");
	while ((fhp = TAILQ_FIRST(&dbenv->fdlist)) != NULL) {
		__db_errx(dbenv, "Open file handle: %s", fhp->name);
		(void)__os_closehandle(dbenv, fhp);
	}
	return (EINVAL);
}

/*
 * __env_get_open_flags
 *	Retrieve the flags passed to DB_ENV->open.
 *
 * PUBLIC: int __env_get_open_flags __P((DB_ENV *, u_int32_t *));
 */
int
__env_get_open_flags(dbenv, flagsp)
	DB_ENV *dbenv;
	u_int32_t *flagsp;
{
	ENV_ILLEGAL_BEFORE_OPEN(dbenv, "DB_ENV->get_open_flags");

	*flagsp = dbenv->open_flags;
	return (0);
}