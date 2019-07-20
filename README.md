# struct2x
一个开源的C结构体与 JSON 快速互转库，它可以快速实现**结构体对象**与**JSON对象**之间序列化及反序列化要求。快速、简洁的 API 设计，大大降低直接使用 JSON 解析库来实现此类功能的代码复杂度。

# 起源
把面向对象设计应用到C语言中，是当下很流行的设计思想。由于C语言中没有类，所以一般使用结构体`struct`充当类，那么结构体变量就是对象。有了对象之后，很多时候需要考虑对象的序列化及反序列化问题。C语言不像很多高级语言拥有反射等机制，使得对象序列化及反序列化被原生的支持。

对于C语言来说，序列化为 JSON 字符串是个不错的选择，所以就得使用 cJSON 这类 JSON 解析库，但是使用后的代码冗余且逻辑性差，所以萌生对cJSON库进行二次封装，实现一个 struct 与 JSON 之间快速互转的库。 struct2json 就诞生于此。下面是 struct2json 主要使用场景：

**持久化** ：结构体对象序列化为 JSON 对象后，可直接保存至文件、Flash，实现对结构体对象的掉电存储。

**通信** ：高级语言对JSON支持的很友好，例如： Javascript、Groovy 就对 JSON 具有原生的支持，所以 JSON 也可作为C语言与其他语言软件之间的通信协议格式及对象传递格式。

**可视化** ：序列化为 JSON 后的对象，可以更加直观的展示到控制台或者 UI 上，可用于产品调试、产品二次开发等场景。

# 如何使用
包含srtuctMacro.h文件。

结构体如下：
```
struct struItem {
    int id;
    std::string str;
    std::vector<std::string> v;
    std::vector<struInfo> v2;
    std::map<std::string, int> m;
    std::map<std::string, struInfo> m2;

    template<typename T>
    void serialize(T& t)//使用该模板函数，指定struct中需要转为json的变量，必须为public
    {
        SERIALIZATION(t, id, str, v, v2, m, m2);
    }
};
```
也可在struct外部使用非侵入式C++结构体序列化：
```
template<typename T>
void serialize(T& t, struItem& item)
{
    NISERIALIZATION(t, item, id, str, v, v2, m, m2);
}
```
生成json时会***优先使用非侵入式***。

示例见 tester/main.cpp。
