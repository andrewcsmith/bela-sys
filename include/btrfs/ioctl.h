/*
 * Copyright (C) 2007 Oracle.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#ifndef __IOCTL_
#define __IOCTL_
#include <asm/types.h>
#include <linux/ioctl.h>

#define BTRFS_IOCTL_MAGIC 0x94
#define BTRFS_VOL_NAME_MAX 255

/* this should be 4k */
#define BTRFS_PATH_NAME_MAX 4087
struct btrfs_ioctl_vol_args {
	__s64 fd;
	char name[BTRFS_PATH_NAME_MAX + 1];
};

#define BTRFS_SUBVOL_RDONLY		(1ULL << 1)
#define BTRFS_SUBVOL_NAME_MAX 4039

struct btrfs_ioctl_vol_args_v2 {
	__s64 fd;
	__u64 transid;
	__u64 flags;
	__u64 unused[4];
	char name[BTRFS_SUBVOL_NAME_MAX + 1];
};

#define BTRFS_FSID_SIZE 16
#define BTRFS_UUID_SIZE 16

struct btrfs_scrub_progress {
	__u64 data_extents_scrubbed;
	__u64 tree_extents_scrubbed;
	__u64 data_bytes_scrubbed;
	__u64 tree_bytes_scrubbed;
	__u64 read_errors;
	__u64 csum_errors;
	__u64 verify_errors;
	__u64 no_csum;
	__u64 csum_discards;
	__u64 super_errors;
	__u64 malloc_errors;
	__u64 uncorrectable_errors;
	__u64 corrected_errors;
	__u64 last_physical;
	__u64 unverified_errors;
};

#define BTRFS_SCRUB_READONLY	1
struct btrfs_ioctl_scrub_args {
	__u64 devid;				/* in */
	__u64 start;				/* in */
	__u64 end;				/* in */
	__u64 flags;				/* in */
	struct btrfs_scrub_progress progress;	/* out */
	/* pad to 1k */
	__u64 unused[(1024-32-sizeof(struct btrfs_scrub_progress))/8];
};

#define BTRFS_DEVICE_PATH_NAME_MAX 1024
struct btrfs_ioctl_dev_info_args {
	__u64 devid;				/* in/out */
	__u8 uuid[BTRFS_UUID_SIZE];		/* in/out */
	__u64 bytes_used;			/* out */
	__u64 total_bytes;			/* out */
	__u64 unused[379];			/* pad to 4k */
	__u8 path[BTRFS_DEVICE_PATH_NAME_MAX];	/* out */
};

struct btrfs_ioctl_fs_info_args {
	__u64 max_id;				/* out */
	__u64 num_devices;			/* out */
	__u8 fsid[BTRFS_FSID_SIZE];		/* out */
	__u64 reserved[124];			/* pad to 1k */
};

/* balance control ioctl modes */
#define BTRFS_BALANCE_CTL_PAUSE		1
#define BTRFS_BALANCE_CTL_CANCEL	2
#define BTRFS_BALANCE_CTL_RESUME	3

/*
 * this is packed, because it should be exactly the same as its disk
 * byte order counterpart (struct btrfs_disk_balance_args)
 */
struct btrfs_balance_args {
	__u64 profiles;
	__u64 usage;
	__u64 devid;
	__u64 pstart;
	__u64 pend;
	__u64 vstart;
	__u64 vend;

	__u64 target;

	__u64 flags;

	__u64 unused[8];
} __attribute__ ((__packed__));

struct btrfs_balance_progress {
	__u64 expected;
	__u64 considered;
	__u64 completed;
};

#define BTRFS_BALANCE_STATE_RUNNING	(1ULL << 0)
#define BTRFS_BALANCE_STATE_PAUSE_REQ	(1ULL << 1)
#define BTRFS_BALANCE_STATE_CANCEL_REQ	(1ULL << 2)

struct btrfs_ioctl_balance_args {
	__u64 flags;				/* in/out */
	__u64 state;				/* out */

	struct btrfs_balance_args data;		/* in/out */
	struct btrfs_balance_args meta;		/* in/out */
	struct btrfs_balance_args sys;		/* in/out */

