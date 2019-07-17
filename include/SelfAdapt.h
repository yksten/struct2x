#ifndef __STRUCT_SELFADAPT_H__
#define __STRUCT_SELFADAPT_H__


struct access
{
    template<class T, class C>
    static void serialize(T& t, C& c)
    {
        c.serialize(t);
    }
};


template<class T, class C>
inline void serialize(T& t, C& c)
{
    access::serialize(t, c);
}


template<class T, class C>
inline void serializeWrapper(T& t, C& c)
{
    serialize(t, c);
}


#endif