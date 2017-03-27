/*

SEAL 3.0 pure C implementation

(c) 2010 Alexander Myasnikov

e-mail: alexanderwdark@ya.ru

fully rewritten from C++ code

Authors of algorithm are Don Coppersmith, Phillip Rogaway

 */

#define PRNG_TEST

#include <mem.h>;

void SEALIncrementCounter(void);

typedef unsigned long int u32;
typedef unsigned char u8;

#define WORD_SIZE 4
#define SHA_DATASIZE 64

void XorWords(u32 *a, u32 *b, u32 n)
{
  u32 i;

  for (i = 0; i < n; i++)
    a[i] = a[i] ^ b[i];
}

void xorbuf(u8 *input, u8 *mask, u32 count)
{

  u32 i;

  if (count % 4 == 0)
    XorWords((u32*)input, (u32*)mask, count / 4);
  else
    {
      for (i = 0; i < count; i++)
	input[i] = input[i] ^ mask[i];
    }
}

#define expand(W,i) ( W[ i & 15 ] = rotlFixed( (W[i&15] ^ W[i-14&15] ^ W[i-8&15] ^ W[i-3&15]), 1U) )

u32 rotlFixed(u32 x, u32 y)
{
  return(x << y) | (x >> (32 - y));
}

#define subRound(a, b, c, d, e, f, k, data) \
(e += rotlFixed(a, 5U) + f(b, c, d) + k + data, b = rotlFixed(b, 30U))

#define K1  0x5A827999L 				/* Rounds  0-19 */
#define K2  0x6ED9EBA1L 				/* Rounds 20-39 */
#define K3  0x8F1BBCDCL 				/* Rounds 40-59 */
#define K4  0xCA62C1D6L 				/* Rounds 60-79 */

#define f1(x,y,z)   ( z ^ ( x & ( y ^ z ) ) )		/* Rounds  0-19 */
#define f2(x,y,z)   ( x ^ y ^ z )			/* Rounds 20-39 */
#define f3(x,y,z)   ( ( x & y ) | ( z & ( x | y ) ) )	/* Rounds 40-59 */
#define f4(x,y,z)   ( x ^ y ^ z )			/* Rounds 60-79 */

