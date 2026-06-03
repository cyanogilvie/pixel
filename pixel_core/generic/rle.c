#include <pixelInt.h>


gimp_image_t *rle_decode(const uint8_t* rle_data, unsigned int data_len, int* status) //{{{
{
	gimp_image_t*		res = NULL;
	const rle_header*	header;
	const _pel*			map;
	const uint8_t*		p;
	_pel*				d;
	const _pel*			s;
	int					i, j;
	unsigned int		tmp;
	unsigned char		chunk_type, repcount;
	unsigned int		key;
	_pel				value;
	int					saw_eof = 0;

	// Sanity checks {{{
	if (data_len < sizeof(rle_header)) {
		*status = RLE_STATUS_TOOSHORT;
		return NULL;
	}

	header = (rle_header*)rle_data;

	if (memcmp("pixelrle", header->tag, 8) != 0) {
		*status = RLE_STATUS_BAD_TAG;
		return NULL;
	}

	if (
			header->line_end_check[0] != 0xA
			|| header->line_end_check[1] != 0xD
			|| header->line_end_check[2] != 'a'
	   ) {
		*status = RLE_STATUS_LINECHECK_FAILED;
		return NULL;
	}

	if (header->unicode_check[0] != 0xF0 || header->unicode_check[1] != 0x0) {
		*status = RLE_STATUS_UNICODE_FAILED;
		return NULL;
	}

	if (header->version != RLE_VERSION) {
		*status = RLE_STATUS_VERSION_FAILED;
		return NULL;
	}

	tmp = sizeof(rle_header) + header->map_table_length * 4 + header->chunks_size;
	if (tmp > data_len) {
		*status = RLE_STATUS_DATA_TRUNCATED;
		return NULL;
	}

	if (tmp < data_len) {
		*status = RLE_STATUS_ENOUGH_ALREADY;
	} else {
		*status = RLE_STATUS_OK;
	}
	// Sanity checks }}}

	/*
	fprintf(stderr, "width: %d\nheight: %d\nmap_size: %d\nkey_size: %d\nrle_header: %d\n",
			header->width,
			header->height,
			header->map_table_length,
			header->map_table_keysize,
			sizeof(rle_header));
	*/
	map = (const _pel *)(rle_data + sizeof(rle_header));
	p = rle_data + sizeof(rle_header) + header->map_table_length * sizeof(_pel);

//	for (i=0; i<header->map_table_length; i++)
//		fprintf(stderr, "decode map[%d]: 0x%x\n", i, map[i].c);

	value.c = 0x0;
	res = pmap_new(header->width, header->height, value);
	defer { if (res) pmap_free(&res); }
	d = res->pixel_data;

	//fprintf(stderr, "processing %d chunks, saw_eof: %d\n", header->num_chunks, saw_eof);
	for (i = header->num_chunks; i>0 && saw_eof==0; i--) {
		//fprintf(stderr, "processing chunk: %d\t%p\t", header->num_chunks - i, p);
		chunk_type = *p; p++;
		switch (chunk_type) {
			case 0:
				// Map run
				repcount = *p; p++;
				switch (header->map_table_keysize) {
					case 1: key = *(uint8_t*)p; p += 1; break;
					case 2: key = *(uint16_t*)p; p += 2; break;
					default:
							*status = RLE_STATUS_INVALID_KEYSIZE;
							return NULL;
							break;
				}
				if (key > header->map_table_length) {
					*status = RLE_STATUS_KEY_OUT_OF_RANGE;
					return NULL;
				}
				value = map[key];
				//fprintf(stderr, "map run %d(0x%x) * %d\n", key, value.c, repcount);
				//value.c = 0xffff0000;
				//if (repcount == 255)
				//	value.c = 0xffffffff;
				//tmp = 1;
				break;

			case 1:
				// Value run
				repcount = *p++;
				value = *(_pel*)p; p += sizeof(_pel);
				//fprintf(stderr, "value run 0x%x * %d\n", value.c, repcount);
				//value.c = 0xff00ff00;
				break;

			case 2:
				// Verbatim run
				repcount = *p++;
				s = (_pel*)p;
				//fprintf(stderr, "verbatim run %d\n", repcount);
				for (j=0; j<repcount; j++) {
					d->c = s->c;
					/*
					if (j == 0) {
						d->c = 0xffffff00;
					} else if (j == repcount-1) {
						d->c = 0xffff00ff;
					} else {
						d->c = 0xff0000ff;
					}
					*/
					d++;
					s++;
				}
				p = (const uint8_t *)s;
				repcount = 0;
				break;

			case 3:
				// EOF
				//fprintf(stderr, "EOF\n");
				saw_eof = 1;
				repcount = 0;
				break;

			default:
				// Unknown chunk
				//fprintf(stderr, "unknown %d\n", chunk_type);
				*status = RLE_STATUS_BAD_CHUNK;
				return NULL;
				break;
		}

		if (repcount > 0) {
			for (j=0; j<repcount; j++) {
				d->c = value.c;
				//if (tmp == 1) {
				//	d->c = 0xff00ffff;
				//	tmp = 0;
				//}
				d++;
			}
		}
	}

	gimp_image_t*	out = res;
	res = NULL;		// transfer ownership to caller; suppress deferred free
	return out;
}

