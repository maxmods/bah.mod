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

#include "glue.h"

#ifdef WIN32
    extern "C" void tss_cleanup_implemented(void)
    {
    }
#endif


Max_session_status::Max_session_status(libtorrent::session_status s)
	: status(s)
{
}

Max_session_status::~Max_session_status()
{
}

libtorrent::session_status & Max_session_status::Status() {
	return status;
}

// --------------------------------------------------------

Max_torrent_handle::Max_torrent_handle(libtorrent::torrent_handle h)
	: handle(h)
{
}

Max_torrent_handle::~Max_torrent_handle()
{
}

libtorrent::torrent_handle & Max_torrent_handle::Handle() {
	return handle;
}

// --------------------------------------------------------

Max_torrent_status::Max_torrent_status(libtorrent::torrent_status s)
	: status(s)
{
}

Max_torrent_status::~Max_torrent_status()
{
}

libtorrent::torrent_status & Max_torrent_status::Status() {
	return status;
}

// --------------------------------------------------------

Max_alert::Max_alert(std::auto_ptr<libtorrent::alert> a)
	: _alert(a), _alertPtr(a.get())
{
}

Max_alert::Max_alert(libtorrent::alert * a)
	: _alert(0), _alertPtr(a)
{
}

Max_alert::~Max_alert()
{
}

libtorrent::alert * Max_alert::Alert() {
	return _alertPtr;
}

// --------------------------------------------------------

Max_bitfield::Max_bitfield(libtorrent::bitfield b)
	: bitfield(b)
{
}

Max_bitfield::~Max_bitfield()
{
}
	
libtorrent::bitfield & Max_bitfield::Bitfield() {
	return bitfield;
}

// --------------------------------------------------------

Max_peer_info::Max_peer_info(libtorrent::peer_info i)
	: info(i)
{
}

Max_peer_info::~Max_peer_info()
{
}

libtorrent::peer_info & Max_peer_info::Info() {
	return info;
}

// --------------------------------------------------------

BBString * bmx_LIBTORRENT_VERSION() {
	return bbStringFromCString(LIBTORRENT_VERSION);
}

void bmx_LIBTORRENT_VERSION_parts(int * major, int * minor, int * tiny) {
	*major = LIBTORRENT_VERSION_MAJOR;
	*minor = LIBTORRENT_VERSION_MINOR;
	*tiny = LIBTORRENT_VERSION_TINY;
}

// --------------------------------------------------------

BBObject * bmx_torrent_alert_getAlertType(Max_alert * alert) {
	libtorrent::alert * a = alert->Alert();
	
	if (dynamic_cast<libtorrent::file_renamed_alert *>(a)) {
	
		return _bah_libtorrent_TFileRenamedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::file_rename_failed_alert *>(a)) {
	
		return _bah_libtorrent_TFileRenameFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::performance_alert *>(a)) {
	
		return _bah_libtorrent_TPerformanceAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::state_changed_alert *>(a)) {
	
		return _bah_libtorrent_TStateChangedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::tracker_error_alert *>(a)) {
	
		return _bah_libtorrent_TTrackerErrorAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::tracker_warning_alert *>(a)) {
	
		return _bah_libtorrent_TTrackerWarningAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::scrape_reply_alert *>(a)) {
	
		return _bah_libtorrent_TScrapeReplyAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::scrape_failed_alert *>(a)) {
	
		return _bah_libtorrent_TScrapeFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::tracker_reply_alert *>(a)) {
	
		return _bah_libtorrent_TTrackerReplyAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::dht_reply_alert *>(a)) {
	
		return _bah_libtorrent_TDhtReplyAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::tracker_announce_alert *>(a)) {
	
		return _bah_libtorrent_TTrackerAnnounceAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::hash_failed_alert *>(a)) {
	
		return _bah_libtorrent_THashFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_ban_alert *>(a)) {
	
		return _bah_libtorrent_TPeerBanAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_unsnubbed_alert *>(a)) {
	
		return _bah_libtorrent_TPeerUnsnubbedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_snubbed_alert *>(a)) {
	
		return _bah_libtorrent_TPeerSnubbedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_error_alert *>(a)) {
	
		return _bah_libtorrent_TPeerErrorAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_connect_alert *>(a)) {
	
		return _bah_libtorrent_TPeerConnectAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_disconnected_alert *>(a)) {
	
		return _bah_libtorrent_TPeerDisconnectedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::invalid_request_alert *>(a)) {
	
		return _bah_libtorrent_TInvalidRequestAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_finished_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentFinishedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::piece_finished_alert *>(a)) {
	
		return _bah_libtorrent_TPieceFinishedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::request_dropped_alert *>(a)) {
	
		return _bah_libtorrent_TRequestDroppedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::block_timeout_alert *>(a)) {
	
		return _bah_libtorrent_TBlockTimeoutAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::block_finished_alert *>(a)) {
	
		return _bah_libtorrent_TBlockFinishedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::block_downloading_alert *>(a)) {
	
		return _bah_libtorrent_TBlockDownloadingAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::unwanted_block_alert *>(a)) {
	
		return _bah_libtorrent_TUnwantedBlockAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::storage_moved_alert *>(a)) {
	
		return _bah_libtorrent_TStorageMovedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::storage_moved_failed_alert *>(a)) {
	
		return _bah_libtorrent_TStorageMovedFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_deleted_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentDeletedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_delete_failed_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentDeleteFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::save_resume_data_alert *>(a)) {
	
		return _bah_libtorrent_TSaveResumeDataAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::save_resume_data_failed_alert *>(a)) {
	
		return _bah_libtorrent_TSaveResumeDataFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_paused_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentPausedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_resumed_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentResumedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_checked_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentCheckedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::url_seed_alert *>(a)) {
	
		return _bah_libtorrent_TUrlSeedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::file_error_alert *>(a)) {
	
		return _bah_libtorrent_TFileErrorAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::metadata_failed_alert *>(a)) {
	
		return _bah_libtorrent_TMetadataFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::metadata_received_alert *>(a)) {
	
		return _bah_libtorrent_TMetadataReceivedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::udp_error_alert *>(a)) {
	
		return _bah_libtorrent_TUdpErrorAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::external_ip_alert *>(a)) {
	
		return _bah_libtorrent_TExternalIpAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::listen_failed_alert *>(a)) {
	
		return _bah_libtorrent_TListenFailedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::listen_succeeded_alert *>(a)) {
	
		return _bah_libtorrent_TListenSucceededAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::portmap_error_alert *>(a)) {
	
		return _bah_libtorrent_TPortmapErrorAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::portmap_alert *>(a)) {
	
		return _bah_libtorrent_TPortmapAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::fastresume_rejected_alert *>(a)) {
	
		return _bah_libtorrent_TFastresumeRejectedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_blocked_alert *>(a)) {
	
		return _bah_libtorrent_TPeerBlockedAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::tracker_alert *>(a)) {
	
		return _bah_libtorrent_TTrackerAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::peer_alert *>(a)) {
	
		return _bah_libtorrent_TPeerAlert__create(alert);
	
	} else if (dynamic_cast<libtorrent::torrent_alert *>(a)) {
	
		return _bah_libtorrent_TTorrentAlert__create(alert);

	}
}