void SHATransform(u32 *digest, const u32 *data)
{
  register u32 A, B, C, D, E;

  u32 eData[16];
  memcpy(eData, data, SHA_DATASIZE);

  A = digest[0];
  B = digest[1];
  C = digest[2];
  D = digest[3];
  E = digest[4];

  // Heavy mangling, in 4 sub-rounds of 20 interations each.
  subRound(A, B, C, D, E, f1, K1, eData[0]);
  subRound(E, A, B, C, D, f1, K1, eData[1]);
  subRound(D, E, A, B, C, f1, K1, eData[2]);
  subRound(C, D, E, A, B, f1, K1, eData[3]);
  subRound(B, C, D, E, A, f1, K1, eData[4]);
  subRound(A, B, C, D, E, f1, K1, eData[5]);
  subRound(E, A, B, C, D, f1, K1, eData[6]);
  subRound(D, E, A, B, C, f1, K1, eData[7]);
  subRound(C, D, E, A, B, f1, K1, eData[8]);
  subRound(B, C, D, E, A, f1, K1, eData[9]);
  subRound(A, B, C, D, E, f1, K1, eData[10]);
  subRound(E, A, B, C, D, f1, K1, eData[11]);
  subRound(D, E, A, B, C, f1, K1, eData[12]);
  subRound(C, D, E, A, B, f1, K1, eData[13]);
  subRound(B, C, D, E, A, f1, K1, eData[14]);
  subRound(A, B, C, D, E, f1, K1, eData[15]);
  subRound(E, A, B, C, D, f1, K1, expand(eData, 16));
  subRound(D, E, A, B, C, f1, K1, expand(eData, 17));
  subRound(C, D, E, A, B, f1, K1, expand(eData, 18));
  subRound(B, C, D, E, A, f1, K1, expand(eData, 19));

  subRound(A, B, C, D, E, f2, K2, expand(eData, 20));
  subRound(E, A, B, C, D, f2, K2, expand(eData, 21));
  subRound(D, E, A, B, C, f2, K2, expand(eData, 22));
  subRound(C, D, E, A, B, f2, K2, expand(eData, 23));
  subRound(B, C, D, E, A, f2, K2, expand(eData, 24));
  subRound(A, B, C, D, E, f2, K2, expand(eData, 25));
  subRound(E, A, B, C, D, f2, K2, expand(eData, 26));
  subRound(D, E, A, B, C, f2, K2, expand(eData, 27));
  subRound(C, D, E, A, B, f2, K2, expand(eData, 28));
  subRound(B, C, D, E, A, f2, K2, expand(eData, 29));
  subRound(A, B, C, D, E, f2, K2, expand(eData, 30));
  subRound(E, A, B, C, D, f2, K2, expand(eData, 31));
  subRound(D, E, A, B, C, f2, K2, expand(eData, 32));
  subRound(C, D, E, A, B, f2, K2, expand(eData, 33));
  subRound(B, C, D, E, A, f2, K2, expand(eData, 34));
  subRound(A, B, C, D, E, f2, K2, expand(eData, 35));
  subRound(E, A, B, C, D, f2, K2, expand(eData, 36));
  subRound(D, E, A, B, C, f2, K2, expand(eData, 37));
  subRound(C, D, E, A, B, f2, K2, expand(eData, 38));
  subRound(B, C, D, E, A, f2, K2, expand(eData, 39));

  subRound(A, B, C, D, E, f3, K3, expand(eData, 40));
  subRound(E, A, B, C, D, f3, K3, expand(eData, 41));
  subRound(D, E, A, B, C, f3, K3, expand(eData, 42));
  subRound(C, D, E, A, B, f3, K3, expand(eData, 43));
  subRound(B, C, D, E, A, f3, K3, expand(eData, 44));
  subRound(A, B, C, D, E, f3, K3, expand(eData, 45));
  subRound(E, A, B, C, D, f3, K3, expand(eData, 46));
  subRound(D, E, A, B, C, f3, K3, expand(eData, 47));
  subRound(C, D, E, A, B, f3, K3, expand(eData, 48));
  subRound(B, C, D, E, A, f3, K3, expand(eData, 49));
  subRound(A, B, C, D, E, f3, K3, expand(eData, 50));
  subRound(E, A, B, C, D, f3, K3, expand(eData, 51));
  subRound(D, E, A, B, C, f3, K3, expand(eData, 52));
  subRound(C, D, E, A, B, f3, K3, expand(eData, 53));
  subRound(B, C, D, E, A, f3, K3, expand(eData, 54));
  subRound(A, B, C, D, E, f3, K3, expand(eData, 55));
  subRound(E, A, B, C, D, f3, K3, expand(eData, 56));
  subRound(D, E, A, B, C, f3, K3, expand(eData, 57));
  subRound(C, D, E, A, B, f3, K3, expand(eData, 58));
  subRound(B, C, D, E, A, f3, K3, expand(eData, 59));

  subRound(A, B, C, D, E, f4, K4, expand(eData, 60));
  subRound(E, A, B, C, D, f4, K4, expand(eData, 61));
  subRound(D, E, A, B, C, f4, K4, expand(eData, 62));
  subRound(C, D, E, A, B, f4, K4, expand(eData, 63));
  subRound(B, C, D, E, A, f4, K4, expand(eData, 64));
  subRound(A, B, C, D, E, f4, K4, expand(eData, 65));
  subRound(E, A, B, C, D, f4, K4, expand(eData, 66));
  subRound(D, E, A, B, C, f4, K4, expand(eData, 67));
  subRound(C, D, E, A, B, f4, K4, expand(eData, 68));
  subRound(B, C, D, E, A, f4, K4, expand(eData, 69));
  subRound(A, B, C, D, E, f4, K4, expand(eData, 70));
  subRound(E, A, B, C, D, f4, K4, expand(eData, 71));
  subRound(D, E, A, B, C, f4, K4, expand(eData, 72));
  subRound(C, D, E, A, B, f4, K4, expand(eData, 73));
  subRound(B, C, D, E, A, f4, K4, expand(eData, 74));
  subRound(A, B, C, D, E, f4, K4, expand(eData, 75));
  subRound(E, A, B, C, D, f4, K4, expand(eData, 76));
  subRound(D, E, A, B, C, f4, K4, expand(eData, 77));
  subRound(C, D, E, A, B, f4, K4, expand(eData, 78));
  subRound(B, C, D, E, A, f4, K4, expand(eData, 79));

  digest[0] += A;
  digest[1] += B;
  digest[2] += C;
  digest[3] += D;
  digest[4] += E;

  memset(eData, 0, SHA_DATASIZE);
}

