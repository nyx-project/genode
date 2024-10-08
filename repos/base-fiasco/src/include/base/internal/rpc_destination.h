/*
 * \brief  RPC destination type
 * \author Norman Feske
 * \date   2016-03-11
 */

/*
 * Copyright (C) 2016-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__BASE__INTERNAL__RPC_DESTINATION_H_
#define _INCLUDE__BASE__INTERNAL__RPC_DESTINATION_H_

/* L4/Fiasco includes */
#include <fiasco/syscall.h>

namespace Genode {

	using Rpc_destination = Fiasco::l4_threadid_t;

	static inline Rpc_destination invalid_rpc_destination()
	{
		using namespace Fiasco;
		return L4_INVALID_ID;
	}

	static void print(Output &out, Rpc_destination const &dst)
	{
		Genode::print(out, "thread=", dst.id.task, ".", dst.id.lthread);
	}
}

#endif /* _INCLUDE__BASE__INTERNAL__RPC_DESTINATION_H_ */
