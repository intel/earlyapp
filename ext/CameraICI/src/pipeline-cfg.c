////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
// SPDX-License-Identifier: MIT
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <fcntl.h>	/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <stdbool.h>

#include "ici.h"


#ifdef LOGPRINT
#define PRINT_DBG(fmt, args...) printf(fmt, ## args)
#else
#define PRINT_DBG(fmt, args...)
#endif

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))
static int fd = -1;

static char* find_node_by_id(int node);
void get_node_list();
void set_link(int pipeline_fd,
		struct ici_link_desc* link_desc, int reset);

struct print_info{
	char *name;
	enum ici_format code;
};

char* find_enum_by_code(int code, struct print_info *info_array,
		int size);

struct print_info sel_list[] = {
	{"0", 0},
	{"ICI_EXT_SEL_TYPE_NATIVE", ICI_EXT_SEL_TYPE_NATIVE},
	{"ICI_EXT_SEL_TYPE_CROP", ICI_EXT_SEL_TYPE_CROP},
	{"ICI_EXT_SEL_TYPE_CROP_BOUNDS", ICI_EXT_SEL_TYPE_CROP_BOUNDS},
	{"ICI_EXT_SEL_TYPE_COMPOSE", ICI_EXT_SEL_TYPE_COMPOSE},
	{"ICI_EXT_SEL_TYPE_COMPOSE_BOUNDS", ICI_EXT_SEL_TYPE_COMPOSE_BOUNDS},
};

struct print_info fmt_list[] = {
	{"0", 0},
	{"ICI_FORMAT_RGB888", ICI_FORMAT_RGB888},
	{"ICI_FORMAT_RGB565", ICI_FORMAT_RGB565},
	{"ICI_FORMAT_UYVY", ICI_FORMAT_UYVY},
	{"ICI_FORMAT_YUYV", ICI_FORMAT_YUYV},
	{"ICI_FORMAT_SBGGR12", ICI_FORMAT_SBGGR12},
	{"ICI_FORMAT_SGBRG12", ICI_FORMAT_SGBRG12},
	{"ICI_FORMAT_SGRBG12", ICI_FORMAT_SGRBG12},
	{"ICI_FORMAT_SRGGB12", ICI_FORMAT_SRGGB12},
	{"ICI_FORMAT_SBGGR10", ICI_FORMAT_SBGGR10},
	{"ICI_FORMAT_SGBRG10", ICI_FORMAT_SGBRG10},
	{"ICI_FORMAT_SGRBG10", ICI_FORMAT_SGRBG10},
	{"ICI_FORMAT_SRGGB10", ICI_FORMAT_SRGGB10},
	{"ICI_FORMAT_SBGGR8", ICI_FORMAT_SBGGR8},
	{"ICI_FORMAT_SGBRG8", ICI_FORMAT_SGBRG8},
	{"ICI_FORMAT_SGRBG8", ICI_FORMAT_SGRBG8},
	{"ICI_FORMAT_SRGGB8", ICI_FORMAT_SRGGB8},
	{"ICI_FORMAT_SBGGR10_DPCM8", ICI_FORMAT_SBGGR10_DPCM8},
	{"ICI_FORMAT_SGBRG10_DPCM8", ICI_FORMAT_SGBRG10_DPCM8},
	{"ICI_FORMAT_SGRBG10_DPCM8", ICI_FORMAT_SGRBG10_DPCM8},
	{"ICI_FORMAT_SRGGB10_DPCM8", ICI_FORMAT_SRGGB10_DPCM8},
	{"ICI_FORMAT_COUNT ", ICI_FORMAT_COUNT},
};

struct print_info field_info[] = {
	{"ICI_FIELD_ANY",ICI_FIELD_ANY},
	{"ICI_FIELD_NONE", ICI_FIELD_NONE},
	{"ICI_FIELD_TOP", ICI_FIELD_TOP},
	{"ICI_FIELD_BOTTOM", ICI_FIELD_BOTTOM},
	{"", ICI_FIELD_BOTTOM + 1},
	{"", ICI_FIELD_BOTTOM + 2},
	{"", ICI_FIELD_BOTTOM + 3},
	{"ICI_FIELD_ALTERNATE", ICI_FIELD_ALTERNATE},
};

struct ici_node_desc* node_list = NULL;
int nodes_num = 0;

