#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_
#ifdef __KERNEL__

#include <linux/mount.h>
#include <linux/sched.h>

struct namespace {
	atomic_t		count;			//引用计数器（共享命名空间的进程数）
	struct vfsmount *	root;			//命名空间根目录的已安装文件系统描述符
	struct list_head	list;			//所有已安装根文件系统描述符链表的头
	struct rw_semaphore	sem;			//保护这个结构的读/写信号量
};

extern void umount_tree(struct vfsmount *);
extern int copy_namespace(int, struct task_struct *);
extern void __put_namespace(struct namespace *namespace);

static inline void put_namespace(struct namespace *namespace)
{
	if (atomic_dec_and_test(&namespace->count))
		__put_namespace(namespace);
}

static inline void exit_namespace(struct task_struct *p)
{
	struct namespace *namespace = p->namespace;
	if (namespace) {
		task_lock(p);
		p->namespace = NULL;
		task_unlock(p);
		put_namespace(namespace);
	}
}

static inline void get_namespace(struct namespace *namespace)
{
	atomic_inc(&namespace->count);
}

#endif
#endif