	struct btrfs_balance_progress stat;	/* out */

	__u64 unused[72];			/* pad to 1k */
};

struct btrfs_ioctl_search_key {
	/* which root are we searching.  0 is the tree of tree roots */
	__u64 tree_id;

	/* keys returned will be >= min and <= max */
	__u64 min_objectid;
	__u64 max_objectid;

	/* keys returned will be >= min and <= max */
	__u64 min_offset;
	__u64 max_offset;

	/* max and min transids to search for */
	__u64 min_transid;
	__u64 max_transid;

	/* keys returned will be >= min and <= max */
	__u32 min_type;
	__u32 max_type;

	/*
	 * how many items did userland ask for, and how many are we
	 * returning
	 */
	__u32 nr_items;

	/* align to 64 bits */
	__u32 unused;

	/* some extra for later */
	__u64 unused1;
	__u64 unused2;
	__u64 unused3;
	__u64 unused4;
};

struct btrfs_ioctl_search_header {
	__u64 transid;
	__u64 objectid;
	__u64 offset;
	__u32 type;
	__u32 len;
} __attribute__((may_alias));

#define BTRFS_SEARCH_ARGS_BUFSIZE (4096 - sizeof(struct btrfs_ioctl_search_key))
/*
 * the buf is an array of search headers where
 * each header is followed by the actual item
 * the type field is expanded to 32 bits for alignment
 */
struct btrfs_ioctl_search_args {
	struct btrfs_ioctl_search_key key;
	char buf[BTRFS_SEARCH_ARGS_BUFSIZE];
};

#define BTRFS_INO_LOOKUP_PATH_MAX 4080
struct btrfs_ioctl_ino_lookup_args {
	__u64 treeid;
	__u64 objectid;
	char name[BTRFS_INO_LOOKUP_PATH_MAX];
};

/* flags for the defrag range ioctl */
#define BTRFS_DEFRAG_RANGE_COMPRESS 1
#define BTRFS_DEFRAG_RANGE_START_IO 2

struct btrfs_ioctl_defrag_range_args {
	/* start of the defrag operation */
	__u64 start;

	/* number of bytes to defrag, use (u64)-1 to say all */
	__u64 len;

	/*
	 * flags for the operation, which can include turning
	 * on compression for this one defrag
	 */
	__u64 flags;

	/*
	 * any extent bigger than this will be considered
	 * already defragged.  Use 0 to take the kernel default
	 * Use 1 to say every single extent must be rewritten
	 */
	__u32 extent_thresh;

	/*
	 * which compression method to use if turning on compression
	 * for this defrag operation.  If unspecified, zlib will
	 * be used
	 */
	__u32 compress_type;

	/* spare for later */
	__u32 unused[4];
};

struct btrfs_ioctl_space_info {
	__u64 flags;
	__u64 total_bytes;
	__u64 used_bytes;
};

struct btrfs_ioctl_space_args {
	__u64 space_slots;
	__u64 total_spaces;
	struct btrfs_ioctl_space_info spaces[0];
};

struct btrfs_data_container {
	__u32	bytes_left;	/* out -- bytes not needed to deliver output */
	__u32	bytes_missing;	/* out -- additional bytes needed for result */
	__u32	elem_cnt;	/* out */
	__u32	elem_missed;	/* out */
	__u64	val[0];		/* out */
};

struct btrfs_ioctl_ino_path_args {
	__u64				inum;		/* in */
	__u64				size;		/* in */
	__u64				reserved[4];
	/* struct btrfs_data_container	*fspath;	   out */
	__u64				fspath;		/* out */
};

struct btrfs_ioctl_logical_ino_args {
	__u64				logical;	/* in */
	__u64				size;		/* in */
	__u64				reserved[4];
	/* struct btrfs_data_container	*inodes;	out   */
	__u64				inodes;
};

/* BTRFS_IOC_SNAP_CREATE is no longer used by the btrfs command */
#define BTRFS_IOC_SNAP_CREATE _IOW(BTRFS_IOCTL_MAGIC, 1, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_DEFRAG _IOW(BTRFS_IOCTL_MAGIC, 2, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_RESIZE _IOW(BTRFS_IOCTL_MAGIC, 3, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_SCAN_DEV _IOW(BTRFS_IOCTL_MAGIC, 4, \
				   struct btrfs_ioctl_vol_args)