/*=====*/
static void close_device(void)
{
	if (-1 == close(fd))
		fprintf(stderr, "%s error %d, %s\n", "close", errno, strerror(errno));;

	fd = -1;
}

static void errno_exit(const char *s, int error)
{
	errno = error;
	fprintf(stderr, "%serror %d, %s\n", s, errno, strerror(errno));
	 if(fd != -1)
		close_device();
	if(node_list) {
		free(node_list);
	}   
}

static int xioctl(int fh, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

static int open_pipe_device(bool w4pipeline)
{
	struct stat st;
	char dev_name[256];
	if(fd != -1)
		return 1;
	sprintf(dev_name, "/dev/%s", ICI_PIPELINE_DEVICE_NAME);
	printf("Opening device \"%s\"...\n", dev_name);
	while (w4pipeline && (access(dev_name, R_OK) != 0)) {
		usleep(1000);
	}
	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
				dev_name, errno, strerror(errno));
		return 0;
	}
	return 1;
}

static void print_pad_props(struct ici_pad_framefmt *props)
{
	printf("\t \t format: %dx%d, %s, %s, %d, %x \n",
			props->ffmt.width, props->ffmt.height,
			find_enum_by_code(props->ffmt.pixelformat, fmt_list, ARRAY_SIZE(fmt_list)),
			field_info[props->ffmt.field].name,
			props->ffmt.colorspace, props->ffmt.flags);
}

static void reset_active_links(void)
{
	int i;
	struct ici_node_desc *node_info;

	if(!node_list)
		get_node_list();

	for (i=0; i < nodes_num; i++) {
		int p;
		node_info = &node_list[i];
		for (p = 0; p < node_info->nr_pads; p++) {
			int f, ret;
			struct ici_links_query links_query = {0};

			links_query.pad.node_id = node_info->node_id;
			links_query.pad.pad_idx = node_info->node_pad[p].pad_idx;

			ret = xioctl(fd, ICI_IOC_ENUM_LINKS, &links_query);
			if (ret < 0 ) {
				fprintf(stderr, "error(%d) querying the links on node:%d pad_idx:%d\n", ret,
					node_info->node_id, node_info->node_pad[p].pad_idx);
                        }

			for (f = 0; f < links_query.links_cnt; f++ ) {
				if ((links_query.links[f].flags & ICI_LINK_FLAG_ENABLED)
					&& !(links_query.links[f].flags & ICI_LINK_FLAG_BACKLINK))
				{
					set_link(fd, &links_query.links[f], 1);
				}
			}
		}
	}
}

void set_link(int pipeline_fd,
	struct ici_link_desc* link_desc, int reset)
{
	if (reset) {
		link_desc->flags = 0;
		printf(" Resetting link %s -> %s\n",
			find_node_by_id(link_desc->source.node_id),
			find_node_by_id(link_desc->sink.node_id));
	} else {
		printf(" Setting link %s -> %s\n",
			find_node_by_id(link_desc->source.node_id),
			find_node_by_id(link_desc->sink.node_id));
	}

	if (xioctl(pipeline_fd, ICI_IOC_SETUP_PIPE,
			link_desc) < 0 ) {
		fprintf(stderr, "Error on set link source %d:%d sink %d:%d\n",
				link_desc->source.node_id, link_desc->source.pad_idx,
				link_desc->sink.node_id, link_desc->sink.pad_idx);
		errno_exit("", errno);
	}
}

static void set_pad_format(int pipeline_fd,
		struct ici_pad_framefmt *pad_ffmt)
{
	printf("\nSetting \"%s\" %d:%d: \n",
			find_node_by_id(pad_ffmt->pad.node_id),
			pad_ffmt->pad.node_id, pad_ffmt->pad.pad_idx);

	print_pad_props(pad_ffmt);

	if (xioctl(pipeline_fd, ICI_IOC_SET_FRAMEFMT,
			pad_ffmt) < 0 ) {
		errno_exit("error on set format\n", errno);
	}
	printf("Format set: \n");
	print_pad_props(pad_ffmt);

	PRINT_DBG("Node-\"%s\" %d:%d:\n", find_node_by_id(pad_ffmt->pad.node_id),
			 pad_ffmt->pad.node_id, pad_ffmt->pad.pad_idx);
#ifdef LOGPRINT
	get_pad_format(pipeline_fd, pad_ffmt->pad.node_id,
			pad_ffmt->pad.pad_idx);
#endif
}

