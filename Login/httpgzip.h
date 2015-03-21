#include "stdafx.h"

typedef unsigned char Bytef;
typedef unsigned char Byte;
typedef unsigned int uLong;


typedef voidpf(*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void(*free_func)  OF((voidpf opaque, voidpf address));

struct internal_state;

typedef struct z_stream_s {
	z_const Bytef *next_in;     /* 下一个输入字节 */
	uInt     avail_in;  /* next_in中可用的字节数 */
	uLong    total_in;  /* 目前读取的输入总字节数 */

	Bytef    *next_out; /* 下一个输出字节应该放在这 */
	uInt     avail_out; /* next_out中剩下的可用空间 */
	uLong    total_out; /* 目前输出的总字节数 */

	z_const char *msg;  /* 最后的错误消息，如果没错误为NULL */
	struct internal_state FAR *state; /* 对应用程序不可见 */

	alloc_func zalloc;  /* 用来分配内部状态 */
	free_func  zfree;   /* 用来释放内部状态 */
	voidpf     opaque;  /* 传给zalloc和zfree的私有数据对象 */

	int     data_type;  /* 数据类型的最好猜测：二进制数据或文本 */
	uLong   adler;      /* 解压数据的adler32值 */
	uLong   reserved;   /* 保留为将来使用 */
} z_stream;

typedef z_stream FAR *z_streamp;
/* 来自或传给zlib例程的gzip头部信息，对这些字段的含义，参考RFC 1952 */
typedef struct gz_header_s {
	int     text;       /* 为true，如果压缩数据被认为是文本 */
	uLong   time;       /* 修改时间 */
	int     xflags;     /* 额外标志（写gzip文件时不会用到） */
	int     os;         /* 操作系统 */
	Bytef   *extra;     /* 指向额外的字段，如果没有则为Z_NULL */
	uInt    extra_len;  /* 额外字段的长度（如果extrextra != Z_NULL则有底） */
	uInt    extra_max;  /* extra的空间上限（只用在读头部） */
	Bytef   *name;      /* 指向以0终止的文件名，或为Z_NULL */
	uInt    name_max;   /* name的空间上限（只用在读头部） */
	Bytef   *comment;   /* 指向以0终止的注释，或为Z_NULL */
	uInt    comm_max;   /* comment的空间上限（只用在读头部） */
	int     hcrc;       /* 为true，如果会有头部CRC */
	int     done;       /* 为true，当gzip头部读取完时（写gzip文件时不会用到） */
} gz_header;

typedef gz_header FAR *gz_headerp;

/* Compress data */
int zcompress(Bytef *data, uLong ndata,
	Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if (data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit(&c_stream, Z_DEFAULT_COMPRESSION) != Z_OK) return -1;
		c_stream.next_in = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Compress gzip data */
int gzcompress(Bytef *data, uLong ndata,
	Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if (data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			-MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Uncompress data */
int zdecompress(Byte *zdata, uLong nzdata,
	Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream; /* decompression stream */

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if (inflateInit(&d_stream) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK) return -1;
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}

/* HTTP gzip decompress */
int httpgzdecompress(Byte *zdata, uLong nzdata,
	Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] =
	{
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if (inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK)
		{
			if (err == Z_DATA_ERROR)
			{
				d_stream.next_in = (Bytef*)dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
				{
					return -1;
				}
			}
			else return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}

/* Uncompress gzip data */
int gzdecompress(Byte *zdata, uLong nzdata,
	Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] =
	{
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if (inflateInit2(&d_stream, -MAX_WBITS) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;

	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK)
		{
			if (err == Z_DATA_ERROR)
			{
				d_stream.next_in = (Bytef*)dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
				{
					return -1;
				}
			}
			else return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}