//}}}
uint8_t* rle_encode(const gimp_image_t* pmap, unsigned int* data_len, int* /*status*/) //{{{
{
	uint8_t*		res;
	rle_header*		header;
	uint32_t		test_map[4096] = {0};
	int				test_map_count[4096] = {0};
	int				max_map = 0;
	int				map_size;
	uint32_t		value = 0;
	int				slot;
	_pel*			map;
	const _pel*		s;
	const _pel*		r;
	_pel*			d;
	uint32_t		pelcount, remaining;
	int				i, j, runlength, keysize;
	size_t			guess_len, used;
	int				chunks_count;
	unsigned char	*p;
	int				stats[5] = {0};
	
	pelcount = pmap->width * pmap->height;
	guess_len = sizeof(rle_header) + 256 + (pelcount * 4) / 3;

	res = (uint8_t*)malloc(guess_len);
	header = (rle_header*)res;
	*header = (rle_header){
		.line_end_check		= {0xA, 0xD, 'a'},
		.unicode_check		= {0xF0, 0x0},
		.version			= RLE_VERSION,
		.width				= pmap->width,
		.height				= pmap->height,
	};
	memcpy(header->tag, "pixelrle", 8);
	used = sizeof(rle_header);

	s = pmap->pixel_data;
	
	for (i = pelcount; i>0; i--, s++) {
		if ((uint32_t)i != pelcount && value == s->c)
			continue;

		value = s->c;
		slot = -1;
		for (j=0; j<max_map; j++) {
			if (test_map[j] == value) {
				slot = j;
				break;
			}
		}
		if (slot == -1 && j < 4096) {
			slot = j;
			test_map[slot] = value;
			max_map++;
		}

		if (slot >= 0)
			test_map_count[slot]++;
	}

	s = pmap->pixel_data;

	j = 0;
	for (i=0; i<max_map; i++) {
		if (test_map_count[i] >= 6) j++;
	}

	if (guess_len < used + sizeof(_pel) * j + (pelcount * 4) / 3) {
		guess_len = used + sizeof(_pel) * j + (pelcount * 4) / 3;
		res = (uint8_t*)realloc(res, guess_len);
		header = (rle_header*)res;
	}

	map_size = j;
	header->map_table_length = map_size;
	header->map_table_keysize = keysize = (j <= 256) ? 1 : 2;

	map = (_pel*)(res + sizeof(rle_header));
	slot = 0;
	for (i=0; i<max_map; i++) {
		if (test_map_count[i] >= 6) {
			map[slot].c = test_map[i];
			slot++;
		}
	}

	used += map_size * sizeof(_pel);
	
	p = (uint8_t*)(res + used);

	remaining = pelcount;

	chunks_count = 0;
	while (remaining > 0) {
		runlength = 1;
		r = s;
		value = r->c;
		r++;
		while (runlength < 255 && remaining - runlength > 0 && r->c == value) {
			runlength++;
			r++;
		}
		slot = -1;
		for (j=0; j<=map_size; j++) {
			if (map[j].c == value) {
				slot = j;
				break;
			}
		}

		// Ensure there is enough allocated at least for this chunk and the
		// EOF marker (1 byte)
		if (guess_len < used + 3 + 256*sizeof(_pel)) {
			const size_t p_offset = p - res;
			guess_len += 65535;
			//fprintf(stderr, "reallocing to %zu bytes\n", guess_len);
			res = (uint8_t*)realloc(res, guess_len);
			p = res + p_offset;
			map = (_pel*)(res + sizeof(rle_header));
			header = (rle_header*)res;
		}
		
		if (slot >= 0) {
			/*
			if (chunks_count < 20) {
				fprintf(stderr, "processing chunk: %p %p %d\t%d\t", res, p, chunks_count, p - res);
				fprintf(stderr, "map run %d(0x%x) * %d\n",
						slot, map[slot].c, runlength);
			}
			*/
			stats[0]++;
			*p = 0; p++;			// Map run
			*p = (uint8_t)runlength; p++;
			if (runlength == 255)
				stats[4]++;
			used += 2;
			if (keysize == 1) {
				*p = slot; p++;
				used++;
			} else {
				*(uint16_t*)p = slot; p += 2;
				used += 2;
			}
		} else {
			if (runlength > 1) {
				stats[1]++;
				*p = 1; p++;		// Value run
				*p = (uint8_t)runlength; p++;
				*(uint32_t*)p = value; p += sizeof(_pel);
				used += 6;
				//fprintf(stderr, "processing chunk: %d\t%d\t", chunks_count, p - res);
				//fprintf(stderr, "value run 0x%x %d\n", value, runlength);
			} else {
				stats[2]++;
				*p = 2; p++;		// Verbatim run
				r = s + 1;
				d = (_pel *)(p + 1);
				while (runlength < 255 && remaining - runlength > 0 && r->c != value) {
					d->c = value; d++;
					value = r->c;
					runlength++;
					r++;
				}
				d->c = value; d++;
				//fprintf(stderr, "processing chunk: %d\t%d\t", chunks_count, p - res);
				//fprintf(stderr, "verbatim run %d\n", runlength);
				*p = (uint8_t)runlength; p++;
				p += runlength * sizeof(_pel);

				used += 2 + runlength * sizeof(_pel);
			}
		}
		chunks_count++;
		s += runlength;
		remaining -= runlength;
	}

	stats[3]++;
	*p = 3; p++;		// EOF marker
	used++;
	chunks_count++;

	guess_len = used;
	res = realloc(res, guess_len);
	header = (rle_header*)res;

	*data_len = used;

	header->chunks_size = used - sizeof(rle_header) - map_size * sizeof(_pel);
	header->num_chunks = chunks_count;

	/*
	fprintf(stderr, "width: %d\nheight: %d\nmap_size: %d\n",
			header->width,
			header->height,
			header->map_table_length);
	fprintf(stderr, "chunks:\n\tmap run: %d\n\tmax length map runs: %d\n\tvalue run: %d\n\tverbatim run: %d\n\teof: %d\nchunks_count: %d\n",
			stats[0],
			stats[4],
			stats[1],
			stats[2],
			stats[3],
			chunks_count);
	*/

//	for (i=0; i<map_size; i++)
//		fprintf(stderr, "map[%d]: 0x%x\n", i, map[i].c);

	return res;
}

//}}}
