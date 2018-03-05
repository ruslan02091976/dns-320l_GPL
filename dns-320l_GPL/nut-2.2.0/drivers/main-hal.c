/* main-hal.c - Network UPS Tools driver core for HAL

   Copyright (C) 1999  Russell Kroll <rkroll@exploits.org>
   Copyright (C) 2006  Arnaud Quette <aquette.dev@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "main-hal.h"
#include "dstate-hal.h"
#include <hal/libhal.h>

#ifdef HAVE_POLKIT
#include <libpolkit.h>
#endif

	/* HAL specific */
	extern LibHalContext *halctx;
	extern char *udi;

	/* data which may be useful to the drivers */	
	int	upsfd = -1;
	char	*device_path = NULL;
	const	char	*progname = NULL, *upsname = NULL, 
			*device_name = NULL;

	/* may be set by the driver to wake up while in dstate_poll_fds */
	int	extrafd = -1;

	/* for ser_open */
	int	do_lock_port = 1;

	/* set by the drivers */
	int	experimental_driver = 0;
	int	broken_driver = 0;

	/* for detecting -a values that don't match anything */
	static	int	upsname_found = 0;

	static vartab_t	*vartab_h = NULL;

	/* variables possibly set by the global part of ups.conf */
	unsigned int	poll_interval = 2;
	static char	*chroot_path = NULL, *user = NULL;

	/* signal handling */
	int	exit_flag = 0;
	static	sigset_t		main_sigmask;
	static	struct	sigaction	main_sa;

	/* everything else */
	static	char	*pidfn = NULL;

#if 0 /* HAL stripping */
/* power down the attached load immediately */
static void forceshutdown(void)
{
	upslogx(LOG_NOTICE, "Initiating UPS shutdown");

	/* the driver must not block in this function */
	upsdrv_shutdown();
	exit(EXIT_SUCCESS);
}

/* this function only prints the usage message; it does not call exit() */
static void help_msg(void)
{
	vartab_t	*tmp;

	printf("\nusage: %s [OPTIONS] [<device>]\n\n", progname);

	printf("  -V             - print version, then exit\n");
	printf("  -L             - print parseable list of driver variables\n");
	printf("  -a <id>        - autoconfig using ups.conf section <id>\n");
	printf("                 - note: -x after -a overrides ups.conf settings\n");
	printf("  -D             - raise debugging level\n");
	printf("  -h             - display this help\n");
	printf("  -k             - force shutdown\n");
	printf("  -i <int>       - poll interval\n");
	printf("  -r <dir>       - chroot to <dir>\n");
	printf("  -u <user>      - switch to <user> (if started as root)\n");
	printf("  -x <var>=<val> - set driver variable <var> to <val>\n");
	printf("                 - example: -x cable=940-0095B\n\n");

	printf("  <device>       - /dev entry corresponding to UPS port\n");
	printf("                 - Only optional when using -a!\n\n");

	if (vartab_h) {
		tmp = vartab_h;

		printf("Acceptable values for -x or ups.conf in this driver:\n\n");

		while (tmp) {
			if (tmp->vartype == VAR_VALUE)
				printf("%40s : -x %s=<value>\n", 
					tmp->desc, tmp->var);
			else
				printf("%40s : -x %s\n", tmp->desc, tmp->var);
			tmp = tmp->next;
		}
	}

	upsdrv_help();
}

/* store these in dstate as driver.(parameter|flag) */
static void dparam_setinfo(const char *var, const char *val)
{
	char	vtmp[SMALLBUF];

	/* store these in dstate for debugging and other help */
	if (val) {
		snprintf(vtmp, sizeof(vtmp), "driver.parameter.%s", var);
		dstate_setinfo(vtmp, "%s", val);
		return;
	}

	/* no value = flag */

	snprintf(vtmp, sizeof(vtmp), "driver.flag.%s", var);
	dstate_setinfo(vtmp, "enabled");
}

