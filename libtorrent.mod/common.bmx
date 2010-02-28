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

?linux
Import "-ldl"
Import "-lidn"
Import "-lssl"
Import "-lcrypto"
Import "-lrt"
?win32
Import "-ladvapi32"
Import "-leay32"
Import "-lssleay32"
Import "-lws2_32"
?macos
Import "-lssl"
Import "-lcrypto"
?

Import "source.bmx"


Extern

	Function bmx_LIBTORRENT_VERSION:String()
	Function bmx_LIBTORRENT_VERSION_parts(major:Int Ptr, minor:Int Ptr, tiny:Int Ptr)

	Function bmx_torrent_session_create:Byte Ptr()
	Function bmx_torrent_session_free(handle:Byte Ptr)
	Function bmx_torrent_session_listenOn:Int(handle:Byte Ptr, portFrom:Int, portTo:Int, interface:String)
	Function bmx_torrent_session_status:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_session_is_listening:Int(handle:Byte Ptr)
	Function bmx_torrent_session_pause(handle:Byte Ptr)
	Function bmx_torrent_session_resume(handle:Byte Ptr)
	Function bmx_torrent_session_is_paused:Int(handle:Byte Ptr)
	Function bmx_torrent_session_set_upload_rate_limit(handle:Byte Ptr, bytesPerSecond:Int)
	Function bmx_torrent_session_set_download_rate_limit(handle:Byte Ptr, bytesPerSecond:Int)
	Function bmx_torrent_session_upload_rate_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_session_download_rate_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_session_set_max_uploads(handle:Byte Ptr, limit:Int)
	Function bmx_torrent_session_set_max_connections(handle:Byte Ptr, limit:Int)
	Function bmx_torrent_session_num_uploads:Int(handle:Byte Ptr)
	Function bmx_torrent_session_num_connections:Int(handle:Byte Ptr)
	Function bmx_torrent_session_add_torrent:Byte Ptr(handle:Byte Ptr, params:Byte Ptr)
	Function bmx_torrent_session_set_settings(handle:Byte Ptr, settings:Byte Ptr)
	Function bmx_torrent_session_pop_alert:Object(handle:Byte Ptr)
	Function bmx_torrent_session_start_upnp:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_session_stop_upnp(handle:Byte Ptr)
	Function bmx_torrent_session_start_natpmp:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_session_stop_natpmp(handle:Byte Ptr)
	Function bmx_torrent_session_start_lsd(handle:Byte Ptr)
	Function bmx_torrent_session_stop_lsd(handle:Byte Ptr)
	Function bmx_torrent_session_set_max_half_open_connections(handle:Byte Ptr, limit:Int)
	Function bmx_torrent_session_max_half_open_connections:Int(handle:Byte Ptr)
	Function bmx_torrent_session_get_cache_status:Byte Ptr(handle:Byte Ptr)

	Function bmx_torrent_sessionstatus_free(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_has_incoming_connections:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_upload_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_download_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_payload_upload_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_payload_download_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_total_download(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_sessionstatus_total_upload(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_sessionstatus_total_redundant_bytes(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_sessionstatus_total_failed_bytes(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_sessionstatus_total_payload_download(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_sessionstatus_total_payload_upload(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_sessionstatus_num_peers:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_num_unchoked:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_allowed_upload_slots:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_dht_nodes:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_dht_node_cache:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_dht_torrents:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionstatus_dht_global_nodes:Int(handle:Byte Ptr)

	Function bmx_torrent_addtorrentparams_create:Byte Ptr()
	Function bmx_torrent_addtorrentparams_free(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_save_path(handle:Byte Ptr, path:String)
	Function bmx_torrent_addtorrentparams_set_torrent_info(handle:Byte Ptr, info:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_tracker_url(handle:Byte Ptr, urlPtr:Byte Ptr)
	Function bmx_torrent_addtorrentparams_tracker_url:String(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_name(handle:Byte Ptr, namePtr:Byte Ptr)
	Function bmx_torrent_addtorrentparams_name:String(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_save_path:String(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_storage_mode(handle:Byte Ptr, _mode:Int)
	Function bmx_torrent_addtorrentparams_storage_mode:Int(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_paused(handle:Byte Ptr, value:Int)
	Function bmx_torrent_addtorrentparams_paused:Int(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_auto_managed(handle:Byte Ptr, value:Int)
	Function bmx_torrent_addtorrentparams_auto_managed:Int(handle:Byte Ptr)
	Function bmx_torrent_addtorrentparams_set_duplicate_is_error(handle:Byte Ptr, value:Int)
	Function bmx_torrent_addtorrentparams_duplicate_is_error:Int(handle:Byte Ptr)

	Function bmx_torrent_torrentinfo_load:Byte Ptr(path:String)
	Function bmx_torrent_torrentinfo_free(handle:Byte Ptr)

	Function bmx_torrent_torrentstatus_free(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_state:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_paused:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_progress:Float(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_error:String(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_current_tracker:String(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_total_upload(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_download(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_payload_download(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_payload_upload(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_failed_bytes(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_redundant_bytes(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_download_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_upload_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_download_payload_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_upload_payload_rate:Float(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_num_peers:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_num_complete:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_num_incomplete:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_list_seeds:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_list_peers:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_connect_candidates:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_total_done(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_wanted_done(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_total_wanted(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_num_seeds:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_distributed_copies:Float(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_block_size:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_num_uploads:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_num_connections:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_uploads_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_connections_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_up_bandwidth_queue:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_down_bandwidth_queue:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_all_time_upload(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_all_time_download(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_torrentstatus_active_time:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_seeding_time:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_seed_rank:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_last_scrape:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_has_incoming:Int(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_pieces:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_torrentstatus_num_pieces:Int(handle:Byte Ptr)

	Function bmx_torrent_sessionsettings_create:Byte Ptr()
	Function bmx_torrent_sessionsettings_free(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_user_agent:String(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_user_agent(handle:Byte Ptr, value:String)
	Function bmx_torrent_sessionsettings_tracker_completion_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_tracker_completion_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_tracker_receive_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_tracker_receive_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_stop_tracker_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_stop_tracker_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_tracker_maximum_response_length:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_tracker_maximum_response_length(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_piece_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_piece_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_request_queue_time:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_request_queue_time(handle:Byte Ptr, value:Float)
	Function bmx_torrent_sessionsettings_max_allowed_in_request_queue:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_max_allowed_in_request_queue(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_max_out_request_queue:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_max_out_request_queue(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_whole_pieces_threshold:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_whole_pieces_threshold(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_peer_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_peer_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_urlseed_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_urlseed_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_urlseed_pipeline_size:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_urlseed_pipeline_size(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_file_pool_size:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_file_pool_size(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_allow_multiple_connections_per_ip:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_allow_multiple_connections_per_ip(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_max_failcount:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_max_failcount(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_min_reconnect_time:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_min_reconnect_time(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_peer_connect_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_peer_connect_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_ignore_limits_on_local_network:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_ignore_limits_on_local_network(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_connection_speed:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_connection_speed(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_send_redundant_have:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_send_redundant_have(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_lazy_bitfields:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_lazy_bitfields(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_inactivity_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_inactivity_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_unchoke_interval:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_unchoke_interval(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_optimistic_unchoke_multiplier:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_optimistic_unchoke_multiplier(handle:Byte Ptr, value:Int)
	'Function bmx_torrent_sessionsettings_announce_ip:address(handle:Byte Ptr)
	'Function bmx_torrent_sessionsettings_set_announce_ip(handle:Byte Ptr, value:address)
	Function bmx_torrent_sessionsettings_num_want:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_num_want(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_initial_picker_threshold:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_initial_picker_threshold(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_allowed_fast_set_size:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_allowed_fast_set_size(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_max_outstanding_disk_bytes_per_connection:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_max_outstanding_disk_bytes_per_connection(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_handshake_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_handshake_timeout(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_use_dht_as_fallback:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_use_dht_as_fallback(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_free_torrent_hashes:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_free_torrent_hashes(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_upnp_ignore_nonrouters:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_upnp_ignore_nonrouters(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_send_buffer_watermark:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_send_buffer_watermark(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_auto_upload_slots:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_auto_upload_slots(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_use_parole_mode:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_use_parole_mode(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_cache_size:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_cache_size(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_cache_expiry:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_cache_expiry(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_peer_tos:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_peer_tos(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_active_downloads:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_active_downloads(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_active_seeds:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_active_seeds(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_active_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_active_limit(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_dont_count_slow_torrents:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_dont_count_slow_torrents(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_auto_manage_interval:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_auto_manage_interval(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_share_ratio_limit:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_share_ratio_limit(handle:Byte Ptr, value:Float)
	Function bmx_torrent_sessionsettings_seed_time_ratio_limit:Float(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_seed_time_ratio_limit(handle:Byte Ptr, value:Float)
	Function bmx_torrent_sessionsettings_seed_time_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_seed_time_limit(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_close_redundant_connections:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_close_redundant_connections(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_auto_scrape_interval:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_auto_scrape_interval(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_auto_scrape_min_interval:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_auto_scrape_min_interval(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_max_peerlist_size:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_max_peerlist_size(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_min_announce_interval:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_min_announce_interval(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_prioritize_partial_pieces:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_prioritize_partial_pieces(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_auto_manage_startup:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_auto_manage_startup(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_rate_limit_ip_overhead:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_rate_limit_ip_overhead(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_urlseed_wait_retry:Int(handle:Byte Ptr)
	Function bmx_torrent_sessionsettings_set_urlseed_wait_retry(handle:Byte Ptr, value:Int)
	Function bmx_torrent_sessionsettings_outgoing_ports(handle:Byte Ptr, fromPort:Int Ptr, toPort:Int Ptr)
	Function bmx_torrent_sessionsettings_set_outgoing_ports(handle:Byte Ptr, fromPort:Int, toPort:Int)

	Function bmx_torrent_torrenthandle_is_valid:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_status:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_name:String(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_get_torrent_info:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_save_resume_data(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_force_reannounce(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_scrape_tracker(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_set_tracker_login(handle:Byte Ptr, username:String, password:String)
	Function bmx_torrent_torrenthandle_add_url_seed(handle:Byte Ptr, url:String)
	Function bmx_torrent_torrenthandle_remove_url_seed(handle:Byte Ptr, url:String)
	Function bmx_torrent_torrenthandle_url_seeds:String[](handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_set_ratio(handle:Byte Ptr, ratio:Float)
	Function bmx_torrent_torrenthandle_set_max_uploads(handle:Byte Ptr, maxUploads:Int)
	Function bmx_torrent_torrenthandle_set_max_connections(handle:Byte Ptr, maxConnections:Int)
	Function bmx_torrent_torrenthandle_set_upload_limit(handle:Byte Ptr, limit:Int)
	Function bmx_torrent_torrenthandle_upload_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_set_download_limit(handle:Byte Ptr, limit:Int)
	Function bmx_torrent_torrenthandle_download_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_set_sequential_download(handle:Byte Ptr, value:Int)
	Function bmx_torrent_torrenthandle_is_sequential_download:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_queue_position:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_queue_position_up(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_queue_position_down(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_queue_position_top(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_queue_position_bottom(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_use_interface(handle:Byte Ptr, netInterface:String)
	Function bmx_torrent_torrenthandle_pause(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_resume(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_is_paused:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_is_seed:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_force_recheck(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_clear_error(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_is_auto_managed:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_auto_managed(handle:Byte Ptr, value:Int)
	Function bmx_torrent_torrenthandle_has_metadata:Int(handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_piece_availability:Int[](handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_set_piece_priority(handle:Byte Ptr, index:Int, priority:Int)
	Function bmx_torrent_torrenthandle_piece_priority:Int(handle:Byte Ptr, index:Int)
	Function bmx_torrent_torrenthandle_prioritize_pieces(handle:Byte Ptr, pieces:Int[])
	Function bmx_torrent_torrenthandle_piece_priorities:Int[](handle:Byte Ptr)

	Function bmx_torrent_bitfield_get_bit:Int(handle:Byte Ptr, index:Int)
	Function bmx_torrent_bitfield_size:Int(handle:Byte Ptr)
	Function bmx_torrent_bitfield_free(handle:Byte Ptr)

	Function bmx_torrent_peerinfo_free(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_flags:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_source:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_up_speed:Float(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_down_speed:Float(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_payload_up_speed:Float(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_payload_down_speed:Float(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_total_download(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_peerinfo_total_upload(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_peerinfo_pieces:Byte Ptr(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_upload_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_download_limit:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_last_request:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_last_active:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_request_timeout:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_send_buffer_size:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_used_send_buffer:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_receive_buffer_size:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_used_receive_buffer:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_num_hashfails:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_inet_as_name:String(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_inet_as:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_load_balancing(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_peerinfo_requests_in_buffer:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_target_dl_queue_length:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_download_queue_length:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_upload_queue_length:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_failcount:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_downloading_piece_index:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_downloading_block_index:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_downloading_progress:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_downloading_total:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_client:String(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_connection_type:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_remote_dl_rate:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_pending_disk_bytes:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_send_quota:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_receive_quota:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_rtt:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_download_rate_peak:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_upload_rate_peak:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_progress:Float(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_read_state:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_write_state:Int(handle:Byte Ptr)
	Function bmx_torrent_peerinfo_ip:String(handle:Byte Ptr)

	Function bmx_torrent_cachestatus_free(handle:Byte Ptr)
	Function bmx_torrent_cachestatus_blocks_written(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_cachestatus_writes(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_cachestatus_blocks_read(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_cachestatus_blocks_read_hit(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_cachestatus_reads(handle:Byte Ptr, v:Long Ptr)
	Function bmx_torrent_cachestatus_cache_size:Int(handle:Byte Ptr)
	Function bmx_torrent_cachestatus_read_cache_size:Int(handle:Byte Ptr)

	Function bmx_torrent_upnp_add_mapping:Int(handle:Byte Ptr, protocol:Int, externalPort:Int, localPort:Int)
	Function bmx_torrent_upnp_delete_mapping(handle:Byte Ptr, mappingIndex:Int)
	Function bmx_torrent_upnp_router_model:String(handle:Byte Ptr)

	Function bmx_torrent_partialpieceinfo_piece_index:Int(handle:Byte Ptr)
	Function bmx_torrent_partialpieceinfo_blocks_in_piece:Int(handle:Byte Ptr)
	Function bmx_torrent_partialpieceinfo_piece_state:Int(handle:Byte Ptr)
	Function bmx_torrent_partialpieceinfo_free(handle:Byte Ptr)

	Function bmx_torrent_blockinfo_free(handle:Byte Ptr)
	Function bmx_torrent_blockinfo_peer:String(handle:Byte Ptr)
	Function bmx_torrent_blockinfo_state:Int(handle:Byte Ptr)
	Function bmx_torrent_blockinfo_num_peers:Int(handle:Byte Ptr)

End Extern

Rem
bbdoc: 
End Rem
Type TTorrentException Extends TRuntimeException

	Field message:String

	Method CreateException:TTorrentException(message:String)
		Self.message = message
		Return Self
	End Method

	Function _create:TTorrentException(message:String)
		Return New TTorrentException.CreateException(message + " (Warning: Unhandled Exception. Inform Brucey!)")
	End Function
	
	Method ToString:String()
		Return message
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TDuplicateTorrent Extends TTorrentException
	Function _create:TTorrentException(message:String)
		Return New TDuplicateTorrent.CreateException(message)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TInvalidEncoding Extends TTorrentException
	Function _create:TTorrentException(message:String)
		Return New TInvalidEncoding.CreateException(message)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TTypeError Extends TTorrentException
	Function _create:TTorrentException(message:String)
		Return New TTypeError.CreateException(message)
	End Function
End Type

Rem
bbdoc: 
End Rem
Type TInvalidTorrentFile Extends TTorrentException
	Function _create:TTorrentException(message:String)
		Return New TInvalidTorrentFile.CreateException(message)
	End Function
End Type

