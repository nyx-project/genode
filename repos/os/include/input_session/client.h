/*
 * \brief  Client-side input session interface
 * \author Norman Feske
 * \date   2006-08-16
 */

/*
 * Copyright (C) 2006-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__INPUT_SESSION__CLIENT_H_
#define _INCLUDE__INPUT_SESSION__CLIENT_H_

#include <input_session/capability.h>
#include <input/event.h>
#include <base/attached_dataspace.h>
#include <base/rpc_client.h>

namespace Input { struct Session_client; }


class Input::Session_client : public Genode::Rpc_client<Session>
{
	private:

		Genode::Attached_dataspace _event_ds;

		Genode::size_t const _max_events =
			_event_ds.size() / sizeof(Input::Event);

		friend class Input::Binding;

	public:

		Session_client(Genode::Region_map &local_rm, Session_capability session)
		:
			Genode::Rpc_client<Session>(session),
			_event_ds(local_rm, call<Rpc_dataspace>())
		{ }

		Genode::Dataspace_capability dataspace() override {
			return call<Rpc_dataspace>(); }

		bool pending() const override {
			return call<Rpc_pending>(); }

		int flush() override {
			return call<Rpc_flush>(); }

		void sigh(Genode::Signal_context_capability sigh) override {
			call<Rpc_sigh>(sigh); }

		/**
		 * Flush and apply functor to pending events
		 *
		 * \param fn    functor in the form of f(Event const &e)
		 * \return      number of events processed
		 */
		void for_each_event(auto const &fn)
		{
			Genode::size_t const n = Genode::min((Genode::size_t)call<Rpc_flush>(), _max_events);

			Event const *ev_buf = _event_ds.local_addr<const Event>();
			for (Genode::size_t i = 0; i < n; ++i)
				fn(ev_buf[i]);
		}
};

#endif /* _INCLUDE__INPUT_SESSION__CLIENT_H_ */
