/*   This file is part of hpkg/hardman.
 *
 *   hpkg/hardman is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   hpkg/hardman is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with hpkg/hardman.  If not, see <https://www.gnu.org/licenses/>.
 * -------------------------------------------------------------------------
 *   Copyright (C) 2021 Arseniy 'EmptiedSoul' Lesin
 * -------------------------------------------------------------------------
 *   Daria - Dandified Aria2 -- libaria2-based high-speed utility to fetch packages and its deltas in parallel
 *           ^         ^^^^
 *   Invokation:
 *	echo <space-separated list of urls> | /usr/libexec/hardman/daria <opts>
 * -------------------------------------------------------------------------
 *  Ideally, this program should been written in C, but aria2 only has C++ bindings :(
 *  So, there is an ugly mix of C++ and C, enjoy!
 */

// C++ Headers
#include <iostream>
#include <string>
#include <chrono>
#include <cstring>
#include <mutex>

// C Headers
#include <cstdlib>
#include <cstdio>
#include <libintl.h>
#include <locale.h>
#include <unistd.h>
#include <sys/ioctl.h>

extern "C" {
	char** hrd_string_split(char*, char*);
	size_t hrd_string_discard_chars(char*, char);
}

int silent;
int no_color;

std::mutex print_mtx;
std::mutex bool_mtx;
bool onDownloadWasCalled = false; 

#define _(x)	gettext(x)
#define add_option(x,y) options.push_back(std::pair<std::string, std::string> (x, y))

// Aria2 Headers
#include <aria2/aria2.h>

#include "../../chpkg/hpkglib.h" // for hpkg-like messages macros

// Finds last occurance of needle in haystack and returns pointer to it
// Dont know why it is not standard function
static char* strrstr(const char* haystack, const char* needle) {
	if (needle == NULL) 
		return (char *) haystack;
	char *rv = NULL;
	while (1) {
		char *p = (char*)strstr(haystack, needle);
		if (p == NULL)
			break;
		rv = p;
		haystack = p + strlen(needle);
	}
	return rv;
}

int onDownload(aria2::Session* session, aria2::DownloadEvent event, aria2::A2Gid gid, void* __attribute__((unused))userData){
	// Should i delete or free() any strings here?
	bool_mtx.lock();
	onDownloadWasCalled = true;
	bool_mtx.unlock();
	char*			filename = NULL;
	char*			real_filename = NULL;
	char			total_measurement[4]; // Longest -> "GiB"   + '\0' 
	char			speed_measurement[6]; // Longest -> "GiB/s" + '\0'
	unsigned long long	total_bytes;
	float			total_units;
	int			fetch_speed;
	float			fetch_speed_in_units;
	std::string		tmp;
	setlocale(LC_ALL, "");
	bindtextdomain("hpkg", "/usr/share/locale");
	textdomain("hpkg");

	aria2::DownloadHandle* 	dh = aria2::getDownloadHandle(session, gid);
	if (!dh) 
		return 0;
	if (dh->getNumFiles() > 0) {
		aria2::FileData f = dh->getFile(1);
		if (f.path.empty()) {
			if (!f.uris.empty()) {
				filename = strdup(f.uris[0].uri.c_str());
			}
		} else {
			filename = strdup(f.path.c_str());
		}
		real_filename = strrstr(filename, "/") + sizeof(char);
	}
	total_bytes = dh->getTotalLength();
	fetch_speed = dh->getDownloadSpeed();
	int offset_from_right = (1 + 6 + 1 + 3 + 1 + 6 + 1 + 6 + 1);
	switch (event) {
	case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
		if (total_bytes >= 1024 && total_bytes < 1024*1024) {
			strcpy(total_measurement, "KiB");
			total_units = total_bytes / 1024.0f;
		} else if (total_bytes >= 1024*1024 && total_bytes < 1024*1024*1024) {
			strcpy(total_measurement, "MiB");
			total_units = total_bytes / 1024.0f / 1024.0f;
		} else if (total_bytes >= 1024*1024*1024) {
			strcpy(total_measurement, "GiB");
			total_units = total_bytes / 1024.0f / 1024.0f / 1024.0f;
		} else {
			strcpy(total_measurement, "  B");
			total_units = total_bytes;
		}
		if (fetch_speed >= 1024 && fetch_speed < 1024*1024) {
			strcpy(speed_measurement, "KiB/s");
			fetch_speed_in_units = fetch_speed / 1024.0f;
		} else if (fetch_speed >= 1024*1024 && fetch_speed < 1024*1024*1024) {
			strcpy(speed_measurement, "MiB/s");
			fetch_speed_in_units = fetch_speed / 1024.0f / 1024.0f;
		} else if (fetch_speed >= 1024*1024*1024) {
			strcpy(speed_measurement, "GiB/s");
			fetch_speed_in_units = fetch_speed / 1024.0f / 1024.0f / 1024.0f;
		} else {
			strcpy(speed_measurement, "  B/s");
			fetch_speed_in_units = fetch_speed;
		}
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		print_mtx.lock();
		printf("\e[2K\r");
		pointer("%s %s\e[%dC\e[%dD %6.1f %s %6.1f %s ",
					_("Fetched"),
					real_filename,
					w.ws_col,
					offset_from_right,
					total_units,
					total_measurement,
					fetch_speed_in_units,
					speed_measurement
					);
		print_mtx.unlock();
		break;
	case aria2::EVENT_ON_DOWNLOAD_ERROR:
		print_mtx.lock();
		printf("\e[2K\r");
		warn("%s %s. %s: %d\n", _("Error occured while fetching"), real_filename,
				      _("Error code"), dh->getErrorCode());
		print_mtx.unlock();
		break;
	}
	aria2::deleteDownloadHandle(dh);
	free(filename);
	return 0;
}

