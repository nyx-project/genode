/*
 * \brief  Core-specific region map for Linux
 * \author Norman Feske
 * \date   2017-05-10
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__INCLUDE__CORE_REGION_MAP_H_
#define _CORE__INCLUDE__CORE_REGION_MAP_H_

/* base-internal includes */
#include <base/internal/region_map_mmap.h>

/* core includes */
#include <types.h>

namespace Core { class Core_region_map; }


struct Core::Core_region_map : Region_map_mmap
{
	static void init(Rpc_entrypoint &);

	Core_region_map(Rpc_entrypoint &ep) : Region_map_mmap(false) { init(ep); }
};

#endif /* _CORE__INCLUDE__CORE_REGION_MAP_H_ */
