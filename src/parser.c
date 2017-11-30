#include <obj.h>

// zero-copy space splitter to float
int obj_parse_v3(char *line, size_t slen, obj_v3_t *v3)
{
	// get pointers to struct members
	float *vfields[3];
	vfields[0] = (float *)((void *)v3 + offsetof(obj_v3_t, x));
	vfields[1] = (float *)((void *)v3 + offsetof(obj_v3_t, y));
	vfields[2] = (float *)((void *)v3 + offsetof(obj_v3_t, z));

	size_t field_index = 0;

	// ptr always points to the current split-part
	char *ptr = line;
	for(size_t i=0;i<slen;i++)
	{
		if (isspace(line[i]) || i+1 == slen)
		{
			// strtof will set errno accordingly
			errno = 0;
			char *next_ptr = NULL;
			*vfields[field_index++] = strtof(ptr, &next_ptr);
			if (errno != 0 || ptr == next_ptr)
				return -1;

			ptr = next_ptr;

			// we have z !
			if (field_index > 2)
				break;
		}
	}

	return field_index == 3 ? 0 : -1;
}

static int _obj_strtoidx(char *str, size_t slen, size_t *num)
{
	*num = 0;

	for (size_t i = 0; i < slen; i++) {
		if (!isdigit(str[i]))
			break;
		size_t delta = pow(10, i);
		*num += delta * (str[i] - 48);
	}

	return 0;
}

// zero-copy slash splitter to index (size_t)
int obj_parse_vertex(char *line, size_t slen, obj_vertex_t *v)
{
	// get pointers to struct members
	size_t *vfields[3];
	vfields[0] = (size_t *)((void *)v + offsetof(obj_vertex_t, v_idx));
	vfields[1] = (size_t *)((void *)v + offsetof(obj_vertex_t, vt_idx));
	vfields[2] = (size_t *)((void *)v + offsetof(obj_vertex_t, vn_idx));

	size_t field_index = 0;

	// ptr always points to the current split-part
	char *ptr = line;
	for(size_t i=0;i<slen;i++)
	{
		if (line[i] == '/' || isspace(line[i]) || i+1 == slen)
		{
			if (_obj_strtoidx(ptr, slen - (ptr - line), vfields[field_index++]))
				return -1;

			ptr = line + i + 1;

			if (field_index > 2)
				break;
		}
	}

	return field_index == 3 ? 0 : -1;
}

// zero-copy space splitter to face (obj_face_t)
int obj_parse_face(char *line, size_t slen, obj_face_t *f)
{
	// get pointers to struct members
	obj_vertex_t *vfields[3];
	vfields[0] = (obj_vertex_t *)((void *)f + offsetof(obj_face_t, v1));
	vfields[1] = (obj_vertex_t *)((void *)f + offsetof(obj_face_t, v2));
	vfields[2] = (obj_vertex_t *)((void *)f + offsetof(obj_face_t, v3));

	size_t field_index = 0;

	// ptr always points to the current split-part
	char *ptr = line;
	for(size_t i=0;i<slen;i++)
	{
		if (isspace(line[i]) || i+1 == slen)
		{
			if (obj_parse_vertex(ptr, i - (ptr - line), vfields[field_index++]))
				return -1;

			ptr = line + i + 1;

			if (field_index > 2)
				break;
		}
	}

	return field_index == 3 ? 0 : -1;
}

static obj_v3_t *_obj_increase_memory_for_v3(obj_v3_t *ptr, size_t items)
{
	return (obj_v3_t *) realloc(ptr, items * (sizeof(obj_v3_t) + 1));
}

static obj_face_t *_obj_increase_memory_for_face(obj_face_t *ptr, size_t items)
{
	return (obj_face_t *) realloc(ptr, items * (sizeof(obj_face_t) + 1));
}

int obj_parse_line(char *line, size_t slen, obj_ctx_t *ctx)
{
	if (slen < 2)
		return 0;

	if (line[0] == 'v' && line[1] == ' ')
	{
		ctx->v_cnt++;
		obj_v3_t *new_chunk = _obj_increase_memory_for_v3(ctx->v, ctx->v_cnt);
		if (!new_chunk)
			return -1;
		ctx->v = new_chunk;
		if (obj_parse_v3(line + 2, slen - 2, &ctx->v[ctx->v_cnt-1]))
		{
			return -1;
		}
		return 0;
	}

	if (line[0] == 'f' && line[1] == ' ')
	{
		ctx->faces_cnt++;
		obj_face_t *new_chunk = _obj_increase_memory_for_face(ctx->faces, ctx->faces_cnt);
		if (!new_chunk)
			return -1;
		ctx->faces = new_chunk;
		if (obj_parse_face(line + 2, slen - 2, &ctx->faces[ctx->faces_cnt-1]))
		{
			return -1;
		}
		return 0;
	}

	if (slen < 3)
		return 0;

	if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
	{
		ctx->vn_cnt++;
		obj_v3_t *new_chunk = _obj_increase_memory_for_v3(ctx->vn, ctx->vn_cnt);
		if (!new_chunk)
			return -1;
		ctx->vn = new_chunk;
		if (obj_parse_v3(line + 3, slen - 3, &ctx->vn[ctx->vn_cnt-1]))
		{
			return -1;
		}
		return 0;
	}

	return 0;
}

void obj_parser_init(obj_ctx_t *ctx)
{
	memset(ctx, 0, sizeof(obj_ctx_t));
}


void obj_parser_destroy(obj_ctx_t *ctx)
{
	if (ctx->v) free(ctx->v);
	if (ctx->vn) free(ctx->vn);
	if (ctx->vt) free(ctx->vt);
	if (ctx->faces) free(ctx->faces);
}
