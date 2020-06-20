#pragma once

/*
Resort2OG utilizes EKG's C++ Endian library
https://github.com/ekg/endian
*/

// from http://stackoverflow.com/a/8979034/238609
inline int IsBigEndian()
{
    union
    {
        unsigned int i;
        char c[sizeof(unsigned int)];
    } u;
    u.i = 1;
    return !u.c[0];
}

/* or (pedantic non-UB version) */
inline int IsBigEndian()
{
    int i = 1;
    return !*((char*)&i);
}

// from http://stackoverflow.com/a/4956493/238609
template <typename T>
T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

template <typename T>
T to_big_endian(T u)
{
    if (IsBigEndian()) {
        return u;
    }
    else {
        return swap_endian<T>(u);
    }
}

template <typename T>
T to_little_endian(T u)
{
    if (!IsBigEndian()) {
        return u;
    }
    else {
        return swap_endian<T>(u);
    }
}