char pipeline_usage []= "Usage: \n"
						"\t -L \t\t\t\t List Nodes \n"
						"\t -f node:pad FORMAT \t\t Set format \n"
						"\t -l src_node:pad dest_node:pad \t Set link \n"
						"\t -g node:pad \t\t\t Get format \n"
						"\t -s node:pad SELECTION \t\t set selection for node \n"
						"\t -S node:pad \t\t\t Get selection for node \n"
						"\t -r \t\t\t\t Reset links\n"
						"\t -n node \t\t\t Print node info\n"
						"\n\n Where the arguments are:\n"
						"\t FORMAT -\t\t [ffmt:wxh, pixelformat, field, colorspace, flags] \n"
						"\t SELECTION - \t\t [type:sel_type][rect:top,left,width,height] \n"
						"\t node - \t\t node id \n"
						"\t pad - \t\t\t pad id \n"
						"\n\n Example: \n"
						"\t List nodes \t\t\t\t\t\t pipeline-cfg -L\n"
						"\t Get format for adv7481 hdmi binner at pad 0 \t\t pipeline-cfg -g 'adv7481 hdmi binner:0'\n"
						"\t Set format for adv7481 hdmi binner at pad 0 \t\t pipeline-cfg -f 'adv7481 hdmi binner:0"
						" [ffmt:720x240,ICI_FORMAT_UYVY, ICI_FIELD_ALTERNATE, 0, 0]'\n"
						"\t Set selection for adv7481 hdmi binner at pad 0 \t pipeline-cfg -s 'adv7481 hdmi binner:0"
						" [type:ICI_EXT_SEL_TYPE_CROP][rect:0,0,720,240]'\n"
						"\t Get selection for adv7481 hdmi binner at pad 0 \t pipeline-cfg -S 'adv7481 hdmi binner:0"
						" [type:ICI_EXT_SEL_TYPE_CROP]'\n"
						"\t Set link from source node adv7481 hdmi pixel array:0 to\n\t sink node adv7481 hdmi binner:0 \t\t\t pipeline-cfg -l 'adv7481 hdmi pixel array:0 adv7481 hdmi binner:0[enabled]'\n"
						"\t Get node info for node adv7481 hdmi pixel array \t pipeline-cfg -n 'adv7481 hdmi pixel array'\n";


void get_node_list()
{
	struct ici_node_desc node_info_count = {0};
	struct ici_node_desc *node_info;

	if(open_pipe_device(false))
        {
            node_info_count.node_id = -1;
            if (xioctl(fd, ICI_IOC_ENUM_NODES, &node_info_count) == -1 ) {
                fprintf(stderr, "Unable to get the nodes count\n");
                errno_exit("get_node_list - Unable to get the nodes count", errno);
            }
            PRINT_DBG("total nodes %d\n", node_info_count.node_count);
            if(!node_list) {
		node_list = (struct ici_node_desc *)calloc(node_info_count.node_count,
                                                           sizeof(struct ici_node_desc));
		if(!node_list)
                    errno_exit("", errno);
            }
            for (nodes_num = 0; nodes_num < node_info_count.node_count; nodes_num++) {
		node_info = &node_list[nodes_num];
		node_info->node_id = nodes_num;
		if (xioctl(fd, ICI_IOC_ENUM_NODES, node_info) == -1 )
                    errno_exit("Error calling ICI_IOC_ENUM_NODES\n", errno);
            }
            PRINT_DBG("nodes_num %d \n", nodes_num);
        }
        else
        {
            PRINT_DBG("Failed to open device\n");
        }
}

int find_node_by_name(const char *name)
{
	int i = 0;
	if(!node_list)
		get_node_list();

	for(i = 0; i < nodes_num; i++) {
		if(strcmp(node_list[i].name, name) == 0)
			return node_list[i].node_id;
	}
	return -1;
}


static char* find_node_by_id(int node)
{
	int i = 0;
	if(!node_list)
		get_node_list();

	for(i = 0; i < nodes_num; i++) {
		if(node_list[i].node_id == node) {
			return node_list[i].name;
		}
	}
	return NULL;
}

int find_enum_by_name(const char *name, struct print_info *info_array,
		int size)
{
	int i;

	for(i = 0; i < size; i++) {
		if(strcmp(info_array[i].name, name) == 0)
			return info_array[i].code;
	}

	errno_exit("Value not found", EINVAL);
	return -1;
}