// --------------------------------------------------------

libtorrent::session * bmx_torrent_session_create() {
	return new libtorrent::session();
}

void bmx_torrent_session_free(libtorrent::session * s) {
	delete s;
}

int bmx_torrent_session_listenOn(libtorrent::session * s, int portFrom, int portTo, BBString * interface) {
	char * i = 0;
	if (interface != &bbEmptyString) {
		i = bbStringToCString(interface);
	}
	
	int res = static_cast<int>(s->listen_on(std::make_pair(6881, 6889), i));
	
	if (i) bbMemFree(i);
	
	return res;
}

Max_session_status * bmx_torrent_session_status(libtorrent::session * s) {
	return new Max_session_status(s->status());
}

int bmx_torrent_session_is_listening(libtorrent::session * s) {
	return static_cast<int>(s->is_listening());
}

void bmx_torrent_session_pause(libtorrent::session * s) {
	s->pause();
}

void bmx_torrent_session_resume(libtorrent::session * s) {
	s->resume();
}

int bmx_torrent_session_is_paused(libtorrent::session * s) {
	return s->is_paused();
}

void bmx_torrent_session_set_upload_rate_limit(libtorrent::session * s, int bytesPerSecond) {
	s->set_upload_rate_limit(bytesPerSecond);
}

void bmx_torrent_session_set_download_rate_limit(libtorrent::session * s, int bytesPerSecond) {
	s->set_download_rate_limit(bytesPerSecond);
}

int bmx_torrent_session_upload_rate_limit(libtorrent::session * s) {
	return s->upload_rate_limit();
}

int bmx_torrent_session_download_rate_limit(libtorrent::session * s) {
	return s->download_rate_limit();
}

void bmx_torrent_session_set_max_uploads(libtorrent::session * s, int limit) {
	s->set_max_uploads(limit);
}

void bmx_torrent_session_set_max_connections(libtorrent::session * s, int limit) {
	s->set_max_connections(limit);
}

int bmx_torrent_session_num_uploads(libtorrent::session * s) {
	return s->num_uploads();
}

int bmx_torrent_session_num_connections(libtorrent::session * s) {
	return s->num_connections();
}

Max_torrent_handle * bmx_torrent_session_add_torrent(libtorrent::session * s, libtorrent::add_torrent_params * params) {
	return new Max_torrent_handle(s->add_torrent(*params));
}

void bmx_torrent_session_set_settings(libtorrent::session * s, libtorrent::session_settings * settings) {
	s->set_settings(*settings);
}

BBObject * bmx_torrent_session_pop_alert(libtorrent::session * s) {
	std::auto_ptr<libtorrent::alert> a = s->pop_alert();
	
	if (a.get()) {
		Max_alert * alert = new Max_alert(a);

		return bmx_torrent_alert_getAlertType(alert);
	
	} else {
		return &bbNullObject;
	}
}

// --------------------------------------------------------

void bmx_torrent_sessionstatus_free(Max_session_status * s) {
	delete s;
}

int bmx_torrent_sessionstatus_has_incoming_connections(Max_session_status * s) {
	return static_cast<int>(s->Status().has_incoming_connections);
}

float bmx_torrent_sessionstatus_upload_rate(Max_session_status * s) {
	return s->Status().upload_rate;
}

float bmx_torrent_sessionstatus_download_rate(Max_session_status * s) {
	return s->Status().download_rate;
}

float bmx_torrent_sessionstatus_payload_upload_rate(Max_session_status * s) {
	return s->Status().payload_upload_rate;
}

float bmx_torrent_sessionstatus_payload_download_rate(Max_session_status * s) {
	return s->Status().payload_download_rate;
}

void bmx_torrent_sessionstatus_total_download(Max_session_status * s, BBInt64 * v) {
	*v = s->Status().total_download;
}

void bmx_torrent_sessionstatus_total_upload(Max_session_status * s, BBInt64 * v) {
	*v = s->Status().total_upload;
}

void bmx_torrent_sessionstatus_total_redundant_bytes(Max_session_status * s, BBInt64 * v) {
	*v = s->Status().total_redundant_bytes;
}

void bmx_torrent_sessionstatus_total_failed_bytes(Max_session_status * s, BBInt64 * v) {
	*v = s->Status().total_failed_bytes;
}

