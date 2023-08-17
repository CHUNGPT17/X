/* adapter_driver197_global_init_ext.h
 *
 * Linux kernel specific Adapter Global Control extensions
 */

/*****************************************************************************
* Copyright (c) 2012-2022 by Rambus, Inc. and/or its subsidiaries.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Top-level Adapter configuration
#include "c_adapter_global.h"   // ADAPTER_GLOBAL_LICENSE

// Linux Kernel API
#include <linux/init.h>     // module_init, module_exit
#include <linux/module.h>   // EXPORT_SYMBOL


/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

// Global Classification Control Init API
#include "api_global_eip207.h"

// Global Classification Control Status API
#include "api_global_status_eip207.h"

// Global Packet I/O Control Init API
#include "api_global_eip97.h"

// Global Packet I/O Control Status API
#include "api_global_status_eip97.h"

// Global DRBG Control API
#include "api_global_eip74.h"

MODULE_LICENSE(ADAPTER_GLOBAL_LICENSE);

// Global Classification Control Init API
EXPORT_SYMBOL(GlobalControl207_Capabilities_Get);
EXPORT_SYMBOL(GlobalControl207_Init);
EXPORT_SYMBOL(GlobalControl207_UnInit);

// Global Packet I/O Control Status API
EXPORT_SYMBOL(GlobalControl97_Capabilities_Get);
EXPORT_SYMBOL(GlobalControl97_Init);
EXPORT_SYMBOL(GlobalControl97_UnInit);
EXPORT_SYMBOL(GlobalControl97_Configure);

// Global Classification Control Status API
EXPORT_SYMBOL(GlobalControl207_Status_Get);
EXPORT_SYMBOL(GlobalControl207_GlobalStats_Get);
EXPORT_SYMBOL(GlobalControl207_ClockCount_Get);
EXPORT_SYMBOL(GlobalControl207_Firmware_Configure);

// Global Packet I/O Control Status API
EXPORT_SYMBOL(GlobalControl97_Debug_Statistics_Get);
EXPORT_SYMBOL(GlobalControl97_DFE_Status_Get);
EXPORT_SYMBOL(GlobalControl97_DSE_Status_Get);
EXPORT_SYMBOL(GlobalControl97_Token_Status_Get);
EXPORT_SYMBOL(GlobalControl97_Context_Status_Get);
EXPORT_SYMBOL(GlobalControl97_Interrupt_Status_Get);
EXPORT_SYMBOL(GlobalControl97_OutXfer_Status_Get);
EXPORT_SYMBOL(GlobalControl97_PRNG_Status_Get);
EXPORT_SYMBOL(GlobalControl97_PRNG_Reseed);
EXPORT_SYMBOL(GlobalControl97_Interfaces_Get);

// Global DRBG Control API
EXPORT_SYMBOL(GlobalControl74_Init);
EXPORT_SYMBOL(GlobalControl74_UnInit);
EXPORT_SYMBOL(GlobalControl74_Capabilities_Get);
EXPORT_SYMBOL(GlobalControl74_Status_Get);
EXPORT_SYMBOL(GlobalControl74_Reseed);
EXPORT_SYMBOL(GlobalControl74_Clear);
#ifdef ADAPTER_EIP74_INTERRUPTS_ENABLE
EXPORT_SYMBOL(GlobalControl74_Notify_Request);
#endif

module_init(Driver197_Global_Init);
module_exit(Driver197_Global_Exit);

/* end of file adapter_driver197_init_ext.h */
