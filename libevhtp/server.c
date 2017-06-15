#include <stdio.h>
#include <evhtp.h>

void testcb(evhtp_request_t *req, void *a)
{
	evbuffer_add_reference(req->buffer_in, "foobar", 6, NULL, NULL);
	evhtp_send_reply(req, EVHTP_RES_OK);
}

void querycb(evhtp_request_t *req, void *a)
{
	struct evbuffer *evb;
	evb = evbuffer_new();
	
	evbuffer_add_reference(evb, "fuckbar", 7, NULL, NULL);
	evhtp_headers_add_header(req->headers_out, 
		evhtp_header_new("Content-Length", "7", 0, 1));
	//evhtp_headers_add_headers();
	evhtp_send_reply_start(req, EVHTP_RES_OK);
	evhtp_send_reply_body(req, evb);
	//evhtp_send_reply_chunk(req, evb);
	//evhtp_send_reply_chunk_end(req);
	evbuffer_free(evb);
}

int main()
{
	evbase_t *evbase = event_base_new();
	evhtp_t *htp = evhtp_new(evbase, NULL);
	
	evhtp_set_cb(htp, "/test", testcb, NULL);
	evhtp_set_cb(htp, "/query", querycb, NULL);
	evhtp_bind_socket(htp, "0.0.0.0", 8080, 1024);
	event_base_loop(evbase, 0);
	
	event_base_free(evbase);
	return 0;
}