void bmx_torrent_sessionstatus_total_payload_download(Max_session_status * s, BBInt64 * v) {
	*v = s->Status().total_payload_download;
}

void bmx_torrent_sessionstatus_total_payload_upload(Max_session_status * s, BBInt64 * v) {
	*v = s->Status().total_payload_upload;
}

int bmx_torrent_sessionstatus_num_peers(Max_session_status * s) {
	return s->Status().num_peers;
}

int bmx_torrent_sessionstatus_num_unchoked(Max_session_status * s) {
	return s->Status().num_unchoked;
}

int bmx_torrent_sessionstatus_allowed_upload_slots(Max_session_status * s) {
	return s->Status().allowed_upload_slots;
}

int bmx_torrent_sessionstatus_dht_nodes(Max_session_status * s) {
	return s->Status().dht_nodes;
}

int bmx_torrent_sessionstatus_dht_node_cache(Max_session_status * s) {
	return s->Status().dht_node_cache;
}

int bmx_torrent_sessionstatus_dht_torrents(Max_session_status * s) {
	return s->Status().dht_torrents;
}

int bmx_torrent_sessionstatus_dht_global_nodes(Max_session_status * s) {
	return s->Status().dht_global_nodes;
}


// --------------------------------------------------------

libtorrent::add_torrent_params * bmx_torrent_addtorrentparams_create() {
	return new libtorrent::add_torrent_params();
}

void bmx_torrent_addtorrentparams_free(libtorrent::add_torrent_params * par) {
	delete par;
}

void bmx_torrent_addtorrentparams_set_save_path(libtorrent::add_torrent_params * par, BBString * path) {
	char * p = bbStringToUTF8String(path);
	par->save_path = p;
	bbMemFree(p);
}

void bmx_torrent_addtorrentparams_set_torrent_info(libtorrent::add_torrent_params * par, libtorrent::torrent_info * info) {
	par->ti = info;
}

void bmx_torrent_addtorrentparams_set_tracker_url(libtorrent::add_torrent_params * par, const char * urlPtr) {
	par->tracker_url = urlPtr;
}

BBString * bmx_torrent_addtorrentparams_tracker_url(libtorrent::add_torrent_params * par) {
	return bbStringFromUTF8String(par->tracker_url);
}

void bmx_torrent_addtorrentparams_set_name(libtorrent::add_torrent_params * par, const char * namePtr) {
	par->name = namePtr;
}

BBString * bmx_torrent_addtorrentparams_name(libtorrent::add_torrent_params * par) {
	return bbStringFromUTF8String(par->name);
}

BBString * bmx_torrent_addtorrentparams_save_path(libtorrent::add_torrent_params * par) {
	return bbStringFromUTF8String(par->save_path.string().c_str());
}

void bmx_torrent_addtorrentparams_set_storage_mode(libtorrent::add_torrent_params * par, int mode) {
	par->storage_mode = static_cast<libtorrent::storage_mode_t>(mode);
}

int bmx_torrent_addtorrentparams_storage_mode(libtorrent::add_torrent_params * par) {
	return static_cast<int>(par->storage_mode);
}

void bmx_torrent_addtorrentparams_set_paused(libtorrent::add_torrent_params * par, int value) {
	par->paused = static_cast<bool>(value);
}

int bmx_torrent_addtorrentparams_paused(libtorrent::add_torrent_params * par) {
	return static_cast<int>(par->paused);
}

void bmx_torrent_addtorrentparams_set_auto_managed(libtorrent::add_torrent_params * par, int value) {
	par->auto_managed = static_cast<bool>(value);
}

int bmx_torrent_addtorrentparams_auto_managed(libtorrent::add_torrent_params * par) {
	return static_cast<int>(par->auto_managed);
}

void bmx_torrent_addtorrentparams_set_duplicate_is_error(libtorrent::add_torrent_params * par, int value) {
	par->duplicate_is_error = static_cast<bool>(value);
}

int bmx_torrent_addtorrentparams_duplicate_is_error(libtorrent::add_torrent_params * par) {
	return static_cast<int>(par->duplicate_is_error);
}


// --------------------------------------------------------

libtorrent::torrent_info * bmx_torrent_torrentinfo_load(BBString * path) {
	char * p = bbStringToCString(path);
	
	libtorrent::torrent_info * info = new libtorrent::torrent_info(p);
	
	bbMemFree(p);
	return info;
}

void bmx_torrent_torrentinfo_free(libtorrent::torrent_info * info) {
	delete info;
}

// --------------------------------------------------------

void bmx_torrent_torrentstatus_free(Max_torrent_status * status) {
	delete status;
}

int bmx_torrent_torrentstatus_state(Max_torrent_status * status) {
	return status->Status().state;
}

int bmx_torrent_torrentstatus_paused(Max_torrent_status * status) {
	return static_cast<int>(status->Status().paused);
}

float bmx_torrent_torrentstatus_progress(Max_torrent_status * status) {
	return status->Status().progress;
}

BBString * bmx_torrent_torrentstatus_error(Max_torrent_status * status) {
	return bbStringFromUTF8String(status->Status().error.c_str());
}

BBString * bmx_torrent_torrentstatus_current_tracker(Max_torrent_status * status) {
	return bbStringFromUTF8String(status->Status().current_tracker.c_str());
}

void bmx_torrent_torrentstatus_total_upload(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_upload;
}

void bmx_torrent_torrentstatus_total_download(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_download;
}

void bmx_torrent_torrentstatus_total_payload_download(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_payload_download;
}

void bmx_torrent_torrentstatus_total_payload_upload(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_payload_upload;
}

void bmx_torrent_torrentstatus_total_failed_bytes(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_failed_bytes;
}

void bmx_torrent_torrentstatus_total_redundant_bytes(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_redundant_bytes;
}

