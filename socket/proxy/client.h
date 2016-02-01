#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include "util.h"

struct mux_client;
int client_accept(int fd);
void client_get_fds(struct fdlist *list);
//void client_process(int fd, short events);
void client_process(int fd, short events, enum fdowner owner);

void client_init(void);
void client_shutdown(void);

int client_read(struct mux_client *client, void *buffer, uint32_t len);
int client_write(struct mux_client *client, void *buffer, uint32_t len);
int client_set_events(struct mux_client *client, short events);
void client_close(struct mux_client *client);

#endif
