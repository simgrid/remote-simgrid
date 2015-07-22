#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include "simgrid/s4u.h"

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
		"</head><body>libmicrohttpd demo %d</body></html>"

static int handle_request(void * cls,
		struct MHD_Connection * connection,
		const char * url, const char * method, const char * version, const char * upload_data, size_t * upload_data_size,
		void ** ptr) {
	static int dummy;
	const char * page = PAGE;
	struct MHD_Response * response;
	int ret;

	if (&dummy != *ptr) {
		if (0 != strcmp(method, "POST")) {
			fprintf(stderr, "Method is not POST but %s\n", method);
			return MHD_NO; /* unexpected method */
		}
		/* The first time only the headers are valid, do not respond in the first round... */
		*ptr = &dummy;  // note that we already validated the headers
		return MHD_YES; // ask for the rest, beyond the headers
	}
	*ptr = NULL; /* clear context pointer: prepare to accept the headers only on next request  */
	fprintf(stderr, "Got an request to handle. URL=%s; Data='%s'\n",url,upload_data);

	// Split process / action
	char *process=strdup(url+1), *action = process;
	while (*action !='/' && *action!='\0')
		action++;
	if (*action=='\0') {
		fprintf(stderr, "Malformed URL: %s\n",url);
		free(process);
		return MHD_NO;
	}
	*action='\0'; // final \0 of process name
	action++;     // Move pointer onto the action name

	fprintf(stderr,"Process: '%s', Action: '%s' \n",process,action);

	// We can now answer that call
	response = MHD_create_response_from_data(strlen(page),
			(void*) page,
			MHD_NO,
			MHD_NO);
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	free(process);
	return ret;
}

static void catcher (int sig){}

static void ignore_sigpipe () {
  struct sigaction oldsig;
  struct sigaction sig;

  sig.sa_handler = &catcher;
  sigemptyset (&sig.sa_mask);
  sig.sa_flags = SA_INTERRUPT;
  if (0 != sigaction (SIGPIPE, &sig, &oldsig))
    fprintf (stderr, "Failed to install SIGPIPE handler: %s\n", strerror (errno));
}

int main(int argc,
		char ** argv) {
	struct MHD_Daemon * d;
	if (argc != 2) {
		printf("Usage: %s PORT\n", argv[0]);
		return 1;
	}
	int port = atoi(argv[1]);

	ignore_sigpipe();
	d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
			port,
			NULL,
			NULL,
			&handle_request,
			NULL,
			MHD_OPTION_END);
	if (d == NULL)
		return 1;
	fprintf(stderr,"Server started on port %d. Press enter to shut it down.\n", port);
	getchar();
	MHD_stop_daemon(d);
	return 0;
}


