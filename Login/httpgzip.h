#include "stdafx.h"

typedef unsigned char Bytef;
typedef unsigned char Byte;
typedef unsigned int uLong;


typedef voidpf(*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void(*free_func)  OF((voidpf opaque, voidpf address));

struct internal_state;

typedef struct z_stream_s {
	z_const Bytef *next_in;     /* ��һ�������ֽ� */
	uInt     avail_in;  /* next_in�п��õ��ֽ��� */
	uLong    total_in;  /* Ŀǰ��ȡ���������ֽ��� */

	Bytef    *next_out; /* ��һ������ֽ�Ӧ�÷����� */
	uInt     avail_out; /* next_out��ʣ�µĿ��ÿռ� */
	uLong    total_out; /* Ŀǰ��������ֽ��� */

	z_const char *msg;  /* ���Ĵ�����Ϣ�����û����ΪNULL */
	struct internal_state FAR *state; /* ��Ӧ�ó��򲻿ɼ� */

	alloc_func zalloc;  /* ���������ڲ�״̬ */
	free_func  zfree;   /* �����ͷ��ڲ�״̬ */
	voidpf     opaque;  /* ����zalloc��zfree��˽�����ݶ��� */

	int     data_type;  /* �������͵���ò²⣺���������ݻ��ı� */
	uLong   adler;      /* ��ѹ���ݵ�adler32ֵ */
	uLong   reserved;   /* ����Ϊ����ʹ�� */
} z_stream;

typedef z_stream FAR *z_streamp;
/* ���Ի򴫸�zlib���̵�gzipͷ����Ϣ������Щ�ֶεĺ��壬�ο�RFC 1952 */
typedef struct gz_header_s {
	int     text;       /* Ϊtrue�����ѹ�����ݱ���Ϊ���ı� */
	uLong   time;       /* �޸�ʱ�� */
	int     xflags;     /* �����־��дgzip�ļ�ʱ�����õ��� */
	int     os;         /* ����ϵͳ */
	Bytef   *extra;     /* ָ�������ֶΣ����û����ΪZ_NULL */
	uInt    extra_len;  /* �����ֶεĳ��ȣ����extrextra != Z_NULL���еף� */
	uInt    extra_max;  /* extra�Ŀռ����ޣ�ֻ���ڶ�ͷ���� */
	Bytef   *name;      /* ָ����0��ֹ���ļ�������ΪZ_NULL */
	uInt    name_max;   /* name�Ŀռ����ޣ�ֻ���ڶ�ͷ���� */
	Bytef   *comment;   /* ָ����0��ֹ��ע�ͣ���ΪZ_NULL */
	uInt    comm_max;   /* comment�Ŀռ����ޣ�ֻ���ڶ�ͷ���� */
	int     hcrc;       /* Ϊtrue���������ͷ��CRC */
	int     done;       /* Ϊtrue����gzipͷ����ȡ��ʱ��дgzip�ļ�ʱ�����õ��� */
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