float bmx_torrent_torrentstatus_download_rate(Max_torrent_status * status) {
	return status->Status().download_rate;
}

float bmx_torrent_torrentstatus_upload_rate(Max_torrent_status * status) {
	return status->Status().upload_rate;
}

float bmx_torrent_torrentstatus_download_payload_rate(Max_torrent_status * status) {
	return status->Status().download_payload_rate;
}

float bmx_torrent_torrentstatus_upload_payload_rate(Max_torrent_status * status) {
	return status->Status().upload_payload_rate;
}

int bmx_torrent_torrentstatus_num_peers(Max_torrent_status * status) {
	return status->Status().num_peers;
}

int bmx_torrent_torrentstatus_num_complete(Max_torrent_status * status) {
	return status->Status().num_complete;
}

int bmx_torrent_torrentstatus_num_incomplete(Max_torrent_status * status) {
	return status->Status().num_incomplete;
}

int bmx_torrent_torrentstatus_list_seeds(Max_torrent_status * status) {
	return status->Status().list_seeds;
}

int bmx_torrent_torrentstatus_list_peers(Max_torrent_status * status) {
	return status->Status().list_peers;
}

int bmx_torrent_torrentstatus_connect_candidates(Max_torrent_status * status) {
	return status->Status().connect_candidates;
}

void bmx_torrent_torrentstatus_total_done(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_done;
}

void bmx_torrent_torrentstatus_total_wanted_done(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_wanted_done;
}

void bmx_torrent_torrentstatus_total_wanted(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().total_wanted;
}

int bmx_torrent_torrentstatus_num_seeds(Max_torrent_status * status) {
	return status->Status().num_seeds;
}

float bmx_torrent_torrentstatus_distributed_copies(Max_torrent_status * status) {
	return status->Status().distributed_copies;
}

int bmx_torrent_torrentstatus_block_size(Max_torrent_status * status) {
	return status->Status().block_size;
}

int bmx_torrent_torrentstatus_num_uploads(Max_torrent_status * status) {
	return status->Status().num_uploads;
}

int bmx_torrent_torrentstatus_num_connections(Max_torrent_status * status) {
	return status->Status().num_connections;
}

int bmx_torrent_torrentstatus_uploads_limit(Max_torrent_status * status) {
	return status->Status().uploads_limit;
}

int bmx_torrent_torrentstatus_connections_limit(Max_torrent_status * status) {
	return status->Status().connections_limit;
}

int bmx_torrent_torrentstatus_up_bandwidth_queue(Max_torrent_status * status) {
	return status->Status().up_bandwidth_queue;
}

int bmx_torrent_torrentstatus_down_bandwidth_queue(Max_torrent_status * status) {
	return status->Status().down_bandwidth_queue;
}

void bmx_torrent_torrentstatus_all_time_upload(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().all_time_upload;
}

void bmx_torrent_torrentstatus_all_time_download(Max_torrent_status * status, BBInt64 * v) {
	*v = status->Status().all_time_download;
}

int bmx_torrent_torrentstatus_active_time(Max_torrent_status * status) {
	return status->Status().active_time;
}

int bmx_torrent_torrentstatus_seeding_time(Max_torrent_status * status) {
	return status->Status().seeding_time;
}

int bmx_torrent_torrentstatus_seed_rank(Max_torrent_status * status) {
	return status->Status().seed_rank;
}

int bmx_torrent_torrentstatus_last_scrape(Max_torrent_status * status) {
	return status->Status().last_scrape;
}

int bmx_torrent_torrentstatus_has_incoming(Max_torrent_status * status) {
	return static_cast<int>(status->Status().has_incoming);
}

Max_bitfield * bmx_torrent_torrentstatus_pieces(Max_torrent_status * status) {
	return new Max_bitfield(status->Status().pieces);
}

int bmx_torrent_torrentstatus_num_pieces(Max_torrent_status * status) {
	return status->Status().num_pieces;
}

// --------------------------------------------------------

libtorrent::session_settings * bmx_torrent_sessionsettings_create() {
	return new libtorrent::session_settings();
}

void bmx_torrent_sessionsettings_free(libtorrent::session_settings * s) {
	delete s;
}

BBString * bmx_torrent_sessionsettings_user_agent(libtorrent::session_settings * s) {
	return bbStringFromUTF8String(s->user_agent.c_str());
}

void bmx_torrent_sessionsettings_set_user_agent(libtorrent::session_settings * s, BBString * value) {
	char * v = bbStringToUTF8String(value);
	s->user_agent = v;
	bbMemFree(v);
}

int bmx_torrent_sessionsettings_tracker_completion_timeout(libtorrent::session_settings * s) {
	return s->tracker_completion_timeout;
}

void bmx_torrent_sessionsettings_set_tracker_completion_timeout(libtorrent::session_settings * s, int value) {
	s->tracker_completion_timeout = value;
}

int bmx_torrent_sessionsettings_tracker_receive_timeout(libtorrent::session_settings * s) {
	return s->tracker_receive_timeout;
}

void bmx_torrent_sessionsettings_set_tracker_receive_timeout(libtorrent::session_settings * s, int value) {
	s->tracker_receive_timeout = value;
}

int bmx_torrent_sessionsettings_stop_tracker_timeout(libtorrent::session_settings * s) {
	return s->stop_tracker_timeout;
}

void bmx_torrent_sessionsettings_set_stop_tracker_timeout(libtorrent::session_settings * s, int value) {
	s->stop_tracker_timeout = value;
}

int bmx_torrent_sessionsettings_tracker_maximum_response_length(libtorrent::session_settings * s) {
	return s->tracker_maximum_response_length;
}

void bmx_torrent_sessionsettings_set_tracker_maximum_response_length(libtorrent::session_settings * s, int value) {
	s->tracker_maximum_response_length = value;
}


