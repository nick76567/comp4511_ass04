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

struct xmerge_param{
	__user const char *outfile;
	__user const char **infiles;
	int infile_count;
	int oflags;
	mode_t mode;
	__user int *ofile_count;
};

long file_read_write(int in_fd, int out_fd){
	char buffer;
	int read_res = -1 , total_byte = 0;
	if(in_fd < 0 || out_fd < 0){
		printk(KERN_INFO "Error in file open\n");
		return -1;
	}

	while(1){
		if((read_res = sys_read(in_fd, &buffer, 1)) < 0) return -1;
		if(sys_write(out_fd, &buffer, 1) < 0) return -1;
		if(read_res == 0) break;
		total_byte++;
	}
	
	return total_byte;
}

long map_file_read_write(struct xmerge_param *ps){
	char infile[FILE_NAME_SIZE], outfile[FILE_NAME_SIZE];
	int rw_res = 0, total_byte = 0, ofile_count = 0, in_fd, out_fd, i;
	
	if(copy_from_user(outfile, ps->outfile, FILE_NAME_SIZE)){
		printk(KERN_INFO "Errpr copy from user outfile\n");
		return -EFAULT;
	}

	out_fd = sys_open(outfile, O_APPEND | O_WRONLY, 0);
	if(out_fd < 0) {
		printk(KERN_INFO "Error in open outfile\n");
		return -1;
	}

	for(i = 0; i < ps->infile_count; i++){
		if(copy_from_user(infile, ps->infiles[i], FILE_NAME_SIZE)){
		printk(KERN_INFO "Error copy infile %d\n", i);
		return -EFAULT;
	}
		in_fd = sys_open(infile, O_RDONLY, 0);
		rw_res = file_read_write(in_fd, out_fd);
		ofile_count++;
		sys_close(in_fd);

		if(rw_res < 0){
			printk(KERN_INFO "Error after rw %d\n", i);
			return -1;
		}else{
			total_byte += rw_res;
		}	
		
	}
	sys_close(out_fd);
	if(put_user(ofile_count, ps->ofile_count)){
		printk(KERN_INFO "Error in put user\n");
		return -1;
	}
	
	return total_byte;
	
}

asmlinkage long sys_xmerge(void *args, size_t argslen){
	struct xmerge_param xmerge;
	int res;	
	mm_segment_t old_fs;


	if(copy_from_user(&xmerge, args, argslen)){
		printk(KERN_INFO "Error copy from user struct\n");
		return -EFAULT;
	}


	old_fs = get_fs();
	set_fs(KERNEL_DS);	

	res = map_file_read_write(&xmerge);
		
	set_fs(old_fs);	

	if(res < 0){
		printk(KERN_INFO "Error after map function\n");
		return -1;
	}else{
		return res;
	}
	
}
