/*************************************************************************
	> File Name: t.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年10月30日 星期四 10时58分13秒
 ************************************************************************/
#include "jansson.h"

static void dump_error_info(json_error_t * error)
{
	printf("error at line %d column %d pos %d source %s text %s\n", error->line, error->column,
		error->position, error->source, error->text);
};

int main(int argc, char *argv[])
{
	json_error_t error;
	json_t *root = json_load_file(argv[1], JSON_DISABLE_EOF_CHECK, &error);
	if (root == NULL) {
		printf("open file error\n");
		dump_error_info(&error);
		return -1;
	}
#if 0
	json_t *info = json_object_get(root, "system_info");
	if (info == NULL) {
		printf("get info error");
	} else {
		printf("obs size %d\n", json_object_size(info));
		json_t *cmp = json_object_get(info, "comp");
		printf("comp %s\n", json_string_value(cmp));
	}

	json_t *encode = json_object_get(root, "video_encode");
	if (encode == NULL) {
		printf("encode array error\n");
	} else {
		printf("type of encode %d\n", json_typeof(encode));
		json_t *channel1 = json_array_get(encode, 0);
		json_t *storage = json_object_get(channel1, "storage");

		if (storage != NULL) {
			json_t *video_britate = json_object_get(storage, "video_britate");
			if (video_britate)
				printf("video_britate %d\n", json_integer_value(video_britate));
		}

		json_t *nettrans = json_object_get(channel1, "nettrans");

		if (nettrans != NULL) {
			json_t *video_britate = json_object_get(nettrans, "video_britate");
			if (video_britate)
				printf("video_britate %d\n", json_integer_value(video_britate));
		}
	}
#endif
	if (json_dump_file(root, argv[2], JSON_INDENT(4) | JSON_PRESERVE_ORDER) < 0) {
		printf("dump file error");
		return -1;
	}

	json_decref(root);

	return 0;
}
