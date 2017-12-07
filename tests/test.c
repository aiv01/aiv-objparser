#include <obj.h>
#include <stdio.h>

static int tests_counter = 0;

#define test(f) tests_counter++;\
fprintf(stdout, "running " #f "...\n");\
if (f())\
{\
	fprintf(stderr, "test " #f " failed !\n");\
	exit(1);\
}

#define STR(x) x, sizeof(x)

int test_v3(obj_v3_t v, float x, float y, float z) 
{
	if (v.x == x && v.y == y && v.z == z)
		return 0;
	return -1;
}

int test_uv(obj_uv_t uv, float u, float v) 
{
	if (uv.u == u && uv.v == v)
		return 0;
	return -1;
}

int test_vertex(obj_vertex_t v, size_t v_idx, size_t vt_idx, float vn_idx) 
{
	if (v.v_idx == v_idx && v.vt_idx == vt_idx && v.vn_idx == vn_idx)
		return 0;
	return -1;
}

int test_obj_parse_v3_green_light()
{
	obj_v3_t v3;
	if (obj_parse_v3("1 2 3", 5, &v3))
		return -1;
	return test_v3(v3, 1, 2, 3);
}

int test_obj_parse_v3_zero()
{
	obj_v3_t v3;
	if (obj_parse_v3("1 2 3", 0, &v3))
		return 0;
	return -1;
}

int test_obj_parse_v3_one()
{
	obj_v3_t v3;
	if (obj_parse_v3("1 2 3", 1, &v3))
		return 0;
	return -1;
}

int test_obj_parse_v3_invalid()
{
	obj_v3_t v3;
	if (obj_parse_v3("foo bar foobar", 14, &v3))
		return 0;
	return -1;
}

int test_obj_parse_v3_float()
{
	obj_v3_t v3;
	if (obj_parse_v3("1.7 2.2 30", 10, &v3))
		return -1;
	return test_v3(v3, 1.7, 2.2, 30);
}

int test_obj_parse_line_v()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	if (obj_parse_line("v 1.7 2.2 30", 12, &ctx))
	{
		obj_parser_destroy(&ctx);
		return -1;
	}
	int ret = test_v3(ctx.v[0], 1.7, 2.2, 30);
	obj_parser_destroy(&ctx);
	return ret;
}

int test_obj_parse_line_v_red()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	int ret = obj_parse_line("v 1.7 2.2", 9, &ctx);
	obj_parser_destroy(&ctx);
	return ret == -1 ? 0 : -1;
}

int test_obj_parse_line_vn()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	if (obj_parse_line("vn 1.7 2.2 30", 13, &ctx))
	{
		obj_parser_destroy(&ctx);
		return -1;
	}
	int ret = test_v3(ctx.vn[0], 1.7, 2.2, 30);
	obj_parser_destroy(&ctx);
	return ret;
}

int test_obj_parse_line_vt()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	if (obj_parse_line("vt 1.7 2.2", 10, &ctx))
	{
		obj_parser_destroy(&ctx);
		return -1;
	}
	int ret = test_uv(ctx.vt[0], 1.7, 2.2);
	obj_parser_destroy(&ctx);
	return ret;
}

int test_obj_parse_line_vn_red()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	int ret = obj_parse_line("vn 1.7 2.2", 9, &ctx);
	obj_parser_destroy(&ctx);
	return ret == -1 ? 0 : -1;
}

int test_obj_parse_line_vt_red()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	int ret = obj_parse_line("vt 1.7", 6, &ctx);
	obj_parser_destroy(&ctx);
	return ret == -1 ? 0 : -1;
}

int test_obj_parse_line_face()
{
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	if (obj_parse_line(STR("f 1/2/3 4/5/6 7/8/9"), &ctx))
	{
		obj_parser_destroy(&ctx);
		return -1;
	}
	int ret = test_vertex(ctx.faces[0].v1, 1, 2, 3);
	obj_parser_destroy(&ctx);
	return ret;
}

int test_obj_suzanne()
{
	FILE *f = NULL;
#ifdef _WIN32
	if (fopen_s(&f, "suzanne.obj", "rb"))
		return -1;
#else
	f = fopen("suzanne.obj", "rb");
#endif
	if (!f)
		return -1;
	char buf[1024];
	obj_ctx_t ctx;
	obj_parser_init(&ctx);
	int ret = 0;
	char *line = NULL;
	while((line = fgets(buf, 1024, f)))
	{			
		if (obj_parse_line(line, strlen(line), &ctx))
		{
			ret = -1;
			break;
		}
	}
	if (!ret)
	{
		if (ctx.faces_cnt != 968)
			ret = -1;
		if (ctx.v_cnt != 511)
			ret = -1;
		if (ctx.vt_cnt != 590)
			ret = -1;
		if (ctx.vn_cnt != 507)
			ret = -1;
	}
	obj_parser_destroy(&ctx);
	fclose(f);
	return ret;
}


int main(int argc, char **argv)
{
	test(test_obj_parse_v3_green_light);
	test(test_obj_parse_v3_float);
	test(test_obj_parse_v3_zero);
	test(test_obj_parse_v3_one);
	test(test_obj_parse_v3_invalid);
	test(test_obj_parse_line_v);
	test(test_obj_parse_line_v_red);
	test(test_obj_parse_line_vn);
	test(test_obj_parse_line_vn_red);
	test(test_obj_parse_line_face);
	test(test_obj_parse_line_vt);
	test(test_obj_parse_line_vt_red);
	test(test_obj_suzanne);
	fprintf(stdout, "%d tests executed\n", tests_counter);
	return 0;
}
