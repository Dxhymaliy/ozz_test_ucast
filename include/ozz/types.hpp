#pragma once

#include "platform.hpp"
#include "logdeff.hpp"


namespace OZZ_CORE{

	constexpr int32_t gen_numbers{1'000'000};  
	
    enum class msg_type: uint16_t
    {
        ask_msg, answer_msg, recovery_ask, auth, confirm, error_msg, undef = 255
    };

    enum class msg_state: uint16_t
    {
        beg, in, fin, undef = 255
    };

	/**
	 * Parent for non-copyable instances
	 * @details Usage: class someclass : noncopyable ...
	 */
	struct noncopyable
	{
	    /// Assignment operator was disabled
	    noncopyable& operator=(noncopyable const&) = delete;

	    /// Copy constructor disabled
	    noncopyable(noncopyable const&) = delete;

        protected:
			noncopyable() = default;    
	};

	struct nonmovable:
	        noncopyable
	{
	    nonmovable(nonmovable&&) = delete;
	    nonmovable & operator=(noncopyable&&) = delete;
	};
}

