/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015-2017, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <bl_common.h>		/* For ARRAY_SIZE */
#include <debug.h>
#include <io_driver.h>
#include <io_storage.h>
#include <io_semihosting.h>
#include <platform_def.h>
#include <string.h>
#include "io_common.h"
#include "io_rcar.h"
#include "io_memdrv.h"
#include "io_emmcdrv.h"
#include "io_private.h"

/* IO devices */
static uintptr_t rcar_dev_handle;
static uintptr_t memdrv_dev_handle;
static uintptr_t emmcdrv_dev_handle;
static uintptr_t boot_io_drv_id;


static const io_block_spec_t rcar_block_spec = {
	.offset = FLASH0_BASE,
	.length = FLASH0_SIZE
};

static const io_block_spec_t bl2_file_spec = {
	.offset = BL2_IMAGE_ID,
};

static const io_block_spec_t bl31_file_spec = {
	.offset = BL31_IMAGE_ID,
};

static const io_block_spec_t bl32_file_spec = {
	.offset = BL32_IMAGE_ID,
};

static const io_block_spec_t bl33_file_spec = {
	.offset = BL33_IMAGE_ID,
};

static const io_block_spec_t bl332_file_spec = {
	.offset = BL332_IMAGE_ID,
};

static const io_block_spec_t bl333_file_spec = {
	.offset = BL333_IMAGE_ID,
};

static const io_block_spec_t bl334_file_spec = {
	.offset = BL334_IMAGE_ID,
};

static const io_block_spec_t bl335_file_spec = {
	.offset = BL335_IMAGE_ID,
};

static const io_block_spec_t bl336_file_spec = {
	.offset = BL336_IMAGE_ID,
};

static const io_block_spec_t bl337_file_spec = {
	.offset = BL337_IMAGE_ID,
};

static const io_block_spec_t bl338_file_spec = {
	.offset = BL338_IMAGE_ID,
};


#if TRUSTED_BOARD_BOOT
static const io_block_spec_t trusted_key_cert_file_spec = {
	.offset = TRUSTED_KEY_CERT_ID,
};

static const io_block_spec_t bl31_key_cert_file_spec = {
	.offset = SOC_FW_KEY_CERT_ID,
};

static const io_block_spec_t bl32_key_cert_file_spec = {
	.offset = TRUSTED_OS_FW_KEY_CERT_ID,
};

static const io_block_spec_t bl33_key_cert_file_spec = {
	.offset = NON_TRUSTED_FW_KEY_CERT_ID,
};

static const io_block_spec_t bl332_key_cert_file_spec = {
	.offset = BL332_KEY_CERT_ID,
};

static const io_block_spec_t bl333_key_cert_file_spec = {
	.offset = BL333_KEY_CERT_ID,
};

static const io_block_spec_t bl334_key_cert_file_spec = {
	.offset = BL334_KEY_CERT_ID,
};

static const io_block_spec_t bl335_key_cert_file_spec = {
	.offset = BL335_KEY_CERT_ID,
};

static const io_block_spec_t bl336_key_cert_file_spec = {
	.offset = BL336_KEY_CERT_ID,
};

static const io_block_spec_t bl337_key_cert_file_spec = {
	.offset = BL337_KEY_CERT_ID,
};

static const io_block_spec_t bl338_key_cert_file_spec = {
	.offset = BL338_KEY_CERT_ID,
};

static const io_block_spec_t bl31_cert_file_spec = {
	.offset = SOC_FW_CONTENT_CERT_ID,
};

static const io_block_spec_t bl32_cert_file_spec = {
	.offset = TRUSTED_OS_FW_CONTENT_CERT_ID,
};

static const io_block_spec_t bl33_cert_file_spec = {
	.offset = NON_TRUSTED_FW_CONTENT_CERT_ID,
};

static const io_block_spec_t bl332_cert_file_spec = {
	.offset = BL332_CERT_ID,
};

static const io_block_spec_t bl333_cert_file_spec = {
	.offset = BL333_CERT_ID,
};

