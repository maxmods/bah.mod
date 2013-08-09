// builds all boost.asio SSL source as a separate compilation unit
#include <boost/version.hpp>

// BaH
#ifndef SSL_CTX_clear_options
#define SSL_CTRL_CLEAR_OPTIONS			77
#define SSL_CTX_clear_options(ctx,op) \
	SSL_CTX_ctrl((ctx),SSL_CTRL_CLEAR_OPTIONS,(op),NULL)
#endif

#if BOOST_VERSION >= 104610
#include <boost/asio/ssl/impl/src.hpp>
#endif

