

void Camellia_Decrypt( const Byte *c, const Byte *e, Byte *p )
{
	int i;


		XorBlock( c, e+256, p );

		for( i=2; i>=0; i-- ){
			Camellia_Feistel( p+0, e+216+(i<<4), p+8 );
			Camellia_Feistel( p+8, e+208+(i<<4), p+0 );
		}

		Camellia_FLlayer( p, e+200, e+192 );


	for( i=2; i>=0; i-- ){
		Camellia_Feistel( p+0, e+152+(i<<4), p+8 );
		Camellia_Feistel( p+8, e+144+(i<<4), p+0 );
	}

	Camellia_FLlayer( p, e+136, e+128 );

	for( i=2; i>=0; i-- ){
		Camellia_Feistel( p+0, e+88+(i<<4), p+8 );
		Camellia_Feistel( p+8, e+80+(i<<4), p+0 );
	}

	Camellia_FLlayer( p, e+72, e+64 );

	for( i=2; i>=0; i-- ){
		Camellia_Feistel( p+0, e+24+(i<<4), p+8 );
		Camellia_Feistel( p+8, e+16+(i<<4), p+0 );
	}

	SwapHalf( p );
	XorBlock( p, e+0, p );
}
