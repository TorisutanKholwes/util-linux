/*
 * Copyright (C) 2025 OpenSVC, Inc.
 *
 * This file may be redistributed under the terms of the
 * GNU Lesser General Public License.
*/

#include <inttypes.h>

#include "superblocks.h"
#include "crc32c.h"

#define OSVCFS_MAGIC_STRING "\x3d\xc1\x3c\x87\xc0\x5b\xe3\xb6"
#define OSVCFS_MAGIC_LEN    (sizeof(OSVCFS_MAGIC_STRING) - 1)

#define OSVCFS_UUID_BYTES 16

struct osvcfs_super_block {
    uint64_t magic;
    uint32_t version;
    uint32_t block_size;
    uint32_t slot_size;
    uint8_t uuid[OSVCFS_UUID_BYTES];
};

static int probe_osvcfs(blkid_probe pr, const struct blkid_idmag *mag) {

    const struct osvcfs_super_block *sb;

    sb = blkid_probe_get_sb(pr, mag, struct osvcfs_super_block);
    if (!sb)
        return errno ? -errno : 1;

    blkid_probe_sprintf_version(pr, "%"PRIu32, le32_to_cpu(sb->version));
    blkid_probe_set_uuid(pr, sb->uuid);
    blkid_probe_set_fsblocksize(pr, le32_to_cpu(sb->block_size));
    blkid_probe_set_block_size(pr, le32_to_cpu(sb->block_size));

    return BLKID_PROBE_OK;
}

const struct blkid_idinfo osvcfs_idinfo = {
    .name =  "osvcfs",
    .usage = BLKID_USAGE_FILESYSTEM,
    .probefunc = probe_osvcfs,
    .minsz = 1024 * 1024,
    .magics = {
        {
            .magic = OSVCFS_MAGIC_STRING,
            .len = OSVCFS_MAGIC_LEN,
            .kboff = 0,
            .sboff = 0,
        },
        { NULL }
    }
};