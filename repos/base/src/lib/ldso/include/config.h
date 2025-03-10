/*
 * \brief  Linker configuration
 * \author Norman Feske
 * \date   2019-08-13
 */

/*
 * Copyright (C) 2019 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__CONFIG_H_
#define _INCLUDE__CONFIG_H_

#include <util/reconstructible.h>
#include <base/attached_rom_dataspace.h>

namespace Linker { class Config; }


class Linker::Config : Noncopyable
{
	private:

		Attached_rom_dataspace const _config;

		Bind const _bind = _config.xml().attribute_value("ld_bind_now", false)
		                 ? BIND_NOW : BIND_LAZY;

		bool const _verbose     = _config.xml().attribute_value("ld_verbose",     false);
		bool const _check_ctors = _config.xml().attribute_value("ld_check_ctors", true);

	public:

		Config(Env &env) : _config(env, "config") { }

		Bind bind()        const { return _bind; }
		bool verbose()     const { return _verbose; }
		bool check_ctors() const { return _check_ctors; }

		using Rom_name = String<100>;

		/**
		 * Call fn for each library specified in the configuration
		 *
		 * The functor 'fn' is called with 'Rom_name', 'Keep' as arguments.
		 */
		void for_each_library(auto const &fn) const
		{
			_config.xml().with_optional_sub_node("ld", [&] (Xml_node ld) {

				ld.for_each_sub_node("library", [&] (Xml_node lib) {

					Rom_name const rom = lib.attribute_value("rom", Rom_name());

					Keep const keep = lib.attribute_value("keep", false)
					                ? DONT_KEEP : KEEP;

					fn(rom, keep);
				});
			});
		}
};

#endif /* _INCLUDE__CONFIG_H_ */
