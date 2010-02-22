' Copyright (c) 2010 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BRL.Blitz
Import Pub.Zlib
Import BaH.Boost
?win32
Import BaH.Crypto
?

Import "../../pub.mod/zlib.mod/*.h"
Import "../boost.mod/src/*.hpp"

?win32
Import "../crypto.mod/ssl/include/*.h"
?

Import "include/*.h"
Import "include/libtorrent/*.h"

Import "src/entry.cpp"
Import "src/escape_string.cpp"
Import "src/lazy_bdecode.cpp"
Import "src/assert.cpp"
Import "src/enum_net.cpp"
Import "src/broadcast_socket.cpp"
Import "src/peer_connection.cpp"
Import "src/bt_peer_connection.cpp"
Import "src/web_peer_connection.cpp"
Import "src/natpmp.cpp"
Import "src/piece_picker.cpp"
Import "src/policy.cpp"
Import "src/session.cpp"
Import "src/session_impl.cpp"
Import "src/sha1.cpp"
Import "src/stat.cpp"
Import "src/storage.cpp"
Import "src/torrent.cpp"
Import "src/torrent_handle.cpp"
Import "src/pe_crypto.cpp"
Import "src/torrent_info.cpp"
Import "src/tracker_manager.cpp"
Import "src/http_connection.cpp"
Import "src/http_tracker_connection.cpp"
Import "src/udp_tracker_connection.cpp"
Import "src/alert.cpp"
Import "src/identify_client.cpp"
Import "src/ip_filter.cpp"
Import "src/file.cpp"
Import "src/metadata_transfer.cpp"
Import "src/logger.cpp"
Import "src/file_pool.cpp"
Import "src/ut_pex.cpp"
Import "src/lsd.cpp"
Import "src/upnp.cpp"
Import "src/instantiate_connection.cpp"
Import "src/socks5_stream.cpp"
Import "src/socks4_stream.cpp"
Import "src/http_stream.cpp"
Import "src/connection_queue.cpp"
Import "src/disk_io_thread.cpp"
Import "src/ut_metadata.cpp"
Import "src/magnet_uri.cpp"
Import "src/udp_socket.cpp"
Import "src/smart_ban.cpp"
Import "src/http_parser.cpp"
Import "src/gzip.cpp"
Import "src/disk_buffer_holder.cpp"
Import "src/create_torrent.cpp"
Import "src/parse_url.cpp"
Import "src/file_storage.cpp"
Import "src/error_code.cpp"
Import "src/kademlia/closest_nodes.cpp"
Import "src/kademlia/dht_tracker.cpp"
Import "src/kademlia/find_data.cpp"
Import "src/kademlia/node.cpp"
Import "src/kademlia/node_id.cpp"
Import "src/kademlia/refresh.cpp"
Import "src/kademlia/routing_table.cpp"
Import "src/kademlia/rpc_manager.cpp"
Import "src/kademlia/traversal_algorithm.cpp"
Import "src/GeoIP.c"


Import "*.h"
Import "glue.cpp"

?Not win32
Import "../boost.mod/src/libs/thread/src/pthread/exceptions.cpp"
Import "../boost.mod/src/libs/thread/src/pthread/thread.cpp"
Import "../boost.mod/src/libs/thread/src/pthread/once.cpp"
?win32
Import "../boost.mod/src/libs/thread/src/win32/exceptions.cpp"
Import "../boost.mod/src/libs/thread/src/win32/thread.cpp"
?

Import "../boost.mod/src/libs/filesystem/src/operations.cpp"
Import "../boost.mod/src/libs/filesystem/src/path.cpp"
Import "../boost.mod/src/libs/filesystem/src/portability.cpp"