/* cram var [= <val>] data into storage */
static void storeval(const char *var, char *val)
{
	vartab_t	*tmp, *last;

	tmp = last = vartab_h;

	while (tmp) {
		last = tmp;

		/* sanity check */
		if (!tmp->var) {
			tmp = tmp->next;
			continue;
		}

		/* later definitions overwrite earlier ones */
		if (!strcasecmp(tmp->var, var)) {
			free(tmp->val);
			if (val)
				tmp->val = xstrdup(val);

			/* don't keep things like SNMP community strings */
			if ((tmp->vartype & VAR_SENSITIVE) == 0)
				dparam_setinfo(var, val);

			tmp->found = 1;
			return;
		}

		tmp = tmp->next;
	}

	/* try to help them out */
	printf("\nFatal dbus_error: '%s' is not a valid %s for this driver.\n", var,
		val ? "variable name" : "flag");
	printf("\n");
	printf("Look in the man page or call this driver with -h for a list of\n");
	printf("valid variable names and flags.\n");

	exit(EXIT_SUCCESS);
}
#endif

/* retrieve the value of variable <var> if possible */
char *getval(const char *var)
{
	vartab_t	*tmp = vartab_h;

	while (tmp) {
		if (!strcasecmp(tmp->var, var))
			return(tmp->val);
		tmp = tmp->next;
	}

	return NULL;
}

/* see if <var> has been defined, even if no value has been given to it */
int testvar(const char *var)
{
	vartab_t	*tmp = vartab_h;

	while (tmp) {
		if (!strcasecmp(tmp->var, var))
			return tmp->found;
		tmp = tmp->next;
	}

	return 0;	/* not found */
}

/* callback from driver - create the table for -x/conf entries */
void addvar(int vartype, const char *name, const char *desc)
{
	vartab_t	*tmp, *last;

	tmp = last = vartab_h;

	while (tmp) {
		last = tmp;
		tmp = tmp->next;
	}

	tmp = xmalloc(sizeof(vartab_t));

	tmp->vartype = vartype;
	tmp->var = xstrdup(name);
	tmp->val = NULL;
	tmp->desc = xstrdup(desc);
	tmp->found = 0;
	tmp->next = NULL;

	if (last)
		last->next = tmp;
	else
		vartab_h = tmp;
}

#if 0 /* HAL stripping */
/* handle -x / ups.conf config details that are for this part of the code */
static int main_arg(char *var, char *val)
{
	/* flags for main: just 'nolock' for now */

	if (!strcmp(var, "nolock")) {
		do_lock_port = 0;
		dstate_setinfo("driver.flag.nolock", "enabled");
		return 1;	/* handled */
	}

	/* any other flags are for the driver code */
	if (!val)
		return 0;

	/* variables for main: port */

	if (!strcmp(var, "port")) {
		device_path = xstrdup(val);
		device_name = xbasename(device_path);
		dstate_setinfo("driver.parameter.port", "%s", val);
		return 1;	/* handled */
	}

	if (!strcmp(var, "sddelay")) {
		upslogx(LOG_INFO, "Obsolete value sddelay found in ups.conf");
		return 1;	/* handled */
	}

	/* only for upsdrvctl - ignored here */
	if (!strcmp(var, "sdorder"))
		return 1;	/* handled */

	/* only for upsd (at the moment) - ignored here */
	if (!strcmp(var, "desc"))
		return 1;	/* handled */

	return 0;	/* unhandled, pass it through to the driver */
}

static void do_global_args(const char *var, const char *val)
{
	if (!strcmp(var, "pollinterval")) {
		poll_interval = atoi(val);
		return;
	}

	if (!strcmp(var, "chroot")) {
		free(chroot_path);
		chroot_path = xstrdup(val);
	}

	if (!strcmp(var, "user")) {
		free(user);
		user = xstrdup(val);
	}


	/* unrecognized */
}

