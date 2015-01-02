#ifndef EASTWOOD_BLOWFISH_H
#define	EASTWOOD_BLOWFISH_H

#include "eastwood/StdDef.h"

typedef uint32_t t_bf_p[18];
typedef uint32_t t_bf_s[4][256];
namespace eastwood {
    
class Cblowfish
{
public:
    void set_key(const uint8_t* key, int cb_key);
    void encipher(uint32_t& xl, uint32_t& xr) const;
    void encipher(const void* s, void* d, int size) const;
    void decipher(uint32_t& xl, uint32_t& xr) const;
    void decipher(const void* s, void* d, int size) const;
private:
    uint32_t S(uint32_t x, int i) const;
    uint32_t bf_f(uint32_t x) const;
    void ROUND(uint32_t& a, uint32_t b, int n) const;

    t_bf_p m_p;
    t_bf_s m_s;
};

}
#endif	/* CBLOWFISH_H */
