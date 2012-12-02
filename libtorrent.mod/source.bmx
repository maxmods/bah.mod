' Copyright (c) 2010-2012 Bruce A Henderson
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
Import "../libiconv.mod/include/*.h"
Import "../crypto.mod/ssl/include/*.h"
?

Import "include/*.h"
Import "include/libtorrent/*.h"

Import "src/GeoIP.c"
Import "src/mpi.c"
Import "src/alert.cpp"
Import "src/allocator.cpp"
Import "src/asio_ssl.cpp"
Import "src/asio.cpp"
Import "src/assert.cpp"
Import "src/bandwidth_limit.cpp"
Import "src/bandwidth_manager.cpp"
Import "src/bandwidth_queue_entry.cpp"
Import "src/bloom_filter.cpp"
Import "src/broadcast_socket.cpp"
Import "src/bt_peer_connection.cpp"
Import "src/chained_buffer.cpp"
Import "src/connection_queue.cpp"
Import "src/ConvertUTF.cpp"
Import "src/create_torrent.cpp"
Import "src/disk_buffer_holder.cpp"
Import "src/disk_buffer_pool.cpp"
Import "src/disk_io_thread.cpp"
Import "src/entry.cpp"
Import "src/enum_net.cpp"
Import "src/error_code.cpp"
Import "src/escape_string.cpp"
Import "src/file_pool.cpp"
Import "src/file_storage.cpp"
Import "src/file.cpp"
Import "src/gzip.cpp"
Import "src/http_connection.cpp"
Import "src/http_parser.cpp"
Import "src/http_seed_connection.cpp"
Import "src/http_stream.cpp"
Import "src/http_tracker_connection.cpp"
Import "src/i2p_stream.cpp"
Import "src/identify_client.cpp"
Import "src/instantiate_connection.cpp"
Import "src/ip_filter.cpp"
Import "src/lazy_bdecode.cpp"
Import "src/logger.cpp"
Import "src/lsd.cpp"
Import "src/lt_trackers.cpp"
Import "src/magnet_uri.cpp"
Import "src/metadata_transfer.cpp"
Import "src/natpmp.cpp"
Import "src/packet_buffer.cpp"
Import "src/parse_url.cpp"
Import "src/pe_crypto.cpp"
Import "src/peer_connection.cpp"
Import "src/piece_picker.cpp"
Import "src/policy.cpp"
Import "src/puff.cpp"
Import "src/random.cpp"
Import "src/rsa.cpp"
Import "src/rss.cpp"
Import "src/session_impl.cpp"
Import "src/session.cpp"
Import "src/settings.cpp"
Import "src/sha1.cpp"
Import "src/smart_ban.cpp"
Import "src/socket_io.cpp"
Import "src/socket_type.cpp"
Import "src/socks5_stream.cpp"
Import "src/stat.cpp"
Import "src/storage.cpp"
Import "src/string_util.cpp"
Import "src/bmx_thread.cpp"
Import "src/time.cpp"
Import "src/timestamp_history.cpp"
Import "src/torrent_handle.cpp"
Import "src/torrent_info.cpp"
Import "src/torrent.cpp"
Import "src/tracker_manager.cpp"
Import "src/udp_socket.cpp"
Import "src/udp_tracker_connection.cpp"
Import "src/upnp.cpp"
Import "src/ut_metadata.cpp"
Import "src/ut_pex.cpp"
Import "src/utf8.cpp"
Import "src/utp_socket_manager.cpp"
Import "src/utp_stream.cpp"
Import "src/web_connection_base.cpp"
Import "src/web_peer_connection.cpp"
'Import "src/kademlia/closest_nodes.cpp"
Import "src/kademlia/dht_tracker.cpp"
Import "src/kademlia/find_data.cpp"
Import "src/kademlia/node.cpp"
Import "src/kademlia/node_id.cpp"
Import "src/kademlia/refresh.cpp"
Import "src/kademlia/routing_table.cpp"
Import "src/kademlia/rpc_manager.cpp"
Import "src/kademlia/traversal_algorithm.cpp"

Import "*.h"
Import "glue.cpp"

?Not win32
Import "../boost.mod/src/libs/thread/src/pthread/thread.cpp"
Import "../boost.mod/src/libs/thread/src/pthread/once.cpp"
?win32
Import "../boost.mod/src/libs/thread/src/win32/thread.cpp"
?

'Import "../boost.mod/src/libs/filesystem2/src/v2_operations.cpp"
'Import "../boost.mod/src/libs/filesystem2/src/v2_path.cpp"
'Import "../boost.Mod/src/libs/filesystem2/src/v2_portability.cpp"