char* find_enum_by_code(int code, struct print_info *info_array,
		int size)
{
	int i;

	for(i = 0; i < size; i++) {
		if(info_array[i].code == code) {
			return info_array[i].name;
		}
	}
	return NULL;
}

enum ici_format find_field_by_name(const char *name)
{
	int i;
	for(i = 0; i <= ICI_FIELD_ALTERNATE; i++) {
		if(strcmp(field_info[i].name, name) == 0)
			return field_info[i].code;
	}
	errno_exit("Invalid field value! \n", EINVAL);
	return -1;
}

int tokenize(char *str, char *delim, int expected, char **toks)
{
	char *token;
	int num = 0;
	token = strtok(str, delim);
	do {
		PRINT_DBG("token %d %s \n", num, token);
		toks[num++] = token;
		token = strtok(NULL, delim);
	} while(token && num < expected);
	return num;
}

int get_pad_id(const char *pad_str)
{
	if(!isdigit(*pad_str)) {
		errno_exit("Invalid pad index! \n", EINVAL);
	} else
		return atoi(pad_str);

	return -1;
}

static void parse_formats(char *ffmt_str,
		struct ici_framefmt *ffmt)
{
	/*Get the format string*/
	int num = 6;
	char *toks[num];

	memset(toks,0,sizeof(char*)*num);
	if (num != tokenize(ffmt_str, ":x, ", num, toks))
		errno_exit("Incomplete number of args \n", EINVAL);


	if(!isdigit(*toks[0]) || !isdigit(*toks[1]) || !isdigit(*toks[4]) ||
			!isdigit(*toks[5]))
		errno_exit("Invalid format args", EINVAL);

	ffmt->width = strtoul(toks[0], NULL, 10);
	ffmt->height = strtoul (toks[1], NULL,10);
	ffmt->pixelformat = find_enum_by_name(toks[2], fmt_list, ARRAY_SIZE(fmt_list));
	ffmt->field = find_enum_by_name(toks[3], field_info, ARRAY_SIZE(field_info));
	ffmt->colorspace = atoi(toks[4]);
	ffmt->flags = atoi(toks[5]);

	PRINT_DBG("Parse formats w:h:pxlfmt:field:colorspace:flags %d:%d:%d:%d:%d:%d \n",
			ffmt->width, ffmt->height, ffmt->pixelformat, ffmt->field,
			ffmt->colorspace, ffmt->flags);
}

static void parse_pad_desc(char *pad_str, struct ici_pad_desc *pad_desc)
{
	/*Get the pad values*/
	char *toks[2];

	memset(toks,0,(sizeof(char*)*2));
	if (tokenize(pad_str, ":", 2, toks) != 2)
		errno_exit("Incomplete number of node args \n", EINVAL);

	pad_desc->node_id = find_node_by_name(toks[0]);
	pad_desc->pad_idx = get_pad_id(toks[1]);

	PRINT_DBG("\nPad desc %d:%d \n", pad_desc->node_id,
			pad_desc->pad_idx);
}

static void get_arg_val(const char * src, const char *arg_name,
		char end_char, char *arg_val)
{
	char *end_pos;
	char *pos = strstr(src, arg_name);
	int length = 0;

	if(!pos) {
		printf("%s' string not found\n", arg_name);
		errno_exit("", EINVAL);
		return;
	}

	PRINT_DBG("'%s' string found! \n", arg_name);
	end_pos = strchr(pos, end_char);
	if(!end_pos)
		errno_exit(" Invalid argument - End char not found\n", EINVAL);

	length = end_pos - (pos + strlen(arg_name));

	if(length == 0)
		errno_exit("", EINVAL);

	strncpy(arg_val, pos + strlen(arg_name), length);
	arg_val[length] = '\0';

	PRINT_DBG("Arg val %s \n", arg_val);
}

static void do_setformat(char *opt_arg)
{
	struct ici_pad_framefmt pad_props = {0};
	char arg_name[250] = {0};

	char *pos = strchr(opt_arg, '[');

	if(!pos)
		errno_exit("Invalid format arguments \n", EINVAL);

	/* Get and parse pad*/
	strncpy(arg_name, opt_arg, pos - opt_arg);
	parse_pad_desc(arg_name, &pad_props.pad);

	/* Get and parse ffmt*/
	get_arg_val(opt_arg, "ffmt:", ']', arg_name);
	parse_formats(arg_name, &pad_props.ffmt);

	if(open_pipe_device(false))
        {
            set_pad_format(fd, &pad_props);
        }
}

