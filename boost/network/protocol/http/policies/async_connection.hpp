#ifndef BOOST_NETWORK_POLICY_ASYNC_CONNECTION_HPP_20100529
#define BOOST_NETWORK_POLICY_ASYNC_CONNECTION_HPP_20100529

// Copyright 2010 (C) Dean Michael Berris
// Copyright 2010 (C) Sinefunc, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/network/version.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#include <boost/network/protocol/http/traits/resolver_policy.hpp>
#include <boost/network/protocol/http/detail/connection_helper.hpp>
#include <boost/network/protocol/http/impl/async_connection_base.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace boost { namespace network { namespace http {

    template <class Tag, unsigned version_major, unsigned version_minor>
    struct async_connection_policy : resolver_policy<Tag>::type {
    protected:

        typedef typename string<Tag>::type string_type;
        typedef typename resolver_policy<Tag>::type resolver_base;
        typedef typename resolver_base::resolver_type resolver_type;
        typedef typename resolver_base::resolve_function resolve_function;
        
        struct connection_impl {
            connection_impl(
                bool follow_redirect, 
                resolve_function resolve, 
                boost::shared_ptr<resolver_type> resolver,
                bool https
                )
                : pimpl(
                    impl::async_connection_base<Tag,version_major,version_minor>::new_connection(resolve, resolver, follow_redirect, https)
                )
            {}

            basic_response<Tag> send_request(string_type const & method, basic_request<Tag> const & request_, bool get_body) {
                return pimpl->start(request_, method, get_body);
            }

        private:

            shared_ptr<http::impl::async_connection_base<Tag, version_major, version_minor> > pimpl;

        };

        typedef boost::shared_ptr<connection_impl> connection_ptr;
        connection_ptr get_connection(boost::shared_ptr<resolver_type> resolver, basic_request<Tag> const & request_) {
            string_type protocol_ = protocol(request_);
            connection_ptr connection_(
                new connection_impl(
                    follow_redirect_
                    , boost::bind(
                        &async_connection_policy<Tag, version_major, version_minor>::resolve,
                        this,
                        _1, _2, _3
                        )
                    , resolver                    
                    , boost::iequals(protocol_, string_type("https"))
                    )
                    );
            return connection_;
        }

        void cleanup() { }

        async_connection_policy(bool cache_resolved, bool follow_redirect)
            : resolver_base(cache_resolved), follow_redirect_(follow_redirect) {}

        bool follow_redirect_;
    };

} // namespace http

} // namespace network

} // namespace boost

#endif // BOOST_NETWORK_POLICY_ASYNC_CONNECTION_HPP_