void do_upsconf_args(char *confupsname, char *var, char *val)
{
	char	tmp[SMALLBUF];

	/* handle global declarations */
	if (!confupsname) {
		do_global_args(var, val);
		return;
	}

	/* no match = not for us */
	if (strcmp(confupsname, upsname) != 0)
		return;

	upsname_found = 1;

	if (main_arg(var, val))
		return;

	/* flags (no =) now get passed to the driver-level stuff */
	if (!val) {

		/* also store this, but it's a bit different */
		snprintf(tmp, sizeof(tmp), "driver.flag.%s", var);
		dstate_setinfo(tmp, "enabled");

		storeval(var, NULL);
		return;
	}

	/* don't let the user shoot themselves in the foot */
	if (!strcmp(var, "driver")) {
		if (strcmp(val, progname) != 0)
			fatalx(EXIT_FAILURE, "Error: UPS [%s] is for driver %s, but I'm %s!\n",
				confupsname, val, progname);
		return;
	}

	/* allow per-driver overrides of the global setting */
	if (!strcmp(var, "pollinterval")) {
		poll_interval = atoi(val);
		return;
	}

	/* everything else must be for the driver */

	storeval(var, val);
}

/* split -x foo=bar into 'foo' and 'bar' */
static void splitxarg(char *inbuf)
{
	char	*eqptr, *val, *buf;

	/* make our own copy - avoid changing argv */
	buf = xstrdup(inbuf);

	eqptr = strchr(buf, '=');

	if (!eqptr)
		val = NULL;
	else {
		*eqptr++ = '\0';
		val = eqptr;
	}

	/* see if main handles this first */
	if (main_arg(buf, val))
		return;

	/* otherwise store it for later */
	storeval(buf, val);
}

/* dump the list from the vartable for external parsers */
static void listxarg(void)
{
	vartab_t	*tmp;

	tmp = vartab_h;

	if (!tmp)
		return;

	while (tmp) {

		switch (tmp->vartype) {
			case VAR_VALUE: printf("VALUE"); break;
			case VAR_FLAG: printf("FLAG"); break;
			default: printf("UNKNOWN"); break;
		}

		printf(" %s \"%s\"\n", tmp->var, tmp->desc);

		tmp = tmp->next;
	}
}
#endif

static void vartab_free(void)
{
	vartab_t	*tmp, *next;

	tmp = vartab_h;

	while (tmp) {
		next = tmp->next;

		free(tmp->var);
		free(tmp->val);
		free(tmp->desc);
		free(tmp);

		tmp = next;
	}
}

#if 0 /* HAL stripping */
/* FIXME: really needed by drivers? */
char *getval(const char *var) { return ""; }
int testvar(const char *var) { return 0; /* not found */}
void addvar(int vartype, const char *name, const char *desc) {}
#endif

static void exit_cleanup(void)
{
	upsdrv_cleanup();

	free(chroot_path);
	free(device_path);
	free(user);

	if (pidfn) {
		unlink(pidfn);
		free(pidfn);
	}

	dstate_free();
	vartab_free();
}

static void set_exit_flag(int sig)
{
	exit_flag = sig;
}

static void setup_signals(void)
{
	sigemptyset(&main_sigmask);
	main_sa.sa_mask = main_sigmask;
	main_sa.sa_flags = 0;

	main_sa.sa_handler = set_exit_flag;
	sigaction(SIGTERM, &main_sa, NULL);
	sigaction(SIGINT, &main_sa, NULL);
	sigaction(SIGQUIT, &main_sa, NULL);

	main_sa.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &main_sa, NULL);
	sigaction(SIGPIPE, &main_sa, NULL);
}