/* trans start and trans end are dangerous, and only for
 * use by applications that know how to avoid the
 * resulting deadlocks
 */
#define BTRFS_IOC_TRANS_START  _IO(BTRFS_IOCTL_MAGIC, 6)
#define BTRFS_IOC_TRANS_END    _IO(BTRFS_IOCTL_MAGIC, 7)
#define BTRFS_IOC_SYNC         _IO(BTRFS_IOCTL_MAGIC, 8)

#define BTRFS_IOC_CLONE        _IOW(BTRFS_IOCTL_MAGIC, 9, int)
#define BTRFS_IOC_ADD_DEV _IOW(BTRFS_IOCTL_MAGIC, 10, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_RM_DEV _IOW(BTRFS_IOCTL_MAGIC, 11, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_BALANCE _IOW(BTRFS_IOCTL_MAGIC, 12, \
				   struct btrfs_ioctl_vol_args)
/* 13 is for CLONE_RANGE */
#define BTRFS_IOC_SUBVOL_CREATE _IOW(BTRFS_IOCTL_MAGIC, 14, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_SNAP_DESTROY _IOW(BTRFS_IOCTL_MAGIC, 15, \
				   struct btrfs_ioctl_vol_args)
#define BTRFS_IOC_DEFRAG_RANGE _IOW(BTRFS_IOCTL_MAGIC, 16, \
				struct btrfs_ioctl_defrag_range_args)
#define BTRFS_IOC_TREE_SEARCH _IOWR(BTRFS_IOCTL_MAGIC, 17, \
				   struct btrfs_ioctl_search_args)
#define BTRFS_IOC_INO_LOOKUP _IOWR(BTRFS_IOCTL_MAGIC, 18, \
				   struct btrfs_ioctl_ino_lookup_args)
#define BTRFS_IOC_DEFAULT_SUBVOL _IOW(BTRFS_IOCTL_MAGIC, 19, u64)
#define BTRFS_IOC_SPACE_INFO _IOWR(BTRFS_IOCTL_MAGIC, 20, \
				    struct btrfs_ioctl_space_args)
#define BTRFS_IOC_SNAP_CREATE_V2 _IOW(BTRFS_IOCTL_MAGIC, 23, \
				   struct btrfs_ioctl_vol_args_v2)
#define BTRFS_IOC_SCRUB _IOWR(BTRFS_IOCTL_MAGIC, 27, \
				struct btrfs_ioctl_scrub_args)
#define BTRFS_IOC_SCRUB_CANCEL _IO(BTRFS_IOCTL_MAGIC, 28)
#define BTRFS_IOC_SCRUB_PROGRESS _IOWR(BTRFS_IOCTL_MAGIC, 29, \
					struct btrfs_ioctl_scrub_args)
#define BTRFS_IOC_DEV_INFO _IOWR(BTRFS_IOCTL_MAGIC, 30, \
					struct btrfs_ioctl_dev_info_args)
#define BTRFS_IOC_FS_INFO _IOR(BTRFS_IOCTL_MAGIC, 31, \
                                 struct btrfs_ioctl_fs_info_args)
#define BTRFS_IOC_BALANCE_V2 _IOWR(BTRFS_IOCTL_MAGIC, 32, \
				   struct btrfs_ioctl_balance_args)
#define BTRFS_IOC_BALANCE_CTL _IOW(BTRFS_IOCTL_MAGIC, 33, int)
#define BTRFS_IOC_BALANCE_PROGRESS _IOR(BTRFS_IOCTL_MAGIC, 34, \
					struct btrfs_ioctl_balance_args)
#define BTRFS_IOC_INO_PATHS _IOWR(BTRFS_IOCTL_MAGIC, 35, \
					struct btrfs_ioctl_ino_path_args)
#define BTRFS_IOC_LOGICAL_INO _IOWR(BTRFS_IOCTL_MAGIC, 36, \
					struct btrfs_ioctl_ino_path_args)

#endif
