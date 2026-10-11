#ifndef MD5_53_H
#define MD5_53_H
#include <stdint.h>
#include <string.h>
/* Incremental RFC 1321 MD5. Used for byte-identity testing, not authentication. */
struct md5_53 {uint32_t h[4];uint64_t bytes;uint8_t buf[64];unsigned used;};
static uint32_t rol53(uint32_t x,unsigned n){return (x<<n)|(x>>(32-n));}
static void md5_block53(struct md5_53 *m,const uint8_t *p){
    static const uint32_t k[64]={
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};
    static const unsigned s[16]={7,12,17,22,5,9,14,20,4,11,16,23,6,10,15,21};
    uint32_t w[16];for(unsigned i=0;i<16;++i)w[i]=(uint32_t)p[4*i]|((uint32_t)p[4*i+1]<<8)|((uint32_t)p[4*i+2]<<16)|((uint32_t)p[4*i+3]<<24);
    uint32_t a=m->h[0],b=m->h[1],c=m->h[2],d=m->h[3];
    for(unsigned i=0;i<64;++i){
        uint32_t f;unsigned g;
        if(i<16){f=(b&c)|(~b&d);g=i;}
        else if(i<32){f=(d&b)|(~d&c);g=(5*i+1)&15;}
        else if(i<48){f=b^c^d;g=(3*i+5)&15;}
        else{f=c^(b|~d);g=(7*i)&15;}
        uint32_t old=d;d=c;c=b;b+=rol53(a+f+k[i]+w[g],s[(i/16)*4+(i&3)]);a=old;
    }
    m->h[0]+=a;m->h[1]+=b;m->h[2]+=c;m->h[3]+=d;
}
static void md5_init53(struct md5_53 *m){*m=(struct md5_53){{0x67452301,0xefcdab89,0x98badcfe,0x10325476},0,{0},0};}
static void md5_add53(struct md5_53 *m,const void *src,unsigned n){
    const uint8_t *p=src;m->bytes+=n;
    while(n){unsigned k=64-m->used;if(k>n)k=n;memcpy(m->buf+m->used,p,k);m->used+=k;p+=k;n-=k;
        if(m->used==64){md5_block53(m,m->buf);m->used=0;}}
}
static void md5_end53(struct md5_53 *m,uint8_t out[16]){
    uint64_t bits=m->bytes*8;uint8_t tail[72]={0x80};unsigned n=m->used<56?56-m->used:120-m->used;
    for(unsigned i=0;i<8;++i)tail[n+i]=(uint8_t)(bits>>(8*i));
    md5_add53(m,tail,n+8);for(unsigned i=0;i<16;++i)out[i]=(uint8_t)(m->h[i/4]>>(8*(i&3)));
}
static void md5_hex53(const uint8_t d[16],char out[33]){
    static const char h[]="0123456789ABCDEF";for(unsigned i=0;i<16;++i){out[2*i]=h[d[i]>>4];out[2*i+1]=h[d[i]&15];}out[32]=0;
}
#endif
