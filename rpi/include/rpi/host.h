#ifndef RPI_HOST_H
#define RPI_HOST_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <chiaki/controller.h>
#include <chiaki/log.h>
#include <chiaki/opusdecoder.h>
#include <chiaki/regist.h>
#include <chiaki/session.h>

#include <chiaki/discovery.h>
#include <chiaki/discoveryservice.h>

#include <rpi/io.h>
#include <rpi/gui.h>

#define HOSTS_MAX	4		// was 16

//typedef void (*ChiakiDiscoveryServiceCb)(ChiakiDiscoveryHost *hosts, size_t hosts_count, void *user);
static void Discovery(ChiakiDiscoveryHost *, void *);
static void EventCB(ChiakiEvent *event, void *user);

static void DiscoveryCB(void *user);

//class Settings;
//~ class RegisteredHost
//~ {
	//~ public:
		//~ bool registered = false;
		//~ std::string rpKey_morning;
		//~ std::string regist_key;
		//~ std::string nick_name;
		//~ /// id/mac
		
	//~ private:
	
//~ };



class Host
{
	private:
		ChiakiConnectVideoProfile video_profile;
		
		ChiakiAudioSink audio_sink; //needs to be pointer/persistent?
		
		/// REGISTERING
		ChiakiRegistInfo regist_info = {};
		ChiakiRegist regist;
		size_t accId_len;
		
	public:
		ChiakiLog log;
		IO *io = nullptr; ///should be private
		NanoSdlWindow *gui = nullptr;
		ChiakiConnectInfo connect_info;
		bool session_init = false;
		ChiakiSession session;
		ChiakiOpusDecoder opus_decoder;
		ChiakiTarget target = CHIAKI_TARGET_PS4_UNKNOWN;
		std::string state = "";  /// standby, unknown, ready etc
		ChiakiDiscoveryService *service;
		ChiakiDiscoveryHost *discoveredHosts = nullptr;

		ChiakiThread play_th;  ///should be private? but harder to do
		
		Host();
		~Host();
		int Init(IO *io);
		int StartDiscoveryService();
		void RegistStart(std::string accountID, std::string pin);
		int StartSession();
		void Play();
		std::string GetHostRPKey(uint8_t rp_key[]);
		
		void DiscoveryCB(ChiakiDiscoveryHost *discovered_host);
		void ConnectionEventCB(ChiakiEvent *event);

};
#endif