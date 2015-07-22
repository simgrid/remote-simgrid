#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include <microhttpd.h>
#include <simgrid/s4u.h>

#define pageOK "<html><body>SimGrid server up and running (or almost)</body></html>"

static int display_headers(void *closure,
		enum MHD_ValueKind kind,
        const char *key, const char *value) {
	fprintf(stderr,"SER: response hdr: %d, %s %s\n",kind,key, value);
	return MHD_YES;
}

static int handle_request(void * cls,
		struct MHD_Connection * connection,
		const char * url, const char * method, const char * version, const char * upload_data, size_t * upload_data_size,
		void ** con_cls) {
//	static int dummy; // To notice that we're called a second time
	struct MHD_Response * response;
	int ret;

	fprintf(stderr, "SER: Got an request to handle. URL='%s'; Data='%s' %s\n",url,upload_data,
			(NULL == *con_cls)? "first time":"not first time");
//	if (NULL == *con_cls) { /* First time we're called, just to check the headers */
		if (0 != strcmp(method, "PUT")) {
			fprintf(stderr, "SER: Ignoring a request using method %s (only PUT are served)\n\n", method);
			return MHD_NO;      // Drop that connection, dude
		}
//		*con_cls = &dummy;  // we just validated the headers, no need to do it again and again
//		return MHD_YES;         // ask for the rest, beyond the headers
//	}

	// Split process / action
	char *process_name=strdup(url+1), *action = process_name;
	while (*action !='/' && *action!='\0')
		action++;
	if (*action=='\0') {
		fprintf(stderr, "SER: Malformed URL: %s\n",url);
		free(process_name);
		return MHD_NO;
	}
	*action='\0'; // final \0 of process name
	action++;     // Move pointer onto the action name


	// We can now answer that call
	response = MHD_create_response_from_buffer(strlen(pageOK), (void*)pageOK, MHD_RESPMEM_PERSISTENT);
	fprintf(stderr, "SER: Display headers\n");
	MHD_get_response_headers(response, display_headers, NULL);
	fprintf(stderr, "SER: Done with the headers\n");
	if (!response)
		xbt_die("Unable to generate response!");
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	if (ret == MHD_NO)
		xbt_die("Unable to post answer %p!",response);

	fprintf(stderr,"SER: Process: '%s', Action: '%s', params: '%s', ret:%d \n",process_name,action,upload_data, ret);

	MHD_destroy_response(response);
	free(process_name);
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

int main(int argc, char ** argv) {
	struct MHD_Daemon * d;
	if (argc != 3) {
		printf("Usage: %s platform.xml PORT\n", argv[0]);
		return 1;
	}
	simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc,argv);
	e->loadPlatform(argv[1]);
	int port = atoi(argv[2]);

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
	fprintf(stderr,"SER. Started on port %d. Press enter to shut it down.\n", port);
	getchar();
	MHD_stop_daemon(d);
	return 0;
}


