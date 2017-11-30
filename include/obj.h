#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

typedef struct obj_v3
{
	float x;
	float y;
	float z;
} obj_v3_t;

typedef struct obj_uv
{
	float u;
	float v;
} obj_uv_t;

typedef struct obj_vertex
{
	size_t v_idx;
	size_t vt_idx;
	size_t vn_idx;
} obj_vertex_t;

typedef struct obj_face
{
	obj_vertex_t v1;
	obj_vertex_t v2;
	obj_vertex_t v3;
} obj_face_t;

typedef struct obj_ctx
{
	obj_v3_t *v;
	obj_v3_t *vn;
	obj_uv_t *vt;
	obj_face_t *faces;
	size_t v_cnt;
	size_t vn_cnt;
	size_t vt_cnt;
	size_t faces_cnt;
} obj_ctx_t;

int obj_parse_v3(char *, size_t, obj_v3_t *);
int obj_parse_vertex(char *, size_t, obj_vertex_t *);
int obj_parse_face(char *, size_t, obj_face_t *);
int obj_parse_line(char *, size_t, obj_ctx_t *);
void obj_parser_init(obj_ctx_t *);
void obj_parser_destroy(obj_ctx_t *);
