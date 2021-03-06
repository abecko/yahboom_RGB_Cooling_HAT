/*
BSD 3-Clause License

Copyright (c) 2020, Michal Duda
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "cooling_hat_information.h"

#include <linux/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/statvfs.h>
#include <unistd.h>

#define TEMPERATURE_PATH "/sys/class/thermal/thermal_zone0/temp"

void get_average_load(char *output_value, size_t output_value_size) {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info)) {
        *output_value = '\0';
        return;
    }
    const float average_cpu_load = sys_info.loads[0] / ((float) (1 << SI_LOAD_SHIFT));
    snprintf(output_value, output_value_size, "LA:%.2f", average_cpu_load);
}

void get_ram_usage(char *output_value, size_t output_value_size) {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info)) {
        *output_value = '\0';
        return;
    }
    const unsigned long total_ram = sys_info.totalram >> 20;
    const unsigned long free_ram = sys_info.freeram >> 20;
    snprintf(output_value, output_value_size, "RAM:%.2f%%", free_ram / (double) total_ram);
}

void get_disk_usage(char *output_value, size_t output_value_size) {
    struct statvfs disk_info;
    statvfs("/", &disk_info);
    const unsigned long long total_blocks = disk_info.f_bsize;
    const unsigned long long total_size = total_blocks * disk_info.f_blocks;
    const unsigned long long free_disk = disk_info.f_bfree * total_blocks;
    snprintf(output_value, output_value_size, "HDD:%.2f%%", (free_disk >> 20) / (double) (total_size >> 20));
}

void get_ip_address(char *output_value, size_t output_value_size) {
    struct ifaddrs *if_addrs = NULL;
    *output_value = '\0';

    if (getifaddrs(&if_addrs) == 0) {
        struct ifaddrs *if_addrs_head = if_addrs;
        while (if_addrs != NULL) {
            if (if_addrs->ifa_addr->sa_family == AF_INET) {
                const void *tmp_addr_ptr = &((struct sockaddr_in *) if_addrs->ifa_addr)->sin_addr;
                char address_buffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmp_addr_ptr, address_buffer, INET_ADDRSTRLEN);

                if (strcmp(if_addrs->ifa_name, "eth0") == 0) {
                    snprintf(output_value, output_value_size, "eth0:%s", address_buffer);
                    break;
                } else if (strcmp(if_addrs->ifa_name, "wlan0") == 0) {
                    snprintf(output_value, output_value_size, "wlan0:%s", address_buffer);
                    break;
                }
            }
            if_addrs = if_addrs->ifa_next;
        }
        freeifaddrs(if_addrs_head);
    }
}

void get_fan_speed(char *output_value, size_t output_value_size, enum fan_speed speed) {
    char *text;
    switch (speed) {
        case fan_speed_0_percent:
            text = "OFF";
            break;
        case fan_speed_100_percent:
            text = "MAX";
            break;
        case fan_speed_20_percent:
            text = "20%";
            break;
        case fan_speed_30_percent:
            text = "30%";
            break;
        case fan_speed_40_percent:
            text = "40%";
            break;
        case fan_speed_50_percent:
            text = "50%";
            break;
        case fan_speed_60_percent:
            text = "60%";
            break;
        case fan_speed_70_percent:
            text = "70%";
            break;
        case fan_speed_80_percent:
            text = "80%";
            break;
        case fan_speed_90_percent:
            text = "90%";
            break;
        default:
            text = "ERR";
            break;
    }
    snprintf(output_value, output_value_size, "Fan:%s", text);
}

void get_temperature(char *output_value, size_t output_value_size, double temperature) {
    snprintf(output_value, output_value_size, "Temp:%.1fC", temperature);
}

double get_temperature_double() {
    double temperature = 0;
    int fd_temp = open(TEMPERATURE_PATH, O_RDONLY);
    if (fd_temp >= 0) {
        char buf[32];
        if (read(fd_temp, buf, sizeof(buf)) > 0)
            temperature = atoi(buf) / 1000.0;
        close(fd_temp);
    }
    return temperature;
}