int main(int argc, char **argv)
{
	DBusError dbus_error;
	DBusConnection	*dbus_connection;
	struct	passwd	*new_uid = NULL;
	int	i, do_forceshutdown = 0;

	/* pick up a default from configure --with-user */
	/* FIXME: really needed, or use inherited privs from hald? */
	user = xstrdup(HAL_USER);	/* xstrdup: this gets freed at exit */

	upsdrv_banner();

	if (experimental_driver) {
		printf("Warning: This is an experimental driver.\n");
		printf("Some features may not function correctly.\n\n");
	}

	progname = xbasename(argv[0]);
	open_syslog(progname);

	/* initialise HAL and DBus interface*/
	halctx = NULL;
	udi = getenv ("UDI");
	if (udi == NULL) {
		fprintf(stderr, "Error: UDI is null.\n");
		exit(EXIT_FAILURE);
	}

	dbus_error_init (&dbus_error);
	if ((halctx = libhal_ctx_init_direct (&dbus_error)) == NULL) {
		fprintf(stderr, "Error: can't initialise libhal.\n");
		exit(EXIT_FAILURE);
	}
	if (dbus_error_is_set (&dbus_error))
	{
		fatalx(EXIT_FAILURE, "Error in context creation: %s\n", dbus_error.message);
		dbus_error_free (&dbus_error);
	}
	
/*	if ((dbus_connection = libhal_ctx_get_dbus_connection(halctx)) == NULL) {
		fprintf(stderr, "Error: can't get DBus connection.\n");
		exit(EXIT_FAILURE);
	}
*/
	/* FIXME: rework HAL param interface! or get path/regex from UDI? */
	/* from UDI, appending usbraw, you can access to usbraw.device. Ex
	/* /org/freedesktop/Hal/devices/usb_device_463_ffff_1H2E300AH_usbraw
	 * => usbraw.device = /dev/bus/usb/002/065 */	
	device_path = xstrdup("auto"); /*getenv ("HAL_PROP_HIDDEV_DEVICE"); */
	nut_debug_level = 3;

	/* Sleep 2 seconds to be able to view the device through usbfs! */
	sleep(2);

/*	dbus_error_init (&dbus_error);
	if (!libhal_device_addon_is_ready (halctx, udi, &dbus_error)) {
		fprintf(stderr, "Error (libhal): device addon is not ready\n");
		exit(EXIT_FAILURE);
	}
*/
	/* Claim interface with void methods, while waiting to register
	 * actual methods, through driver core calling dstate_addcmd()
	 */
/*	if (!libhal_device_claim_interface(halctx, udi, DBUS_INTERFACE,
		"    <method name=\"SetBeeper\">\n"
		"      <arg name=\"beeper_mode\" direction=\"in\" type=\"b\"/>\n"
		"      <arg name=\"return_code\" direction=\"out\" type=\"i\"/>\n"
		"    </method>\n",
		&dbus_error)) {
			fprintf(stderr, "Error: can't claim DBus interface: %s", dbus_error.message);
			exit(EXIT_FAILURE);
	}
*/
/*	if (!libhal_device_claim_interface(halctx, udi, DBUS_INTERFACE,
		"<method></method>", &dbus_error)) {
		fprintf(stderr, "Error: can't claim DBus interface: %s", dbus_error.message);
		exit(EXIT_FAILURE);
	}
	libhal_device_add_capability(halctx,
				     "/org/freedesktop/Hal/devices/computer",
				     "UPS",
				     &dbus_error);
*/    
/* FIXME: got an undef ref! */
/*	dbus_connection_setup_with_g_main(dbus_connection, NULL);
	dbus_connection_add_filter(dbus_connection, dbus_filter_function, NULL, NULL);
	dbus_connection_set_exit_on_disconnect(dbus_connection, 0);

	dbus_init_local(); */
	/* end of HAL init */
	
	/* build the driver's extra (-x) variable table */
	upsdrv_makevartable();

/* HAL stripping */
#if 0
	while ((i = getopt(argc, argv, "+a:kDhx:Lr:u:Vi:")) != -1) {
		switch (i) {
			case 'a':
				upsname = optarg;

				read_upsconf();

				if (!upsname_found)
					upslogx(LOG_WARNING, "Warning: Section %s not found in ups.conf",
						optarg);
				break;
			case 'D':
				nut_debug_level++;
				break;
			case 'i':
				poll_interval = atoi(optarg);
				break;
			case 'k':
				do_lock_port = 0;
				do_forceshutdown = 1;
				break;
			case 'L':
				listxarg();
				exit(EXIT_SUCCESS);
			case 'r':
				chroot_path = xstrdup(optarg);
				break;
			case 'u':
				user = xstrdup(optarg);
				break;
			case 'V':
				/* already printed the banner, so exit */
				exit(EXIT_SUCCESS);
			case 'x':
				splitxarg(optarg);
				break;
			case 'h':
				help_msg();
				exit(EXIT_SUCCESS);
			default:
				fprintf(stderr, "Error: unknown option -%c. Try -h for help.\n", i);
				exit(EXIT_FAILURE);
				break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 1) {
		fprintf(stderr, "Error: too many non-option arguments. Try -h for help.\n");
		exit(EXIT_FAILURE);
	}

	/* we need to get the port from somewhere */
	if (argc < 1) {
		if (!device_path) {
			fprintf(stderr, "Error: You must specify a port name in ups.conf or on the command line.\n");
			fprintf(stderr, "Try -h for help.\n");
			exit(EXIT_FAILURE);
		}
	}

	/* allow argv to override the ups.conf entry if specified */
	else {
		device_path = xstrdup(argv[0]);
		device_name = xbasename(device_path);
	}
#endif
	device_name = xbasename(device_path);

	pidfn = xmalloc(SMALLBUF);

	if (upsname_found)
		snprintf(pidfn, SMALLBUF, "%s/%s-%s.pid", 
			altpidpath(), progname, upsname);
	else
		snprintf(pidfn, SMALLBUF, "%s/%s-%s.pid", 
			 altpidpath(), progname, device_name);

	upsdebugx(1, "debug level is '%d'", nut_debug_level);

	new_uid = get_user_pwent(user);
	
	if (chroot_path)
		chroot_start(chroot_path);

	become_user(new_uid);

	/* Only switch to statepath if we're not powering off */
	/* This avoid case where ie /var is umounted */
/*	! Not needed for HAL !
	if (!do_forceshutdown)
		if (chdir(dflt_statepath()))
			fatal_with_errno(EXIT_FAILURE, "Can't chdir to %s", dflt_statepath());
*/
	setup_signals();

	/* clear out callback handler data */
	memset(&upsh, '\0', sizeof(upsh));

	upsdrv_initups();

	/* now see if things are very wrong out there */
	if (broken_driver) {
		printf("Fatal dbus_error: broken driver. It probably needs to be converted.\n");
		printf("Search for 'broken_driver = 1' in the source for more details.\n");
		exit(EXIT_FAILURE);
	}

#if 0
	if (do_forceshutdown)
		forceshutdown();
#endif
	/* get the base data established before allowing connections */
 	upsdrv_initinfo();
	upsdrv_updateinfo();

	/* now we can start servicing requests */
	if (upsname_found)
		dstate_init(upsname, NULL);
	else
		dstate_init(progname, device_name);

	/* publish the top-level data: version number, driver name */
	dstate_setinfo("driver.version", "%s", UPS_VERSION);
	dstate_setinfo("driver.name", "%s", progname);

	/* The poll_interval may have been changed from the default */
	dstate_setinfo("driver.parameter.pollinterval", "%d", poll_interval);

	if (nut_debug_level == 0) {
		background();
		writepid(pidfn);
	}

	/* safe to do this now that the parent has exited */
	atexit(exit_cleanup);

	/* End HAL init */
	dbus_error_init (&dbus_error);
	if (!libhal_device_addon_is_ready (halctx, udi, &dbus_error)) {
		fprintf(stderr, "Error (libhal): device addon is not ready\n");
		exit(EXIT_FAILURE);
	}

	while (exit_flag == 0) {
		upsdrv_updateinfo();

		dstate_poll_fds(poll_interval, extrafd);
	}

	/* if we get here, the exit flag was set by a signal handler */

	upslogx(LOG_INFO, "Signal %d: exiting", exit_flag);

	exit(EXIT_SUCCESS);
}