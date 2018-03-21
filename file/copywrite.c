#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/time.h>

ssize_t safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 */
ssize_t full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
			return cc;  /* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}

ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * We were using 0x7fff0000 as sendfile chunk size, but it
 * was seen to cause largish delays when user tries to ^C a file copy.
 * Let's use a saner size.
 * Note: needs to be >= max(CONFIG_FEATURE_COPYBUF_KB),
 * or else "copy to eof" code will use neddlesly short reads.
 */
#define SENDFILE_BIGBUF (16*1024*1024)
#define CONFIG_FEATURE_COPYBUF_KB 4
/* Used by NOFORK applets (e.g. cat) - must not use xmalloc.
 * size < 0 means "ignore write errors", used by tar --to-command
 * size = 0 means "copy till EOF"
 */
static off_t bb_full_fd_action(int src_fd, int dst_fd, off_t size)
{
	int status = -1;
	off_t total = 0;
	int continue_on_write_error = 0;
	ssize_t sendfile_sz;

	char buffer[CONFIG_FEATURE_COPYBUF_KB * 1024];
	enum { buffer_size = sizeof(buffer) };

	if (size < 0) {
		size = -size;
		continue_on_write_error = 1;
	}

	if (src_fd < 0)
		goto out;
	sendfile_sz = SENDFILE_BIGBUF;
	if (!size) {
		size = SENDFILE_BIGBUF;
		status = 1; /* copy until eof */
	}

	while (1) {
		ssize_t rd;

		if (sendfile_sz) {
			rd = sendfile(dst_fd, src_fd, NULL,
				size > sendfile_sz ? sendfile_sz : size);
			if (rd >= 0)
				goto read_ok;
			sendfile_sz = 0; /* do not try sendfile anymore */
		}

		rd = safe_read(src_fd, buffer,
			size > buffer_size ? buffer_size : size);
		if (rd < 0) {
			//bb_perror_msg(bb_msg_read_error);
			printf("rd<0");
			break;
		}
 read_ok:
		if (!rd) { /* eof - all done */
			status = 0;
			break;
		}
		/* dst_fd == -1 is a fake, else... */
		if (dst_fd >= 0 && !sendfile_sz) {
			ssize_t wr = full_write(dst_fd, buffer, rd);
			if (wr < rd) {
				if (!continue_on_write_error) {
					printf("write error\n");
					break;
				}
				dst_fd = -1;
			}
		}
		total += rd;
		if (status < 0) { /* if we aren't copying till EOF... */
			size -= rd;
			if (!size) {
				/* 'size' bytes copied - all done */
				status = 0;
				break;
			}
		}
	}
 out:
	return status ? -1 : total;
}

off_t bb_copyfd_eof(int fd1, int fd2)
{
	return bb_full_fd_action(fd1, fd2, 0);
}


/* Return:
 * -1 error, copy not made
 *  0 copy is made or user answered "no" in interactive mode
 *    (failures to preserve mode/owner/times are not reported in exit code)
 */
int copy_file(const char *source, const char *dest, int flags)
{
	/* This is a recursive function, try to minimize stack usage */
	/* NB: each struct stat is ~100 bytes */
	struct stat source_stat;
	struct stat dest_stat;
	int retval = 0;
	int dest_exists = 0;
	int ovr;

	if ((lstat)(source, &source_stat) < 0) {
		/* This may be a dangling symlink.
		 * Making [sym]links to dangling symlinks works, so... */
		printf("can't stat '%s'", source);
		return -1;
	}

	if (lstat(dest, &dest_stat) < 0) {
		if (errno != ENOENT) {
			printf("can't stat '%s'", dest);
			return -1;
		}
	} else {
		if (source_stat.st_dev == dest_stat.st_dev
		 && source_stat.st_ino == dest_stat.st_ino
		) {
			printf("'%s' and '%s' are the same file", source, dest);
			return -1;
		}
		dest_exists = 1;
	}


	if (S_ISREG(source_stat.st_mode)) {
		int src_fd;
		int dst_fd;
		mode_t new_mode;

		src_fd = open(source, O_RDONLY, 0666);
		if (src_fd < 0)
			return -1;

		/* Do not try to open with weird mode fields */
		new_mode = source_stat.st_mode;
		if (!S_ISREG(source_stat.st_mode))
			new_mode = 0666;

		// POSIX way is a security problem versus (sym)link attacks
		//if (!ENABLE_FEATURE_NON_POSIX_CP) {
			dst_fd = open(dest, O_WRONLY|O_CREAT|O_TRUNC, new_mode);
		//} else { /* safe way: */
			//dst_fd = open(dest, O_WRONLY|O_CREAT|O_EXCL, new_mode);
		//}
		if (dst_fd == -1) {
			ovr = unlink(dest);
			if (ovr <= 0) {
				close(src_fd);
				return ovr;
			}
			/* It shouldn't exist. If it exists, do not open (symlink attack?) */
			dst_fd = open(dest, O_WRONLY|O_CREAT|O_EXCL, new_mode);
			if (dst_fd < 0) {
				close(src_fd);
				return -1;
			}
		}

		if (bb_copyfd_eof(src_fd, dst_fd) == -1)
			retval = -1;
		/* Careful with writing... */
		if (close(dst_fd) < 0) {
			printf("error writing to '%s'", dest);
			retval = -1;
		}
		/* ...but read size is already checked by bb_copyfd_eof */
		close(src_fd);
		/* "cp /dev/something new_file" should not
		 * copy mode of /dev/something */
		if (!S_ISREG(source_stat.st_mode))
			return retval;
		goto preserve_mode_ugid_time;
	}

 preserve_mode_ugid_time:

	if (1
	/* Cannot happen: */
	/* && !(flags & (FILEUTILS_MAKE_SOFTLINK|FILEUTILS_MAKE_HARDLINK)) */
	) {
		struct timeval times[2];

		times[1].tv_sec = times[0].tv_sec = source_stat.st_mtime;
		times[1].tv_usec = times[0].tv_usec = 0;
		/* BTW, utimes sets usec-precision time - just FYI */
		if (utimes(dest, times) < 0)
			printf("can't preserve %s of '%s'", "times", dest);
		if (chown(dest, source_stat.st_uid, source_stat.st_gid) < 0) {
			source_stat.st_mode &= ~(S_ISUID | S_ISGID);
			printf("can't preserve %s of '%s'", "ownership", dest);
		}
		if (chmod(dest, source_stat.st_mode) < 0)
			printf("can't preserve %s of '%s'", "permissions", dest);
	}

	if (1) {
		printf("'%s' -> '%s'\n", source, dest);
	}

	return retval;
}

int remove_file(const char *path) {
	struct stat path_stat;

	if (lstat(path, &path_stat) < 0) {
		if (errno != ENOENT) {
			printf("can't stat '%s'", path);
			return -1;
		}
		return 0;
	}
	
	if (S_ISDIR(path_stat.st_mode)) {
		printf("folder remove not implemented\n");
	} else {
		if (access(path, W_OK) < 0
	     && !S_ISLNK(path_stat.st_mode)
	     && isatty(0)){
			if (unlink(path) < 0){
				printf("delete file failed\n");
				return -1;
			}
		}
	}
	
	return 0;
}

int main(int argc, char *argv[]){
	int ret = copy_file("/home/weeds/magicwifi/test/datastructure/file/a.out","/tmp/a.out", 0666);
	
	
	printf("ret=%d\n", ret);
	//ret = remove_file("/tmp/readdir.c");
	printf("ret=%d\n", ret);
	return 0;
}