static const io_block_spec_t bl334_cert_file_spec = {
	.offset = BL334_CERT_ID,
};

static const io_block_spec_t bl335_cert_file_spec = {
	.offset = BL335_CERT_ID,
};

static const io_block_spec_t bl336_cert_file_spec = {
	.offset = BL336_CERT_ID,
};

static const io_block_spec_t bl337_cert_file_spec = {
	.offset = BL337_CERT_ID,
};

static const io_block_spec_t bl338_cert_file_spec = {
	.offset = BL338_CERT_ID,
};
#endif /* TRUSTED_BOARD_BOOT */

static int32_t open_rcar(const uintptr_t spec);
static int32_t open_memmap(const uintptr_t spec);
static int32_t open_emmcdrv(const uintptr_t spec);

struct plat_io_policy {
	uintptr_t *dev_handle;
	uintptr_t image_spec;
	int32_t (*check)(const uintptr_t spec);
};

static const struct plat_io_policy policies[] = {
	[FIP_IMAGE_ID] = {
		&memdrv_dev_handle,
		(uintptr_t)&rcar_block_spec,
		&open_memmap
	},
	[BL2_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl2_file_spec,
		&open_rcar
	},
	[BL31_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl31_file_spec,
		&open_rcar
	},
	[BL32_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl32_file_spec,
		&open_rcar
	},
	[BL33_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl33_file_spec,
		&open_rcar
	},
	[BL332_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl332_file_spec,
		&open_rcar
	},
	[BL333_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl333_file_spec,
		&open_rcar
	},
	[BL334_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl334_file_spec,
		&open_rcar
	},
	[BL335_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl335_file_spec,
		&open_rcar
	},
	[BL336_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl336_file_spec,
		&open_rcar
	},
	[BL337_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl337_file_spec,
		&open_rcar
	},
	[BL338_IMAGE_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl338_file_spec,
		&open_rcar
	},
#if TRUSTED_BOARD_BOOT
	[TRUSTED_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&trusted_key_cert_file_spec,
		&open_rcar
	},
	[SOC_FW_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl31_key_cert_file_spec,
		&open_rcar
	},
	[TRUSTED_OS_FW_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl32_key_cert_file_spec,
		&open_rcar
	},
	[NON_TRUSTED_FW_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl33_key_cert_file_spec,
		&open_rcar
	},
	[BL332_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl332_key_cert_file_spec,
		&open_rcar
	},
	[BL333_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl333_key_cert_file_spec,
		&open_rcar
	},
	[BL334_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl334_key_cert_file_spec,
		&open_rcar
	},
	[BL335_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl335_key_cert_file_spec,
		&open_rcar
	},
	[BL336_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl336_key_cert_file_spec,
		&open_rcar
	},
	[BL337_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl337_key_cert_file_spec,
		&open_rcar
	},
	[BL338_KEY_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl338_key_cert_file_spec,
		&open_rcar
	},
	[SOC_FW_CONTENT_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl31_cert_file_spec,
		&open_rcar
	},
	[TRUSTED_OS_FW_CONTENT_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl32_cert_file_spec,
		&open_rcar
	},
	[NON_TRUSTED_FW_CONTENT_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl33_cert_file_spec,
		&open_rcar
	},
	[BL332_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl332_cert_file_spec,
		&open_rcar
	},
	[BL333_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl333_cert_file_spec,
		&open_rcar
	},
	[BL334_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl334_cert_file_spec,
		&open_rcar
	},
	[BL335_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl335_cert_file_spec,
		&open_rcar
	},
	[BL336_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl336_cert_file_spec,
		&open_rcar
	},
	[BL337_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl337_cert_file_spec,
		&open_rcar
	},
	[BL338_CERT_ID] = {
		&rcar_dev_handle,
		(uintptr_t)&bl338_cert_file_spec,
		&open_rcar
	},{
#endif /* TRUSTED_BOARD_BOOT */
		0, 0, 0
	}
};

