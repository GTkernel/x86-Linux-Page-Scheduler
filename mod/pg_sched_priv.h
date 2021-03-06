#ifndef __PG_SCHED_PRIV_H__
#define __PG_SCHED_PRIV_H__
//probably need to include some headers for stlye
//but putting below all includes should work nonetheless

/* struct initial_vma { */
/*     struct vm_area_struct * vma; */
/*     struct list_head linkage; */
/* }; */

/* struct page_desc { */
/*     char age; */
/*     /\* int accesses; *\/ */
/*     /\* int node; *\/ */
/* }; */

struct vma_desc {
    struct vm_area_struct * vma; /*Use as key*/
    unsigned long           vm_start;
    unsigned long           vm_end;
    /* struct page_desc *      page_accesses; */
    unsigned long           num_pages;
    int                     touched;
    /* enum desc_alloc_method  alloc_method; */
    unsigned long key;
    
    struct list_head linkage; /* struct tracked_process -> vma_list */
};

struct tracked_process {
    pid_t pid;
    int migration_enabled;
    struct {
        struct {
            unsigned long sec;
            unsigned long nsec;
        } period;

	int ratio; /* out of 20 */
	int alpha; /* out of 1024 */
	int theta; /* Threshold */
	enum hotness_policy class;
	int max_promote; /* dynamic */
	int max_demote;  /* dynamic */
        int max_migrations; /* static limit on migrations */
	int epoch;
	int mig_cycle;
        int warmup_scans;
    } policy;

    struct mm_struct * mm;

    struct kref refcount;
    struct list_head linkage; /* List: (static global) pg_sched_tracked_pids */

    struct list_head vma_desc_list; /* VMA's that we are watching */
    /* int vma_desc_list_length; /\* For debugging merged or disappearing vma's *\/ */

    /* struct list_head initial_vma_list; /\*.so's and other stuff I dont want to touch*\/ */
    
    struct scanner_params {
        ktime_t kt;
        struct hrtimer timer;
        struct task_struct* scanner_thread;
    } scanner_thread_struct;

    int slow_pages;
    unsigned long key;
    
    void (*release) (struct kref * refc);
};

void free_unctouched_vmas(struct tracked_process * this);

int
get_vma_desc_add_if_absent(struct tracked_process * this,
                           struct vm_area_struct * vma,
                           struct vma_desc ** res);

extern int hist_size;
extern int pg_max;
#endif /* __PG_SCHED_PRIV_H__ */