int main(int argc, char** argv) {
	int rv;
	char* input_line;
	size_t input_len = 0;
	char** url_list;
	if (argc < 2) {
		std::cerr << "Usage: echo <urls> | daria <dir>" << std::endl;
		exit(1);
	}
	chdir(argv[1]);
	getline(&input_line, &input_len, stdin);
	if (!input_line) {
		fprintf(stderr, "No input supplied!\n");
		exit(32);
	}
	hrd_string_discard_chars(input_line, '\n');
	url_list = hrd_string_split(input_line, " ");
	free(input_line);
	if (getenv("NO_COLOR") != NULL) 
		no_color=1;
	if (argv[2] != NULL && !strcmp(argv[2], "-s"))
		silent=1;
	setlocale(LC_ALL, "");
	bindtextdomain("hpkg", "/usr/share/locale");
	textdomain("hpkg");
	aria2::libraryInit();
	aria2::Session* session;
	aria2::SessionConfig config;
	config.downloadEventCallback = onDownload;
	aria2::KeyVals options;
	add_option("file-allocation", "none");
	add_option("max-connection-per-server", "4");
	add_option("continue", "true");
	session = aria2::sessionNew(options, config);
	for (int i = 0; url_list[i]; i++) {
		std::vector<std::string> uris = {url_list[i]};
		rv = aria2::addUri(session, nullptr, uris, options);
		if (rv < 0) {
			std::cerr << "Falied to add download " << uris[0] << std::endl;
		}
	}
	auto start = std::chrono::steady_clock::now();
	for (;;) {
		rv = aria2::run(session, aria2::RUN_ONCE);
		if (rv != 1) {
			break;
		}
		auto now = std::chrono::steady_clock::now();
		auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
		if (count >= 500 || onDownloadWasCalled) {
			if (onDownloadWasCalled) {
				bool_mtx.lock();
				onDownloadWasCalled = false;
				bool_mtx.unlock();
			} else
				start = now;
			aria2::GlobalStat gstat = aria2::getGlobalStat(session);
			print_mtx.lock();
			struct winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			int			fetch_speed;
			float			fetch_speed_in_units;
			char			speed_measurement[6];
			if (gstat.numActive != 0) {
				fetch_speed = gstat.downloadSpeed;
				if (fetch_speed >= 1024 && fetch_speed < 1024*1024) {
					strcpy(speed_measurement, "KiB/s");
					fetch_speed_in_units = fetch_speed / 1024.0f;
				} else if (fetch_speed >= 1024*1024 && fetch_speed < 1024*1024*1024) {
					strcpy(speed_measurement, "MiB/s");
					fetch_speed_in_units = fetch_speed / 1024.0f / 1024.0f;
				} else if (fetch_speed >= 1024*1024*1024) {
					strcpy(speed_measurement, "GiB/s");
					fetch_speed_in_units = fetch_speed / 1024.0f / 1024.0f / 1024.0f;
				} else {
					strcpy(speed_measurement, "  B/s");
					fetch_speed_in_units = fetch_speed;
				}
				printf("\e[2K\r(A: %d) %5.2f %s ", gstat.numActive, fetch_speed_in_units, speed_measurement);
			}
			std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session);
			for (const auto& gid : gids) {
				aria2::DownloadHandle* dh = aria2::getDownloadHandle(session, gid);
				if (dh) {
					int percentage = (dh->getTotalLength() > 0 
							? (100 * dh->getCompletedLength() /
								dh->getTotalLength())
							: 0);
					int free_cols  = (w.ws_col - 20 - (7*gstat.numActive))/gstat.numActive;
					int sharp_cols = free_cols * (percentage/100.0f);
					int space_cols = free_cols - sharp_cols;
					printf("[");
					for (int j = 0; j<sharp_cols; j++) {
						printf("#");
					}
					for (int j = 0; j<space_cols; j++) {
						printf(" ");
					}
					printf("]%3d%% ", percentage);
					aria2::deleteDownloadHandle(dh);
        			}
      			}
			fflush(stdout);
			print_mtx.unlock();
		}
	}
	rv = aria2::sessionFinal(session);
	aria2::libraryDeinit();
	return rv;
}