#define L 32768

u32 startCount;
u32 counter;
u32 position;
u32 lastIndex;
u32 H[5], Z[5], D[16];
u32 S[256], T[512];
u32 R[4 * L / 8192];
u8 buffer[4096];

u32 ByteReverse(u32 value)
{
  value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
  return rotlFixed(value, 16U);

}

void Gamma(const u32 *key)
{
  u32 i;

  lastIndex = 0xffffffff;

  for (i = 0; i != 5; i++)
    H[i] = ByteReverse(key[i]);

  memset(D, 0, 64);
}

u32 GammaApply(u32 i)
{
  u32 shaIndex = i / 5;
  if (shaIndex != lastIndex)
    {
      memcpy(Z, H, 20);
      D[0] = shaIndex;
      SHATransform(Z, D);
      lastIndex = shaIndex;
    }
  return Z[i % 5];
}

u32 rotrFixed(u32 x, u32 y)
{
  return(x >> y) | (x << (32 - y));
}

#define Ttab(x) *(u32 *)(Tptr + x)

void SEALGenerate(u32 in, u8 *out)
{
  u32 a, b, c, d, n1, n2, n3, n4;
  u32 p, q;
  u32 l, j, i;
  u32 *wout = (u32*)out;
  u8 *Tptr;

  for (l = 0; l < L / 8192; l++)
    {
      a = in ^ R[4 * l];
      b = rotrFixed(in, 8U) ^ R[4 * l + 1];
      c = rotrFixed(in, 16U) ^ R[4 * l + 2];
      d = rotrFixed(in, 24U) ^ R[4 * l + 3];

      Tptr = (u8*) & T[0];

      for (j = 0; j < 2; j++)
	{
	  p = a & 0x7fc;
	  b += Ttab(p);
	  a = rotrFixed(a, 9U);

	  p = b & 0x7fc;
	  c += Ttab(p);
	  b = rotrFixed(b, 9U);

	  p = c & 0x7fc;
	  d += Ttab(p);
	  c = rotrFixed(c, 9U);

	  p = d & 0x7fc;
	  a += Ttab(p);
	  d = rotrFixed(d, 9U);
	}

      n1 = d;
      n2 = b;
      n3 = a;
      n4 = c;

      p = a & 0x7fc;
      b += Ttab(p);
      a = rotrFixed(a, 9U);

      p = b & 0x7fc;
      c += Ttab(p);
      b = rotrFixed(b, 9U);

      p = c & 0x7fc;
      d += Ttab(p);
      c = rotrFixed(c, 9U);

      p = d & 0x7fc;
      a += Ttab(p);
      d = rotrFixed(d, 9U);

      // generate 8192 bits
      for (i = 0; i < 64; i++)
	{
	  p = a & 0x7fc;
	  a = rotrFixed(a, 9U);
	  b += Ttab(p);
	  b ^= a;

	  q = b & 0x7fc;
	  b = rotrFixed(b, 9U);
	  c ^= Ttab(q);
	  c += b;

	  p = (p + c) & 0x7fc;
	  c = rotrFixed(c, 9U);
	  d += Ttab(p);
	  d ^= c;

	  q = (q + d) & 0x7fc;
	  d = rotrFixed(d, 9U);
	  a ^= Ttab(q);
	  a += d;

	  p = (p + a) & 0x7fc;
	  b ^= Ttab(p);
	  a = rotrFixed(a, 9U);

	  q = (q + b) & 0x7fc;
	  c += Ttab(q);
	  b = rotrFixed(b, 9U);

	  p = (p + c) & 0x7fc;
	  d ^= Ttab(p);
	  c = rotrFixed(c, 9U);

	  q = (q + d) & 0x7fc;
	  d = rotrFixed(d, 9U);
	  a += Ttab(q);

	  wout[0] = ByteReverse(b + S[4 * i + 0]);
	  wout[1] = ByteReverse(c ^ S[4 * i + 1]);
	  wout[2] = ByteReverse(d + S[4 * i + 2]);
	  wout[3] = ByteReverse(a ^ S[4 * i + 3]);

	  wout += 4;

	  if (i & 1)
	    {
	      a += n3;
	      b += n4;
	      c ^= n3;
	      d ^= n4;
	    }
	  else
	    {
	      a += n1;
	      b += n2;
	      c ^= n1;
	      d ^= n2;
	    }
	}
    }

  a = b = c = d = n1 = n2 = n3 = n4 = 0;
  p = q = 0;
}

