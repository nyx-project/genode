/*
 * \brief   Pistachio protection-domain facility
 * \author  Christian Helmuth
 * \date    2006-04-11
 */

/*
 * Copyright (C) 2006-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__INCLUDE__PLATFORM_PD_H_
#define _CORE__INCLUDE__PLATFORM_PD_H_

/* Genode includes */
#include <base/allocator.h>

/* base-internal includes */
#include <base/internal/pistachio.h>

/* core includes */
#include <address_space.h>

namespace Core {

	class Platform_thread;
	class Platform_pd;
}


class Core::Platform_pd : public Address_space
{
	public:

		struct Thread_id { unsigned value; };

		enum class Alloc_thread_id_error { EXHAUSTED };
		using Alloc_thread_id_result = Attempt<Thread_id, Alloc_thread_id_error>;

	private:

		/*
		 * Noncopyable
		 */
		Platform_pd(Platform_pd const &);
		Platform_pd &operator = (Platform_pd const &);

		friend class Platform_thread;

		/*
		 * L4 thread ID has 18 bits for thread number and 14 bits for
		 * version info.
		 */
		enum {
			PD_BITS      = 9,
			THREAD_BITS  = 7,
			VERSION_BITS = 14 - 1, /* preserve 1 bit, see 'make_l4_id' */
			PD_FIRST     = 0,
			PD_MAX       = (1 << PD_BITS)      - 1,
			THREAD_MAX   = (1 << THREAD_BITS)  - 1,
			VERSION_MASK = (1 << VERSION_BITS) - 1,
			VERSION_MAX  = VERSION_MASK,
			PD_INVALID   = -1,
		};

		unsigned _pd_id   = 0;
		unsigned _version = 0;

		Pistachio::L4_ThreadId_t _l4_task_id { };  /* L4 task ID */

		/**
		 * Manually construct L4 thread ID from its components
		 */
		static Pistachio::L4_ThreadId_t make_l4_id(unsigned pd_no,
		                                           unsigned thread_no,
		                                           unsigned version)
		{
			/*
			 * We have to make sure that the 6 lower version bits are
			 * never zero. Otherwise, the kernel won't recognize the
			 * thread ID as a global ID (i.e., 'L4_ThreadControl' would
			 * fail during the creation of a new PD). To maintain this
			 * invariant, we always set the lowest version bit to one.
			 */
			return Pistachio::L4_GlobalId((pd_no << PD_BITS) | thread_no,
			                               (version << 1) | 1);
		}


		/**********************************************
		 ** Threads of this protection domain object **
		 **********************************************/

		Platform_thread *_threads[THREAD_MAX] { };


		/******************
		 ** PD allocator **
		 ******************/

		struct Pd_alloc
		{
			unsigned reserved : 1;
			unsigned free     : 1;
			unsigned version  : VERSION_BITS;

			Pd_alloc(bool r, bool f, unsigned v)
			: reserved(r), free(f), version(v & VERSION_MASK) { }

			/*
			 * Start with version 2 to avoid being mistaken as local or
			 * interrupt thread ID.
			 */
			Pd_alloc() : reserved(0), free(0), version(2) { }
		};

		static Pd_alloc *_pds()
		{
			static Pd_alloc static_pds[PD_MAX];
			return static_pds;
		}

		Pistachio::L4_Word_t _kip_ptr  = 0;
		Pistachio::L4_Word_t _utcb_ptr = 0;

		/**
		 * Protection-domain creation
		 *
		 * The syscall parameter propagates if any L4 kernel function
		 * should be used. We need the special case for the Core startup.
		 */
		void _create_pd(bool syscall);

		/**
		 * Protection domain destruction
		 *
		 * No special case for Core here - we just never call it.
		 */
		void _destroy_pd();

		/**
		 * Protection domain allocation
		 *
		 * Find free PD and use it. We need the special case for core
		 * startup.
		 */
		int _alloc_pd(signed pd_id);

		/**
		 * Protection domain deallocation
		 *
		 * No special case for Core here - we just never call it.
		 */
		void _free_pd();

		/**
		 * Setup KIP and UTCB area
		 */
		void _setup_address_space();

		/**
		 * Return the location of the UTCB for the specified thread
		 */
		Pistachio::L4_Word_t _utcb_location(unsigned int thread_id);


		/***************
		 ** Debugging **
		 ***************/

		void _debug_log_pds(void);
		void _debug_log_threads(void);

	public:

		/**
		 * Constructors
		 */
		Platform_pd(bool core);
		Platform_pd(Allocator &md_alloc, char const *,
		            signed pd_id = PD_INVALID, bool create = true);

		/**
		 * Destructor
		 */
		~Platform_pd();

		/**
		 * Register quota donation at allocator guard
		 */
		void upgrade_ram_quota(size_t) { }

		/**
		 * Allocate PD-local ID for a new 'Platform_thread'
		 */
		Alloc_thread_id_result alloc_thread_id(Platform_thread &);

		/**
		 * Release PD-local thread ID at destruction of 'Platform_thread'
		 */
		void free_thread_id(Thread_id);

		/**
		 * Return L4 thread ID from the PD's task ID and the PD-local thread ID
		 */
		Pistachio::L4_ThreadId_t l4_thread_id(Thread_id const id) const
		{
			return make_l4_id(_pd_id, id.value, _version);
		}

		static Pistachio::L4_Word_t _core_utcb_ptr;
		static void touch_utcb_space();

		int bind_initial_thread(Platform_thread &thread);

		/**
		 * Assign parent interface to protection domain
		 */
		void assign_parent(Native_capability) { }

		int pd_id() const { return _pd_id; }


		/*****************************
		 ** Address-space interface **
		 *****************************/

		void flush(addr_t, size_t, Core_local_addr) override;
};

#endif /* _CORE__INCLUDE__PLATFORM_PD_H_ */