int bmx_torrent_sessionsettings_piece_timeout(libtorrent::session_settings * s) {
	return s->piece_timeout;
}

void bmx_torrent_sessionsettings_set_piece_timeout(libtorrent::session_settings * s, int value) {
	s->piece_timeout = value;
}

float bmx_torrent_sessionsettings_request_queue_time(libtorrent::session_settings * s) {
	return s->request_queue_time;
}

void bmx_torrent_sessionsettings_set_request_queue_time(libtorrent::session_settings * s, float value) {
	s->request_queue_time = value;
}

int bmx_torrent_sessionsettings_max_allowed_in_request_queue(libtorrent::session_settings * s) {
	return s->max_allowed_in_request_queue;
}

void bmx_torrent_sessionsettings_set_max_allowed_in_request_queue(libtorrent::session_settings * s, int value) {
	s->max_allowed_in_request_queue = value;
}

int bmx_torrent_sessionsettings_max_out_request_queue(libtorrent::session_settings * s) {
	return s->max_out_request_queue;
}

void bmx_torrent_sessionsettings_set_max_out_request_queue(libtorrent::session_settings * s, int value) {
	s->max_out_request_queue = value;
}

int bmx_torrent_sessionsettings_whole_pieces_threshold(libtorrent::session_settings * s) {
	return s->whole_pieces_threshold;
}

void bmx_torrent_sessionsettings_set_whole_pieces_threshold(libtorrent::session_settings * s, int value) {
	s->whole_pieces_threshold = value;
}

int bmx_torrent_sessionsettings_peer_timeout(libtorrent::session_settings * s) {
	return s->peer_timeout;
}

void bmx_torrent_sessionsettings_set_peer_timeout(libtorrent::session_settings * s, int value) {
	s->peer_timeout = value;
}

int bmx_torrent_sessionsettings_urlseed_timeout(libtorrent::session_settings * s) {
	return s->urlseed_timeout;
}

void bmx_torrent_sessionsettings_set_urlseed_timeout(libtorrent::session_settings * s, int value) {
	s->urlseed_timeout = value;
}

int bmx_torrent_sessionsettings_urlseed_pipeline_size(libtorrent::session_settings * s) {
	return s->urlseed_pipeline_size;
}

void bmx_torrent_sessionsettings_set_urlseed_pipeline_size(libtorrent::session_settings * s, int value) {
	s->urlseed_pipeline_size = value;
}

int bmx_torrent_sessionsettings_file_pool_size(libtorrent::session_settings * s) {
	return s->file_pool_size;
}

void bmx_torrent_sessionsettings_set_file_pool_size(libtorrent::session_settings * s, int value) {
	s->file_pool_size = value;
}

int bmx_torrent_sessionsettings_allow_multiple_connections_per_ip(libtorrent::session_settings * s) {
	return static_cast<int>(s->allow_multiple_connections_per_ip);
}