void SEALIncrementCounter()
{
  counter++;
  position = 0;
  SEALGenerate(counter, buffer);
}

u8 SEALGetByte()
{
  if (position == L / 8)
    SEALIncrementCounter();
  return buffer[position++];
}

void __stdcall __export crypt(u8 *inString, u32 length)
{
  while (length >= L / 8 - position)
    {
      xorbuf(inString, buffer + position, L / 8 - position);
      length -= L / 8 - position;
      inString += L / 8 - position;
      SEALIncrementCounter();
    }

  xorbuf(inString, buffer + position, length);
  position += length;
}

void __stdcall __export prng(u8 *OutString, u32 length)
{
  while (length >= L / 8 - position)
    {
      memcpy(OutString, buffer + position, L / 8 - position);
      length -= L / 8 - position;
      OutString += L / 8 - position;
      SEALIncrementCounter();
    }

  memcpy(OutString, buffer + position, length);
  position += length;
}

void __stdcall __export decrypt(u8 *inString, u32 length)
{
  crypt(inString, length);
}

void __stdcall __export setup(const u32 *key)
{
  u32 i;
  counter = 0;

  startCount = counter;
  counter = counter;
  position = 0;

  Gamma(key);

  for (i = 0; i < 512; i++)
    T[i] = GammaApply(i);

  for (i = 0; i < 256; i++)
    S[i] = GammaApply(0x1000 + i);

  for (i = 0; i < 4 * (L / 8192); i++)
    R[i] = GammaApply(0x2000 + i);

  SEALGenerate(counter, buffer);
}

void SEALSeek(u32 seekPosition)
{
  counter = startCount + seekPosition / (L / 8);
  position = seekPosition % (L / 8);
  SEALGenerate(counter, buffer);
}

u32 NextCount()
{
  return counter + 1;
}

#ifdef PRNG_TEST

#include <stdio.h>

u32 key[20] =
{
  0x10000000, 0x02000000, 0x00300000, 0x00040000, 0x00005000, 0x00000600,
  0x00000070, 0x00000008, 0x90000000, 0x0a000000, 0x00b00000, 0x000c0000,
  0x0000d000, 0x00000e00, 0x000000f0, 0x10000000, 0x01100000, 0x00120000,
  0x00013000, 0x00001400
};

u8 buff[4096];

int main(int argc, char *argv[])
{
  FILE *f;
  int i, n;

  if (argc < 2)
    {
      fprintf(stderr, "usage: %s <output filename>\n", argv[0]);
      return(1);
    }
  if ((f = fopen(argv[1], "wb+")) == NULL)
    {
      fprintf(stderr, "failed to open '%s' for writing.\n", argv[1]);
      return(1);
    }

  setup(key);
  for (i = 0; i != 1073741824 / 4096; i++)
    {

      prng(&buff[0], 4096);

      fwrite(buff, 4096, 1, f);

    }

  fclose(f);
  return(0);
}

#endif
