#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <linux/fcntl.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#define FILE_NAME_SIZE		32
#define BUF_SIZE			1024

struct xmerge_param{
	__user const char *outfile;
	__user const char **infiles;
	int infile_count;
	int oflags;
	mode_t mode;
	__user int *ofile_count;
};


long xmerge_param_check(struct xmerge_param *ps){
	if(ps->infiles == NULL) return -ENOENT;
	if(ps->oflags == -1 || ps->mode == 1) return -EINVAL;
	return 0;
}


long f_open(__user const char *f_name, int oflags, mode_t mode){
	long cp_res, chmod_res, fd;
	char name[FILE_NAME_SIZE];

	if((cp_res = copy_from_user(name, f_name, FILE_NAME_SIZE)) < 0)
		return cp_res;

	if((fd = sys_open(name, oflags, mode)) < 0)
		return fd;
	
	if(oflags != O_RDONLY && ((chmod_res == sys_chmod(f_name, mode)) < 0))
		return chmod_res;
		
	return fd;
}


long f_read_write(int in_fd, int out_fd){
	char buf[BUF_SIZE];
	long r_bytes, w_bytes, total_bytes = 0;

	do{
		if((r_bytes = sys_read(in_fd, buf, BUF_SIZE)) < 0)
			return r_bytes;

		if((w_bytes = sys_write(out_fd, buf, r_bytes)) < 0)
			return w_bytes;

		total_bytes += w_bytes;
	}while(r_bytes == BUF_SIZE);
	
	return total_bytes;
}


long map_f_read_write(struct xmerge_param *ps){
	long i, rw_bytes, res, total_bytes = 0;
	long in_fd, out_fd, ofile_count = 0;

	if((out_fd = f_open(ps->outfile, ps->oflags | O_WRONLY, ps->mode)) < 0)
		return out_fd;

	for(i = 0; i < ps->infile_count; i++){

		if((in_fd = f_open(ps->infiles[i], O_RDONLY, 0)) < 0)
			return in_fd;

		if((rw_bytes = f_read_write(in_fd, out_fd)) < 0)
			return rw_bytes;

		sys_close(in_fd);
		ofile_count++;
		total_bytes += rw_bytes;
	}

	if((res = put_user(ofile_count, ps->ofile_count)) < 0)
		return res;

	sys_close(out_fd);
	return total_bytes;
}


asmlinkage long sys_xmerge(void *args, size_t argslen){
	struct xmerge_param xmerge;
	long res;	
	mm_segment_t old_fs;


	if(copy_from_user(&xmerge, args, argslen))
		return -EFAULT;
	
	if((res = xmerge_param_check(&xmerge)) != 0) return -res;

	old_fs = get_fs();
	set_fs(KERNEL_DS);	

	res = map_f_read_write(&xmerge);
		
	set_fs(old_fs);	

	if(res < 0)	return -res;
	
	return res;
}