void bmx_torrent_sessionsettings_set_allow_multiple_connections_per_ip(libtorrent::session_settings * s, int value) {
	s->allow_multiple_connections_per_ip = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_max_failcount(libtorrent::session_settings * s) {
	return s->max_failcount;
}

void bmx_torrent_sessionsettings_set_max_failcount(libtorrent::session_settings * s, int value) {
	s->max_failcount = value;
}

int bmx_torrent_sessionsettings_min_reconnect_time(libtorrent::session_settings * s) {
	return s->min_reconnect_time;
}

void bmx_torrent_sessionsettings_set_min_reconnect_time(libtorrent::session_settings * s, int value) {
	s->min_reconnect_time = value;
}

int bmx_torrent_sessionsettings_peer_connect_timeout(libtorrent::session_settings * s) {
	return s->peer_connect_timeout;
}

void bmx_torrent_sessionsettings_set_peer_connect_timeout(libtorrent::session_settings * s, int value) {
	s->peer_connect_timeout = value;
}

int bmx_torrent_sessionsettings_ignore_limits_on_local_network(libtorrent::session_settings * s) {
	return static_cast<int>(s->ignore_limits_on_local_network);
}

void bmx_torrent_sessionsettings_set_ignore_limits_on_local_network(libtorrent::session_settings * s, int value) {
	s->ignore_limits_on_local_network = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_connection_speed(libtorrent::session_settings * s) {
	return s->connection_speed;
}

void bmx_torrent_sessionsettings_set_connection_speed(libtorrent::session_settings * s, int value) {
	s->connection_speed = value;
}

int bmx_torrent_sessionsettings_send_redundant_have(libtorrent::session_settings * s) {
	return static_cast<int>(s->send_redundant_have);
}

void bmx_torrent_sessionsettings_set_send_redundant_have(libtorrent::session_settings * s, int value) {
	s->send_redundant_have = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_lazy_bitfields(libtorrent::session_settings * s) {
	return static_cast<int>(s->lazy_bitfields);
}

void bmx_torrent_sessionsettings_set_lazy_bitfields(libtorrent::session_settings * s, int value) {
	s->lazy_bitfields = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_inactivity_timeout(libtorrent::session_settings * s) {
	return s->inactivity_timeout;
}

void bmx_torrent_sessionsettings_set_inactivity_timeout(libtorrent::session_settings * s, int value) {
	s->inactivity_timeout = value;
}

int bmx_torrent_sessionsettings_unchoke_interval(libtorrent::session_settings * s) {
	return s->unchoke_interval;
}

void bmx_torrent_sessionsettings_set_unchoke_interval(libtorrent::session_settings * s, int value) {
	s->unchoke_interval = value;
}

int bmx_torrent_sessionsettings_optimistic_unchoke_multiplier(libtorrent::session_settings * s) {
	return s->optimistic_unchoke_multiplier;
}

void bmx_torrent_sessionsettings_set_optimistic_unchoke_multiplier(libtorrent::session_settings * s, int value) {
	s->optimistic_unchoke_multiplier = value;
}

int bmx_torrent_sessionsettings_num_want(libtorrent::session_settings * s) {
	return s->num_want;
}

void bmx_torrent_sessionsettings_set_num_want(libtorrent::session_settings * s, int value) {
	s->num_want = value;
}

int bmx_torrent_sessionsettings_initial_picker_threshold(libtorrent::session_settings * s) {
	return s->initial_picker_threshold;
}

void bmx_torrent_sessionsettings_set_initial_picker_threshold(libtorrent::session_settings * s, int value) {
	s->initial_picker_threshold = value;
}

int bmx_torrent_sessionsettings_allowed_fast_set_size(libtorrent::session_settings * s) {
	return s->allowed_fast_set_size;
}

void bmx_torrent_sessionsettings_set_allowed_fast_set_size(libtorrent::session_settings * s, int value) {
	s->allowed_fast_set_size = value;
}

int bmx_torrent_sessionsettings_max_outstanding_disk_bytes_per_connection(libtorrent::session_settings * s) {
	return s->max_outstanding_disk_bytes_per_connection;
}

void bmx_torrent_sessionsettings_set_max_outstanding_disk_bytes_per_connection(libtorrent::session_settings * s, int value) {
	s->max_outstanding_disk_bytes_per_connection = value;
}

int bmx_torrent_sessionsettings_handshake_timeout(libtorrent::session_settings * s) {
	return s->handshake_timeout;
}

void bmx_torrent_sessionsettings_set_handshake_timeout(libtorrent::session_settings * s, int value) {
	s->handshake_timeout = value;
}

int bmx_torrent_sessionsettings_use_dht_as_fallback(libtorrent::session_settings * s) {
	return static_cast<int>(s->use_dht_as_fallback);
}

void bmx_torrent_sessionsettings_set_use_dht_as_fallback(libtorrent::session_settings * s, int value) {
	s->use_dht_as_fallback = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_free_torrent_hashes(libtorrent::session_settings * s) {
	return static_cast<int>(s->free_torrent_hashes);
}

void bmx_torrent_sessionsettings_set_free_torrent_hashes(libtorrent::session_settings * s, int value) {
	s->free_torrent_hashes = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_upnp_ignore_nonrouters(libtorrent::session_settings * s) {
	return static_cast<int>(s->upnp_ignore_nonrouters);
}

void bmx_torrent_sessionsettings_set_upnp_ignore_nonrouters(libtorrent::session_settings * s, int value) {
	s->upnp_ignore_nonrouters = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_send_buffer_watermark(libtorrent::session_settings * s) {
	return s->send_buffer_watermark;
}

void bmx_torrent_sessionsettings_set_send_buffer_watermark(libtorrent::session_settings * s, int value) {
	s->send_buffer_watermark = value;
}

int bmx_torrent_sessionsettings_auto_upload_slots(libtorrent::session_settings * s) {
	return static_cast<int>(s->auto_upload_slots);
}

void bmx_torrent_sessionsettings_set_auto_upload_slots(libtorrent::session_settings * s, int value) {
	s->auto_upload_slots = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_use_parole_mode(libtorrent::session_settings * s) {
	return static_cast<int>(s->use_parole_mode);
}

void bmx_torrent_sessionsettings_set_use_parole_mode(libtorrent::session_settings * s, int value) {
	s->use_parole_mode = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_cache_size(libtorrent::session_settings * s) {
	return s->cache_size;
}

void bmx_torrent_sessionsettings_set_cache_size(libtorrent::session_settings * s, int value) {
	s->cache_size = value;
}

int bmx_torrent_sessionsettings_cache_expiry(libtorrent::session_settings * s) {
	return s->cache_expiry;
}

void bmx_torrent_sessionsettings_set_cache_expiry(libtorrent::session_settings * s, int value) {
	s->cache_expiry = value;
}

std::pair<int, int> outgoing_ports;
char bmx_torrent_sessionsettings_peer_tos(libtorrent::session_settings * s) {
	return s->peer_tos;
}

void bmx_torrent_sessionsettings_set_peer_tos(libtorrent::session_settings * s, char value) {
	s->peer_tos = value;
}


int bmx_torrent_sessionsettings_active_downloads(libtorrent::session_settings * s) {
	return s->active_downloads;
}

void bmx_torrent_sessionsettings_set_active_downloads(libtorrent::session_settings * s, int value) {
	s->active_downloads = value;
}

int bmx_torrent_sessionsettings_active_seeds(libtorrent::session_settings * s) {
	return s->active_seeds;
}

void bmx_torrent_sessionsettings_set_active_seeds(libtorrent::session_settings * s, int value) {
	s->active_seeds = value;
}

int bmx_torrent_sessionsettings_active_limit(libtorrent::session_settings * s) {
	return s->active_limit;
}

void bmx_torrent_sessionsettings_set_active_limit(libtorrent::session_settings * s, int value) {
	s->active_limit = value;
}

int bmx_torrent_sessionsettings_dont_count_slow_torrents(libtorrent::session_settings * s) {
	return static_cast<int>(s->dont_count_slow_torrents);
}

void bmx_torrent_sessionsettings_set_dont_count_slow_torrents(libtorrent::session_settings * s, int value) {
	s->dont_count_slow_torrents = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_auto_manage_interval(libtorrent::session_settings * s) {
	return s->auto_manage_interval;
}

void bmx_torrent_sessionsettings_set_auto_manage_interval(libtorrent::session_settings * s, int value) {
	s->auto_manage_interval = value;
}

float bmx_torrent_sessionsettings_share_ratio_limit(libtorrent::session_settings * s) {
	return s->share_ratio_limit;
}

void bmx_torrent_sessionsettings_set_share_ratio_limit(libtorrent::session_settings * s, float value) {
	s->share_ratio_limit = value;
}

float bmx_torrent_sessionsettings_seed_time_ratio_limit(libtorrent::session_settings * s) {
	return s->seed_time_ratio_limit;
}

void bmx_torrent_sessionsettings_set_seed_time_ratio_limit(libtorrent::session_settings * s, float value) {
	s->seed_time_ratio_limit = value;
}

int bmx_torrent_sessionsettings_seed_time_limit(libtorrent::session_settings * s) {
	return s->seed_time_limit;
}

void bmx_torrent_sessionsettings_set_seed_time_limit(libtorrent::session_settings * s, int value) {
	s->seed_time_limit = value;
}

int bmx_torrent_sessionsettings_close_redundant_connections(libtorrent::session_settings * s) {
	return static_cast<int>(s->close_redundant_connections);
}

void bmx_torrent_sessionsettings_set_close_redundant_connections(libtorrent::session_settings * s, int value) {
	s->close_redundant_connections = static_cast<bool>(value);
}


int bmx_torrent_sessionsettings_auto_scrape_interval(libtorrent::session_settings * s) {
	return s->auto_scrape_interval;
}

void bmx_torrent_sessionsettings_set_auto_scrape_interval(libtorrent::session_settings * s, int value) {
	s->auto_scrape_interval = value;
}

int bmx_torrent_sessionsettings_auto_scrape_min_interval(libtorrent::session_settings * s) {
	return s->auto_scrape_min_interval;
}

void bmx_torrent_sessionsettings_set_auto_scrape_min_interval(libtorrent::session_settings * s, int value) {
	s->auto_scrape_min_interval = value;
}


int bmx_torrent_sessionsettings_max_peerlist_size(libtorrent::session_settings * s) {
	return s->max_peerlist_size;
}

void bmx_torrent_sessionsettings_set_max_peerlist_size(libtorrent::session_settings * s, int value) {
	s->max_peerlist_size = value;
}


int bmx_torrent_sessionsettings_min_announce_interval(libtorrent::session_settings * s) {
	return s->min_announce_interval;
}

void bmx_torrent_sessionsettings_set_min_announce_interval(libtorrent::session_settings * s, int value) {
	s->min_announce_interval = value;
}


int bmx_torrent_sessionsettings_prioritize_partial_pieces(libtorrent::session_settings * s) {
	return static_cast<int>(s->prioritize_partial_pieces);
}

void bmx_torrent_sessionsettings_set_prioritize_partial_pieces(libtorrent::session_settings * s, int value) {
	s->prioritize_partial_pieces = static_cast<bool>(value);
}

int bmx_torrent_sessionsettings_auto_manage_startup(libtorrent::session_settings * s) {
	return s->auto_manage_startup;
}

void bmx_torrent_sessionsettings_set_auto_manage_startup(libtorrent::session_settings * s, int value) {
	s->auto_manage_startup = value;
}

int bmx_torrent_sessionsettings_rate_limit_ip_overhead(libtorrent::session_settings * s) {
	return s->rate_limit_ip_overhead;
}

void bmx_torrent_sessionsettings_set_rate_limit_ip_overhead(libtorrent::session_settings * s, int value) {
	s->rate_limit_ip_overhead = value;
}

int bmx_torrent_sessionsettings_urlseed_wait_retry(libtorrent::session_settings * s) {
	return s->urlseed_wait_retry;
}

void bmx_torrent_sessionsettings_set_urlseed_wait_retry(libtorrent::session_settings * s, int value) {
	s->urlseed_wait_retry = value;
}

void bmx_torrent_sessionsettings_outgoing_ports(libtorrent::session_settings * s, int * fromPort, int * toPort) {
	*fromPort = s->outgoing_ports.first;
	*toPort = s->outgoing_ports.second;
}

void bmx_torrent_sessionsettings_set_outgoing_ports(libtorrent::session_settings * s, int fromPort, int toPort) {
	s->outgoing_ports.first = fromPort;
	s->outgoing_ports.second = toPort;
}

// --------------------------------------------------------

int bmx_torrent_torrenthandle_is_valid(Max_torrent_handle * handle) {
	static_cast<int>(handle->Handle().is_valid());
}

Max_torrent_status * bmx_torrent_torrenthandle_status(Max_torrent_handle * handle) {
	return new Max_torrent_status(handle->Handle().status());
}

BBString * bmx_torrent_torrenthandle_name(Max_torrent_handle * handle) {
	return bbStringFromUTF8String(handle->Handle().name().c_str());
}

const libtorrent::torrent_info * bmx_torrent_torrenthandle_get_torrent_info(Max_torrent_handle * handle) {
	return &handle->Handle().get_torrent_info();
}

void bmx_torrent_torrenthandle_save_resume_data(Max_torrent_handle * handle) {
	handle->Handle().save_resume_data();
}

void bmx_torrent_torrenthandle_force_reannounce(Max_torrent_handle * handle) {
	handle->Handle().force_reannounce();
}

void bmx_torrent_torrenthandle_scrape_tracker(Max_torrent_handle * handle) {
	handle->Handle().scrape_tracker();
}

void bmx_torrent_torrenthandle_set_tracker_login(Max_torrent_handle * handle, BBString * username, BBString * password) {
	char * u = bbStringToCString(username);
	char * p = bbStringToCString(password);
	handle->Handle().set_tracker_login(u, p);
	bbMemFree(u);
	bbMemFree(p);
}

void bmx_torrent_torrenthandle_add_url_seed(Max_torrent_handle * handle, BBString * url) {
	char * u = bbStringToCString(url);
	handle->Handle().add_url_seed(u);
	bbMemFree(u);
}

void bmx_torrent_torrenthandle_remove_url_seed(Max_torrent_handle * handle, BBString * url) {
	char * u = bbStringToCString(url);
	handle->Handle().remove_url_seed(u);
	bbMemFree(u);
}

BBArray * bmx_torrent_torrenthandle_url_seeds(Max_torrent_handle * handle) {
	std::set<std::string> strings = handle->Handle().url_seeds();
	int count = strings.size();
	if (!count) {
		return &bbEmptyArray;
	}
	BBArray * arr = bbArrayNew1D("$", count);
	BBString **s = (BBString**)BBARRAYDATA(arr, arr->dims );

	int i = 0;
	for (std::set<std::string>::iterator iter = strings.begin(); iter != strings.end(); iter++) {
		s[i] = bbStringFromUTF8String((*iter).c_str());
		BBRETAIN(s[i]);
		i++;
	}

	return arr;	
}

void bmx_torrent_torrenthandle_set_ratio(Max_torrent_handle * handle, float ratio) {
	handle->Handle().set_ratio(ratio);
}

void bmx_torrent_torrenthandle_set_max_uploads(Max_torrent_handle * handle, int maxUploads) {
	handle->Handle().set_max_uploads(maxUploads);
}

void bmx_torrent_torrenthandle_set_max_connections(Max_torrent_handle * handle, int maxConnections) {
	handle->Handle().set_max_connections(maxConnections);
}

void bmx_torrent_torrenthandle_set_upload_limit(Max_torrent_handle * handle, int limit) {
	handle->Handle().set_upload_limit(limit);
}

int bmx_torrent_torrenthandle_upload_limit(Max_torrent_handle * handle) {
	return handle->Handle().upload_limit();
}

void bmx_torrent_torrenthandle_set_download_limit(Max_torrent_handle * handle, int limit) {
	handle->Handle().set_download_limit(limit);
}

int bmx_torrent_torrenthandle_download_limit(Max_torrent_handle * handle) {
	return handle->Handle().download_limit();
}

void bmx_torrent_torrenthandle_set_sequential_download(Max_torrent_handle * handle, int value) {
	handle->Handle().set_sequential_download(static_cast<bool>(value));
}

int bmx_torrent_torrenthandle_is_sequential_download(Max_torrent_handle * handle) {
	return static_cast<int>(handle->Handle().is_sequential_download());
}

int bmx_torrent_torrenthandle_queue_position(Max_torrent_handle * handle) {
	return handle->Handle().queue_position();
}

void bmx_torrent_torrenthandle_queue_position_up(Max_torrent_handle * handle) {
	handle->Handle().queue_position_up();
}

void bmx_torrent_torrenthandle_queue_position_down(Max_torrent_handle * handle) {
	handle->Handle().queue_position_down();
}

void bmx_torrent_torrenthandle_queue_position_top(Max_torrent_handle * handle) {
	handle->Handle().queue_position_top();
}

void bmx_torrent_torrenthandle_queue_position_bottom(Max_torrent_handle * handle) {
	handle->Handle().queue_position_bottom();
}

void bmx_torrent_torrenthandle_use_interface(Max_torrent_handle * handle, BBString * netInterface) {
	char * i = bbStringToCString(netInterface);
	handle->Handle().use_interface(i);
	bbMemFree(i);
}

void bmx_torrent_torrenthandle_pause(Max_torrent_handle * handle) {
	handle->Handle().pause();
}

void bmx_torrent_torrenthandle_resume(Max_torrent_handle * handle) {
	handle->Handle().resume();
}

int bmx_torrent_torrenthandle_is_paused(Max_torrent_handle * handle) {
	return static_cast<int>(handle->Handle().is_paused());
}

int bmx_torrent_torrenthandle_is_seed(Max_torrent_handle * handle) {
	return static_cast<int>(handle->Handle().is_seed());
}

void bmx_torrent_torrenthandle_force_recheck(Max_torrent_handle * handle) {
	handle->Handle().force_recheck();
}

void bmx_torrent_torrenthandle_clear_error(Max_torrent_handle * handle) {
	handle->Handle().clear_error();
}

int bmx_torrent_torrenthandle_is_auto_managed(Max_torrent_handle * handle) {
	return static_cast<int>(handle->Handle().is_auto_managed());
}

void bmx_torrent_torrenthandle_auto_managed(Max_torrent_handle * handle, int value) {
	handle->Handle().auto_managed(static_cast<bool>(value));
}

int bmx_torrent_torrenthandle_has_metadata(Max_torrent_handle * handle) {
	return static_cast<int>(handle->Handle().has_metadata());
}

BBArray * bmx_torrent_torrenthandle_get_peer_info(Max_torrent_handle * handle) {
	std::vector<libtorrent::peer_info> peers;
	handle->Handle().get_peer_info(peers);
	
	int n = peers.size();
	
	if (n == 0) {
		return &bbEmptyArray;
	}
	
	BBArray * arr = _bah_libtorrent_TPeerInfo__newArray(n);
	
	for (int i = 0; i < n; i++) {
		_bah_libtorrent_TPeerInfo__newEntry(arr, i, new Max_peer_info(peers[i]));
	}
	
	return arr;
}

// --------------------------------------------------------

int bmx_torrent_bitfield_get_bit(Max_bitfield * b, int index) {
	return static_cast<int>(b->Bitfield().get_bit(index));
}

int bmx_torrent_bitfield_size(Max_bitfield * b) {
	return b->Bitfield().size();
}

void bmx_torrent_bitfield_free(Max_bitfield * b) {
	delete b;
}

// --------------------------------------------------------

void bmx_torrent_peerinfo_free(Max_peer_info * p) {
	delete p;
}

unsigned int bmx_torrent_peerinfo_flags(Max_peer_info * p) {
	return p->Info().flags;
}