#define MAX_NUM_SET_FORMAT_ARGS 10
void do_setlink(char *opt_arg)
{
	struct ici_link_desc link_desc = {0};
	int num = 2;
	char *toks[4];
	char arg_val[250], src_node[250], dest_node[250];
	char *pos = strchr(opt_arg, '[');
	char *pos2 = strchr(opt_arg, ']');
	int length = 0;
	int idx = 0;
	int arg_length = strlen(opt_arg);

	if(pos && pos2) {
		length = pos2 - (pos+1);
		strncpy(arg_val, pos+1, length);
		arg_val[length] = '\0';

		if(strstr(arg_val, "enabled"))
			link_desc.flags |= ICI_LINK_FLAG_ENABLED;
		else
			errno_exit("Flags not set\n", EINVAL);

		arg_length = pos - opt_arg;
	} else if(pos) {
		link_desc.flags |= ICI_LINK_FLAG_ENABLED;
		arg_length = pos - opt_arg;
	}

	strncpy(arg_val, opt_arg, arg_length);
	arg_val[arg_length] = '\0';

	/* Find 1st occurance of ":" */
	pos = strchr(arg_val, ':');
	if (!pos) {
		errno_exit("Invalid parameter \n", EINVAL);
	}

	idx = (int)(pos-arg_val);
	length = strlen(arg_val);

	/* Get source node */
	strncpy(src_node, arg_val, idx+2);
	src_node[idx+2] = '\0';

	/* Get destination node */
	idx += 3; //Get 1st character of the destination node */
	strncpy(dest_node, arg_val+idx,length-(idx+1));
	dest_node[length-(idx+1)] = '\0';

	PRINT_DBG("Links String %s \n", arg_val);

	/* Get node name and pad id for source node */
	if (tokenize(src_node, ":", num, toks) != num) {
		errno_exit("Incomplete number of args \n", EINVAL);
	}

	/* Get node name and pad id for destination node */
	if (tokenize(dest_node, ":", num, toks+2) !=num) {
		errno_exit("Incomplete number of args \n", EINVAL);
	}

	link_desc.source.node_id = find_node_by_name(toks[0]);
	link_desc.source.pad_idx = get_pad_id(toks[1]);
	link_desc.sink.node_id = find_node_by_name(toks[2]);
	link_desc.sink.pad_idx = get_pad_id(toks[3]);

        if(open_pipe_device(false))
        {
            set_link(fd, &link_desc, 0);
        }
}

void parse_args(char argc, char *strtext)
{
	char c = argc;

	switch(c) {
		case 'r':
			printf("reset_active_links");
			reset_active_links();
		break;
		case 'l':
			printf("Set link arguments %s\r\n", strtext);
			do_setlink(strtext);
		break;
		case 'f':
			printf("set format %s\r\n", strtext);
			do_setformat(strtext);
		break;
		default:
		break;
	}
}

int ConfigureICI(bool w4pipline)
{
	// this configuration is for GP2.0 ici
    if(open_pipe_device(w4pipline))
    {
        parse_args('r',"");
        parse_args('f',"Intel IPU4 CSI2 BE SOC 0:0 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('f',"Intel IPU4 CSI2 BE SOC 0:1 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('f',"Intel IPU4 CSI-2 4 VC 0:0 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('f',"Intel IPU4 CSI-2 4 VC 0:1 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('f',"adv7481 cvbs binner:0 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('f',"adv7481 cvbs binner:1 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('f',"adv7481 cvbs pixel array:0 [ffmt:720x240,ICI_FORMAT_UYVY,ICI_FIELD_NONE,0,0]");
        parse_args('l',"adv7481 cvbs pixel array:0 adv7481 cvbs binner:0 [enabled]");
        parse_args('l',"adv7481 cvbs binner:1 Intel IPU4 CSI-2 4 VC 0:0 [enabled]");
        parse_args('l',"Intel IPU4 CSI-2 4 VC 0:1 Intel IPU4 CSI2 BE SOC 0:0 [enabled]");
        parse_args('l',"Intel IPU4 CSI2 BE SOC 0:1 Intel IPU4 CSI2 BE SOC 0 Stream:0 [enabled]");
        return 1;
    }

	return 0;
}
