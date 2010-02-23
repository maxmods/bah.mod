/*
 Copyright (c) 2010 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "libtorrent/session.hpp"
#include "libtorrent/alert_types.hpp"

class Max_session_status;
class Max_torrent_handle;
class Max_torrent_status;
class Max_alert;
class Max_bitfield;
class Max_peer_info;

extern "C" {

#include "blitz.h"

	BBObject * _bah_libtorrent_TFileRenamedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TFileRenameFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPerformanceAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TStateChangedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTrackerErrorAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTrackerWarningAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TScrapeReplyAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TScrapeFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTrackerReplyAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TDhtReplyAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTrackerAnnounceAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_THashFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerBanAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerUnsnubbedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerSnubbedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerErrorAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerConnectAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerDisconnectedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TInvalidRequestAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentFinishedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPieceFinishedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TRequestDroppedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TBlockTimeoutAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TBlockFinishedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TBlockDownloadingAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TUnwantedBlockAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TStorageMovedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TStorageMovedFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentDeletedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentDeleteFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TSaveResumeDataAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TSaveResumeDataFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentPausedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentResumedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentCheckedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TUrlSeedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TFileErrorAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TMetadataFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TMetadataReceivedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TUdpErrorAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TExternalIpAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TListenFailedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TListenSucceededAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPortmapErrorAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPortmapAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TFastresumeRejectedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerBlockedAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTrackerAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TPeerAlert__create(Max_alert * a);
	BBObject * _bah_libtorrent_TTorrentAlert__create(Max_alert * a);

	BBArray * _bah_libtorrent_TPeerInfo__newArray(int size);
	void _bah_libtorrent_TPeerInfo__newEntry(BBArray * arr, int index, Max_peer_info * info);

	BBString * bmx_LIBTORRENT_VERSION();
	void bmx_LIBTORRENT_VERSION_parts(int * major, int * minor, int * tiny);

	libtorrent::session * bmx_torrent_session_create();
	void bmx_torrent_session_free(libtorrent::session * s);
	int bmx_torrent_session_listenOn(libtorrent::session * s, int portFrom, int portTo, BBString * interface);
	Max_session_status * bmx_torrent_session_status(libtorrent::session * s);
	int bmx_torrent_session_is_listening(libtorrent::session * s);
	void bmx_torrent_session_pause(libtorrent::session * s);
	void bmx_torrent_session_resume(libtorrent::session * s);
	int bmx_torrent_session_is_paused(libtorrent::session * s);
	void bmx_torrent_session_set_upload_rate_limit(libtorrent::session * s, int bytesPerSecond);
	void bmx_torrent_session_set_download_rate_limit(libtorrent::session * s, int bytesPerSecond);
	int bmx_torrent_session_upload_rate_limit(libtorrent::session * s);
	int bmx_torrent_session_download_rate_limit(libtorrent::session * s);
	void bmx_torrent_session_set_max_uploads(libtorrent::session * s, int limit);
	void bmx_torrent_session_set_max_connections(libtorrent::session * s, int limit);
	int bmx_torrent_session_num_uploads(libtorrent::session * s);
	int bmx_torrent_session_num_connections(libtorrent::session * s);
	Max_torrent_handle * bmx_torrent_session_add_torrent(libtorrent::session * s, libtorrent::add_torrent_params * params);
	void bmx_torrent_session_set_settings(libtorrent::session * s, libtorrent::session_settings * settings);
	BBObject * bmx_torrent_session_pop_alert(libtorrent::session * s);

	void bmx_torrent_sessionstatus_free(Max_session_status * s);
	int bmx_torrent_sessionstatus_has_incoming_connections(Max_session_status * s);
	float bmx_torrent_sessionstatus_upload_rate(Max_session_status * s);
	float bmx_torrent_sessionstatus_download_rate(Max_session_status * s);
	float bmx_torrent_sessionstatus_payload_upload_rate(Max_session_status * s);
	float bmx_torrent_sessionstatus_payload_download_rate(Max_session_status * s);
	void bmx_torrent_sessionstatus_total_download(Max_session_status * s, BBInt64 * v);
	void bmx_torrent_sessionstatus_total_upload(Max_session_status * s, BBInt64 * v);
	void bmx_torrent_sessionstatus_total_redundant_bytes(Max_session_status * s, BBInt64 * v);
	void bmx_torrent_sessionstatus_total_failed_bytes(Max_session_status * s, BBInt64 * v);
	void bmx_torrent_sessionstatus_total_payload_download(Max_session_status * s, BBInt64 * v);
	void bmx_torrent_sessionstatus_total_payload_upload(Max_session_status * s, BBInt64 * v);
	int bmx_torrent_sessionstatus_num_peers(Max_session_status * s);
	int bmx_torrent_sessionstatus_num_unchoked(Max_session_status * s);
	int bmx_torrent_sessionstatus_allowed_upload_slots(Max_session_status * s);
	int bmx_torrent_sessionstatus_dht_nodes(Max_session_status * s);
	int bmx_torrent_sessionstatus_dht_node_cache(Max_session_status * s);
	int bmx_torrent_sessionstatus_dht_torrents(Max_session_status * s);
	int bmx_torrent_sessionstatus_dht_global_nodes(Max_session_status * s);

	libtorrent::add_torrent_params * bmx_torrent_addtorrentparams_create();
	void bmx_torrent_addtorrentparams_free(libtorrent::add_torrent_params * par);
	void bmx_torrent_addtorrentparams_set_save_path(libtorrent::add_torrent_params * par, BBString * path);
	void bmx_torrent_addtorrentparams_set_torrent_info(libtorrent::add_torrent_params * par, libtorrent::torrent_info * info);
	void bmx_torrent_addtorrentparams_set_tracker_url(libtorrent::add_torrent_params * par, const char * urlPtr);
	BBString * bmx_torrent_addtorrentparams_tracker_url(libtorrent::add_torrent_params * par);
	void bmx_torrent_addtorrentparams_set_name(libtorrent::add_torrent_params * par, const char * namePtr);
	BBString * bmx_torrent_addtorrentparams_name(libtorrent::add_torrent_params * par);
	BBString * bmx_torrent_addtorrentparams_save_path(libtorrent::add_torrent_params * par);
	void bmx_torrent_addtorrentparams_set_storage_mode(libtorrent::add_torrent_params * par, int mode);
	int bmx_torrent_addtorrentparams_storage_mode(libtorrent::add_torrent_params * par);
	void bmx_torrent_addtorrentparams_set_paused(libtorrent::add_torrent_params * par, int value);
	int bmx_torrent_addtorrentparams_paused(libtorrent::add_torrent_params * par);
	void bmx_torrent_addtorrentparams_set_auto_managed(libtorrent::add_torrent_params * par, int value);
	int bmx_torrent_addtorrentparams_auto_managed(libtorrent::add_torrent_params * par);
	void bmx_torrent_addtorrentparams_set_duplicate_is_error(libtorrent::add_torrent_params * par, int value);
	int bmx_torrent_addtorrentparams_duplicate_is_error(libtorrent::add_torrent_params * par);

	libtorrent::torrent_info * bmx_torrent_torrentinfo_load(BBString * path);
	void bmx_torrent_torrentinfo_free(libtorrent::torrent_info * info);

	void bmx_torrent_torrentstatus_free(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_state(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_paused(Max_torrent_status * status);
	float bmx_torrent_torrentstatus_progress(Max_torrent_status * status);
	BBString * bmx_torrent_torrentstatus_error(Max_torrent_status * status);
	BBString * bmx_torrent_torrentstatus_current_tracker(Max_torrent_status * status);
	void bmx_torrent_torrentstatus_total_upload(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_download(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_payload_download(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_payload_upload(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_failed_bytes(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_redundant_bytes(Max_torrent_status * status, BBInt64 * v);
	float bmx_torrent_torrentstatus_download_rate(Max_torrent_status * status);
	float bmx_torrent_torrentstatus_upload_rate(Max_torrent_status * status);
	float bmx_torrent_torrentstatus_download_payload_rate(Max_torrent_status * status);
	float bmx_torrent_torrentstatus_upload_payload_rate(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_num_peers(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_num_complete(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_num_incomplete(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_list_seeds(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_list_peers(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_connect_candidates(Max_torrent_status * status);
	void bmx_torrent_torrentstatus_total_done(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_wanted_done(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_total_wanted(Max_torrent_status * status, BBInt64 * v);
	int bmx_torrent_torrentstatus_num_seeds(Max_torrent_status * status);
	float bmx_torrent_torrentstatus_distributed_copies(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_block_size(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_num_uploads(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_num_connections(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_uploads_limit(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_connections_limit(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_up_bandwidth_queue(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_down_bandwidth_queue(Max_torrent_status * status);
	void bmx_torrent_torrentstatus_all_time_upload(Max_torrent_status * status, BBInt64 * v);
	void bmx_torrent_torrentstatus_all_time_download(Max_torrent_status * status, BBInt64 * v);
	int bmx_torrent_torrentstatus_active_time(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_seeding_time(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_seed_rank(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_last_scrape(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_has_incoming(Max_torrent_status * status);
	Max_bitfield * bmx_torrent_torrentstatus_pieces(Max_torrent_status * status);
	int bmx_torrent_torrentstatus_num_pieces(Max_torrent_status * status);

	libtorrent::session_settings * bmx_torrent_sessionsettings_create();
	void bmx_torrent_sessionsettings_free(libtorrent::session_settings * s);
	BBString * bmx_torrent_sessionsettings_user_agent(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_user_agent(libtorrent::session_settings * s, BBString * value);
	int bmx_torrent_sessionsettings_tracker_completion_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_tracker_completion_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_tracker_receive_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_tracker_receive_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_stop_tracker_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_stop_tracker_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_tracker_maximum_response_length(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_tracker_maximum_response_length(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_piece_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_piece_timeout(libtorrent::session_settings * s, int value);
	float bmx_torrent_sessionsettings_request_queue_time(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_request_queue_time(libtorrent::session_settings * s, float value);
	int bmx_torrent_sessionsettings_max_allowed_in_request_queue(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_max_allowed_in_request_queue(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_max_out_request_queue(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_max_out_request_queue(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_whole_pieces_threshold(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_whole_pieces_threshold(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_peer_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_peer_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_urlseed_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_urlseed_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_urlseed_pipeline_size(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_urlseed_pipeline_size(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_file_pool_size(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_file_pool_size(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_allow_multiple_connections_per_ip(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_allow_multiple_connections_per_ip(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_max_failcount(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_max_failcount(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_min_reconnect_time(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_min_reconnect_time(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_peer_connect_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_peer_connect_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_ignore_limits_on_local_network(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_ignore_limits_on_local_network(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_connection_speed(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_connection_speed(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_send_redundant_have(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_send_redundant_have(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_lazy_bitfields(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_lazy_bitfields(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_inactivity_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_inactivity_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_unchoke_interval(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_unchoke_interval(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_optimistic_unchoke_multiplier(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_optimistic_unchoke_multiplier(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_num_want(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_num_want(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_initial_picker_threshold(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_initial_picker_threshold(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_allowed_fast_set_size(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_allowed_fast_set_size(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_max_outstanding_disk_bytes_per_connection(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_max_outstanding_disk_bytes_per_connection(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_handshake_timeout(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_handshake_timeout(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_use_dht_as_fallback(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_use_dht_as_fallback(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_free_torrent_hashes(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_free_torrent_hashes(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_upnp_ignore_nonrouters(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_upnp_ignore_nonrouters(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_send_buffer_watermark(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_send_buffer_watermark(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_auto_upload_slots(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_auto_upload_slots(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_use_parole_mode(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_use_parole_mode(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_cache_size(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_cache_size(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_cache_expiry(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_cache_expiry(libtorrent::session_settings * s, int value);
	char bmx_torrent_sessionsettings_peer_tos(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_peer_tos(libtorrent::session_settings * s, char value);
	int bmx_torrent_sessionsettings_active_downloads(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_active_downloads(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_active_seeds(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_active_seeds(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_active_limit(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_active_limit(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_dont_count_slow_torrents(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_dont_count_slow_torrents(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_auto_manage_interval(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_auto_manage_interval(libtorrent::session_settings * s, int value);
	float bmx_torrent_sessionsettings_share_ratio_limit(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_share_ratio_limit(libtorrent::session_settings * s, float value);
	float bmx_torrent_sessionsettings_seed_time_ratio_limit(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_seed_time_ratio_limit(libtorrent::session_settings * s, float value);
	int bmx_torrent_sessionsettings_seed_time_limit(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_seed_time_limit(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_close_redundant_connections(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_close_redundant_connections(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_auto_scrape_interval(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_auto_scrape_interval(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_auto_scrape_min_interval(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_auto_scrape_min_interval(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_max_peerlist_size(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_max_peerlist_size(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_min_announce_interval(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_min_announce_interval(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_prioritize_partial_pieces(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_prioritize_partial_pieces(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_auto_manage_startup(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_auto_manage_startup(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_rate_limit_ip_overhead(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_rate_limit_ip_overhead(libtorrent::session_settings * s, int value);
	int bmx_torrent_sessionsettings_urlseed_wait_retry(libtorrent::session_settings * s);
	void bmx_torrent_sessionsettings_set_urlseed_wait_retry(libtorrent::session_settings * s, int value);
	void bmx_torrent_sessionsettings_outgoing_ports(libtorrent::session_settings * s, int * fromPort, int * toPort);
	void bmx_torrent_sessionsettings_set_outgoing_ports(libtorrent::session_settings * s, int fromPort, int toPort);

	int bmx_torrent_torrenthandle_is_valid(Max_torrent_handle * handle);
	Max_torrent_status * bmx_torrent_torrenthandle_status(Max_torrent_handle * handle);
	BBString * bmx_torrent_torrenthandle_name(Max_torrent_handle * handle);
	const libtorrent::torrent_info * bmx_torrent_torrenthandle_get_torrent_info(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_save_resume_data(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_force_reannounce(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_scrape_tracker(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_set_tracker_login(Max_torrent_handle * handle, BBString * username, BBString * password);
	void bmx_torrent_torrenthandle_add_url_seed(Max_torrent_handle * handle, BBString * url);
	void bmx_torrent_torrenthandle_remove_url_seed(Max_torrent_handle * handle, BBString * url);
	BBArray * bmx_torrent_torrenthandle_url_seeds(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_set_ratio(Max_torrent_handle * handle, float ratio);
	void bmx_torrent_torrenthandle_set_max_uploads(Max_torrent_handle * handle, int maxUploads);
	void bmx_torrent_torrenthandle_set_max_connections(Max_torrent_handle * handle, int maxConnections);
	void bmx_torrent_torrenthandle_set_upload_limit(Max_torrent_handle * handle, int limit);
	int bmx_torrent_torrenthandle_upload_limit(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_set_download_limit(Max_torrent_handle * handle, int limit);
	int bmx_torrent_torrenthandle_download_limit(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_set_sequential_download(Max_torrent_handle * handle, int value);
	int bmx_torrent_torrenthandle_is_sequential_download(Max_torrent_handle * handle);
	int bmx_torrent_torrenthandle_queue_position(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_queue_position_up(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_queue_position_down(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_queue_position_top(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_queue_position_bottom(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_use_interface(Max_torrent_handle * handle, BBString * netInterface);
	void bmx_torrent_torrenthandle_pause(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_resume(Max_torrent_handle * handle);
	int bmx_torrent_torrenthandle_is_paused(Max_torrent_handle * handle);
	int bmx_torrent_torrenthandle_is_seed(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_force_recheck(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_clear_error(Max_torrent_handle * handle);
	int bmx_torrent_torrenthandle_is_auto_managed(Max_torrent_handle * handle);
	void bmx_torrent_torrenthandle_auto_managed(Max_torrent_handle * handle, int value);
	int bmx_torrent_torrenthandle_has_metadata(Max_torrent_handle * handle);
	BBArray * bmx_torrent_torrenthandle_get_peer_info(Max_torrent_handle * handle);

	int bmx_torrent_bitfield_get_bit(Max_bitfield * b, int index);
	int bmx_torrent_bitfield_size(Max_bitfield * b);
	void bmx_torrent_bitfield_free(Max_bitfield * b);

	void bmx_torrent_peerinfo_free(Max_peer_info * p);
	unsigned int bmx_torrent_peerinfo_flags(Max_peer_info * p);
	int bmx_torrent_peerinfo_source(Max_peer_info * p);
	float bmx_torrent_peerinfo_up_speed(Max_peer_info * p);
	float bmx_torrent_peerinfo_down_speed(Max_peer_info * p);
	float bmx_torrent_peerinfo_payload_up_speed(Max_peer_info * p);
	float bmx_torrent_peerinfo_payload_down_speed(Max_peer_info * p);
	void bmx_torrent_peerinfo_total_download(Max_peer_info * p, BBInt64 * v);
	void bmx_torrent_peerinfo_total_upload(Max_peer_info * p, BBInt64 * v);
	Max_bitfield * bmx_torrent_peerinfo_pieces(Max_peer_info * p);
	int bmx_torrent_peerinfo_upload_limit(Max_peer_info * p);
	int bmx_torrent_peerinfo_download_limit(Max_peer_info * p);
	int bmx_torrent_peerinfo_last_request(Max_peer_info * p);
	int bmx_torrent_peerinfo_last_active(Max_peer_info * p);
	int bmx_torrent_peerinfo_request_timeout(Max_peer_info * p);
	int bmx_torrent_peerinfo_send_buffer_size(Max_peer_info * p);
	int bmx_torrent_peerinfo_used_send_buffer(Max_peer_info * p);
	int bmx_torrent_peerinfo_receive_buffer_size(Max_peer_info * p);
	int bmx_torrent_peerinfo_used_receive_buffer(Max_peer_info * p);
	int bmx_torrent_peerinfo_num_hashfails(Max_peer_info * p);
	BBString * bmx_torrent_peerinfo_inet_as_name(Max_peer_info * p);
	int bmx_torrent_peerinfo_inet_as(Max_peer_info * p);
	void bmx_torrent_peerinfo_load_balancing(Max_peer_info * p, BBInt64 * v);
	int bmx_torrent_peerinfo_requests_in_buffer(Max_peer_info * p);
	int bmx_torrent_peerinfo_target_dl_queue_length(Max_peer_info * p);
	int bmx_torrent_peerinfo_download_queue_length(Max_peer_info * p);
	int bmx_torrent_peerinfo_upload_queue_length(Max_peer_info * p);
	int bmx_torrent_peerinfo_failcount(Max_peer_info * p);
	int bmx_torrent_peerinfo_downloading_piece_index(Max_peer_info * p);
	int bmx_torrent_peerinfo_downloading_block_index(Max_peer_info * p);
	int bmx_torrent_peerinfo_downloading_progress(Max_peer_info * p);
	int bmx_torrent_peerinfo_downloading_total(Max_peer_info * p);
	BBString * bmx_torrent_peerinfo_client(Max_peer_info * p);
	int bmx_torrent_peerinfo_connection_type(Max_peer_info * p);
	int bmx_torrent_peerinfo_remote_dl_rate(Max_peer_info * p);
	int bmx_torrent_peerinfo_pending_disk_bytes(Max_peer_info * p);
	int bmx_torrent_peerinfo_send_quota(Max_peer_info * p);
	int bmx_torrent_peerinfo_receive_quota(Max_peer_info * p);
	int bmx_torrent_peerinfo_rtt(Max_peer_info * p);
	int bmx_torrent_peerinfo_download_rate_peak(Max_peer_info * p);
	int bmx_torrent_peerinfo_upload_rate_peak(Max_peer_info * p);
	float bmx_torrent_peerinfo_progress(Max_peer_info * p);
	int bmx_torrent_peerinfo_read_state(Max_peer_info * p);
	int bmx_torrent_peerinfo_write_state(Max_peer_info * p);


}


// --------------------------------------------------------

class Max_session_status
{
public:
	Max_session_status(libtorrent::session_status s);
	~Max_session_status();
	
	libtorrent::session_status &Status();

private:
	libtorrent::session_status status;
};

// --------------------------------------------------------

class Max_torrent_handle
{
public:
	Max_torrent_handle(libtorrent::torrent_handle h);
	~Max_torrent_handle();
	
	libtorrent::torrent_handle & Handle();

private:
	libtorrent::torrent_handle handle;
};

// --------------------------------------------------------

class Max_torrent_status
{
public:
	Max_torrent_status(libtorrent::torrent_status s);
	~Max_torrent_status();
	
	libtorrent::torrent_status &Status();

private:
	libtorrent::torrent_status status;
};

// --------------------------------------------------------

class Max_alert
{
public:
	Max_alert(std::auto_ptr<libtorrent::alert> a);
	Max_alert(libtorrent::alert * a);
	~Max_alert();

	libtorrent::alert * Alert();

private:
	std::auto_ptr<libtorrent::alert> _alert;
	libtorrent::alert * _alertPtr;

};

// --------------------------------------------------------

class Max_bitfield
{
public:
	Max_bitfield(libtorrent::bitfield b);
	~Max_bitfield();
	
	libtorrent::bitfield & Bitfield();

private:
	libtorrent::bitfield bitfield;
};

// --------------------------------------------------------

class Max_peer_info
{
public:
	Max_peer_info(libtorrent::peer_info i);
	~Max_peer_info();
	
	libtorrent::peer_info &Info();

private:
	libtorrent::peer_info info;
};