static io_drv_spec_t io_drv_spec_memdrv = {
	FLASH0_BASE,
	FLASH0_SIZE,
	0U
};

static io_drv_spec_t io_drv_spec_emmcdrv = {
	0U,
	0U,
	0U
};

static struct plat_io_policy drv_policies[] __attribute__ ((section (".data")))= {
	/* FLASH_DEV_ID */
	{
		&memdrv_dev_handle,
		(uintptr_t)&io_drv_spec_memdrv,
		&open_memmap
	},
	/* EMMC_DEV_ID */
	{
		&emmcdrv_dev_handle,
		(uintptr_t)&io_drv_spec_emmcdrv,
		&open_emmcdrv
	}
};

static int32_t open_rcar(const uintptr_t spec)
{
	int32_t result;

	/* See if a Firmware Image Package is available */
	result = io_dev_init(rcar_dev_handle, (uintptr_t)boot_io_drv_id);

	return result;
}


static int32_t open_memmap(const uintptr_t spec)
{
	int32_t result;
	uintptr_t local_image_handle;

	result = io_dev_init(memdrv_dev_handle, 0U);
	if (result == IO_SUCCESS) {
		result = io_open(memdrv_dev_handle, spec, &local_image_handle);
		if (result == IO_SUCCESS) {
			(void)io_close(local_image_handle);
		}
	}
	return result;
}

static int32_t open_emmcdrv(const uintptr_t spec)
{
	int32_t result;

	result = io_dev_init(emmcdrv_dev_handle, 0U);

	return result;
}


void rcar_io_setup (void)
{
	int32_t io_result;
	const io_dev_connector_t *rcar_dev_con;
	const io_dev_connector_t *memmap_dev_con;

	boot_io_drv_id = FLASH_DEV_ID;

	/* Register the IO devices on this platform */
	io_result = register_io_dev_rcar(&rcar_dev_con);

	io_result = register_io_dev_memdrv(&memmap_dev_con);

	/* Open connections to devices and cache the handles */
	io_result = io_dev_open(rcar_dev_con, 0U, &rcar_dev_handle);

	io_result = io_dev_open(memmap_dev_con, 0U,
				&memdrv_dev_handle);

	/* Ignore improbable errors in release builds */
	(void)io_result;
}

void rcar_io_emmc_setup (void)
{
	int32_t io_result;
	const io_dev_connector_t *rcar_dev_con;
	const io_dev_connector_t *emmc_dev_con;

	boot_io_drv_id = EMMC_DEV_ID;

	/* Register the IO devices on this platform */
	io_result = register_io_dev_rcar(&rcar_dev_con);

	io_result = register_io_dev_emmcdrv(&emmc_dev_con);

	/* Open connections to devices and cache the handles */
	io_result = io_dev_open(rcar_dev_con, 0U, &rcar_dev_handle);

	io_result = io_dev_open(emmc_dev_con, 0U,
				&emmcdrv_dev_handle);

	/* Ignore improbable errors in release builds */
	(void)io_result;
}

/* Return an IO device handle and specification which can be used to access
 * an image. Use this to enforce platform load policy */
int plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
			  uintptr_t *image_spec)
{
	int result = IO_FAIL;
	const struct plat_io_policy *policy;

	policy = &policies[image_id];
	result = policy->check(policy->image_spec);
	if (result == IO_SUCCESS) {
		*image_spec = policy->image_spec;
		*dev_handle = *(policy->dev_handle);
	}

	return result;
}

int32_t plat_get_drv_source(uint32_t io_drv_id, uintptr_t *dev_handle,
			  uintptr_t *image_spec)
{
	int32_t result;
	const struct plat_io_policy *policy;

	policy = &drv_policies[io_drv_id];
	result = policy->check(policy->image_spec);
	if (result == IO_SUCCESS) {
		*image_spec = policy->image_spec;
		*dev_handle = *(policy->dev_handle);
	}

	return result;
}
