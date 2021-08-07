#include <struct2x/json/GenericReader.h>
#include <assert.h>


#define ALIGN(x) (((x) + static_cast<size_t>(7u)) & ~static_cast<size_t>(7u))

#ifndef UNLIKELY
#if defined(__GNUC__) || defined(__clang__)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define UNLIKELY(x) (x)
#endif
#endif

namespace custom {

    class MemoryPoolAllocator {
        struct ChunkHeader {
            size_t capacity;
            size_t size;
            ChunkHeader *next;
        };

        struct SharedData {
            ChunkHeader* chunkHead;
            size_t refcount;
            bool ownBuffer;
        };

        static const size_t SIZEOF_SHARED_DATA  = ALIGN(sizeof(SharedData));
        static const size_t SIZEOF_CHUNK_HEADER = ALIGN(sizeof(ChunkHeader));
        static const size_t kDefaultChunkCapacity = 56 * 32;

        size_t chunk_capacity_;
        SharedData *shared_;
      public:
        explicit MemoryPoolAllocator(size_t chunkSize = kDefaultChunkCapacity)
            : chunk_capacity_(chunkSize), shared_(static_cast<SharedData*>(AllocatorMalloc(SIZEOF_SHARED_DATA + SIZEOF_CHUNK_HEADER))) {
            assert(shared_ != 0);
            shared_->chunkHead           = GetChunkHead(shared_);
            shared_->chunkHead->capacity = 0;
            shared_->chunkHead->size     = 0;
            shared_->chunkHead->next     = 0;
            shared_->ownBuffer           = true;
            shared_->refcount            = 1;
        }

        ~MemoryPoolAllocator() {
            if (!shared_) {
                return;
            }
            if (shared_->refcount > 1) {
                --shared_->refcount;
                return;
            }
            Clear();
            if (shared_->ownBuffer) { AllocatorFree(shared_); }
        }

        void* Malloc(size_t size) {
            assert(shared_->refcount > 0);
            if (!size) return NULL;

            size = ALIGN(size);
            if (UNLIKELY(shared_->chunkHead->size + size > shared_->chunkHead->capacity)) {
                if (!AddChunk(chunk_capacity_ > size ? chunk_capacity_ : size)) {
                    return NULL;
                }
            }

            void *buffer = GetChunkBuffer(shared_) + shared_->chunkHead->size;
            shared_->chunkHead->size += size;
            return buffer;
        }

      private:
        void Clear() {
            assert(shared_->refcount > 0);
            for (;;) {
                ChunkHeader *c = shared_->chunkHead;
                if (!c->next) { break; }
                shared_->chunkHead = c->next;
                AllocatorFree(c);
            }
            shared_->chunkHead->size = 0;
        }
        
        bool AddChunk(size_t capacity) {
            if (ChunkHeader* chunk = static_cast<ChunkHeader *>(AllocatorMalloc(SIZEOF_CHUNK_HEADER + capacity))) {
                chunk->capacity    = capacity;
                chunk->size        = 0;
                chunk->next        = shared_->chunkHead;
                shared_->chunkHead = chunk;
                return true;
            }
            return false;
        }
        
        static inline void AllocatorFree(void *ptr) {
            free(ptr);
        }
        static inline void* AllocatorMalloc(size_t size) {
            if (size) { return malloc(size); } return NULL;
        }
        
        static inline ChunkHeader *GetChunkHead(SharedData *shared) {
            return reinterpret_cast<ChunkHeader *>(reinterpret_cast<uint8_t *>(shared) + SIZEOF_SHARED_DATA);
        }
        static inline uint8_t *GetChunkBuffer(SharedData *shared) {
            return reinterpret_cast<uint8_t *>(shared->chunkHead) + SIZEOF_CHUNK_HEADER;
        }
    };

/*------------------------------------------------------------------------------*/

    GenericReader::GenericReader() : _alloc(new MemoryPoolAllocator), _cur(NULL) {
    }

    GenericReader::~GenericReader() {
        if (_alloc) {
            delete _alloc;
        }
    }

    int64_t GenericReader::convertInt(const char* value, uint32_t length) {
        if (!length) return 0;
        
        int64_t result = 0;
        bool bMinus = false;
        if (value[0] == '-') {
            bMinus = true;
        }
        for (uint32_t idx = bMinus; idx < length; ++idx) {
            result *= 10;
            result += value[idx] - '0';
        }
        if (bMinus) result = 0 - result;
        return result;
    }

    uint64_t GenericReader::convertUint(const char* value, uint32_t length) {
        if (!length) return 0;
        
        uint64_t result = 0;
        for (uint32_t idx = 0; idx < length; ++idx) {
            result *= 10;
            result += value[idx] - '0';
        }
        return result;
    }

    static inline double decimal(uint8_t n, uint32_t num) {
        double db = n * 1.0f;
        while (num--)
            db = db / 10;
        return db;
    }

    double GenericReader::convertDouble(const char* value, uint32_t length) {
        if (!length) return 0.0f;
        
        double result = 0.0;
        for (uint32_t idx = 0, bFlag = false, num = 0; idx < length; ++idx) {
            const char& c = value[idx];
            if (c == '.') {
                bFlag = true;
                continue;
            }
            uint8_t n = c - '0';
            if (!bFlag) {
                result *= 10;
                result += n;
            } else {
                ++num;
                result += decimal(n, num);
            }
        }
        return result;
    }

    const GenericValue* GenericReader::Parse(const char* src, uint32_t length) {
        GenericValue* root = (GenericValue*)_alloc->Malloc(sizeof(GenericValue));
        memset(root, 0, sizeof(GenericValue));
        _cur = root;
        
        StringStream is(src, length);
        if (is.Peek() != '\0') {
            ParseValue(is);
        }
        return root;
    }

    void GenericReader::ParseValue(StringStream& is) {
        switch (is.Peek()) {
            case 'n': ParseNull(is); break;
            case 't': ParseTrue(is); break;
            case 'f': ParseFalse(is); break;
            case '"': ParseString(is); break;
            case '{': { GenericValue* parent = _cur; ParseObject(is); _cur = parent; } break;
            case '[': { GenericValue* parent = _cur; ParseArray(is); _cur = parent; } break;
            default:
                ParseNumber(is);
                break;
        }
        if (_strError[0])
            return;
    }

    void GenericReader::ParseKey(StringStream& is) {
        assert(is.Peek() == '\"');
        is.Take();  // Skip '\"'
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '\"') {
                _cur->key = szStart;
                _cur->keySize = is.Strart() - szStart;
                is.Take();  // Skip '\"'
                return;
            }
        }
        setError("KeyInvalid");
    }

    void GenericReader::ParseNull(StringStream& is) {
        assert(is.Peek() == 'n');
        is.Take();

        if (Consume(is, 'u') && Consume(is, 'l') && Consume(is, 'l')) {
            _cur->type = VALUE_NULL;
            _cur->value = "null";
            _cur->valueSize = 4;
        } else {
            setError("ValueInvalid");
        }
    }

    void GenericReader::ParseTrue(StringStream& is) {
        assert(is.Peek() == 't');
        const char* szStart = is.Strart();
        is.Take();

        if (Consume(is, 'r') && Consume(is, 'u') && Consume(is, 'e')) {
            _cur->type = VALUE_BOOL;
            _cur->value = szStart;
            _cur->valueSize = is.Strart() - szStart;
        } else {
            setError("ValueInvalid");
        }
    }

    void GenericReader::ParseFalse(StringStream& is) {
        assert(is.Peek() == 'f');
        const char* szStart = is.Strart();
        is.Take();

        if (Consume(is, 'a') && Consume(is, 'l') && Consume(is, 's') && Consume(is, 'e')) {
            _cur->type = VALUE_BOOL;
            _cur->value = szStart;
            _cur->valueSize = is.Strart() - szStart;
        } else {
            setError("ValueInvalid");
        }
    }

    void GenericReader::ParseString(StringStream& is) {
        assert(is.Peek() == '\"');
        is.Take();  // Skip '\"'
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '\"' && is.Second2Last() != '\\') {
                _cur->type = VALUE_STRING;
                _cur->value = szStart;
                _cur->valueSize = is.Strart() - szStart;
                is.Take();  // Skip '\"'
                return;
            }
        }
        setError("ValueInvalid");
    }

    void GenericReader::ParseArray(StringStream& is) {
        assert(is.Peek() == '[');
        is.Take();  // Skip '['

        SkipWhitespace(is);
        if (Consume(is, ']')) {
            return;
        }

        _cur->type = VALUE_ARRAY;
        for (uint32_t elementCount = 0;;) {
            GenericValue* temp = _cur;
            _cur = (GenericValue*)_alloc->Malloc(sizeof(GenericValue));
            memset(_cur, 0, sizeof(GenericValue));
            _cur->prev = temp;
            if (!elementCount) {
                temp->child = _cur;
            } else {
                temp->next = _cur;
            }
            
            ParseValue(is);
            ++elementCount;
            SkipWhitespace(is);

            if (Consume(is, ',')) {
                SkipWhitespace(is);
            } else if (Consume(is, ']')) {
                return;
            } else {
                setError("ParseErrorArrayMissCommaOrSquareBracket");
                return;
            }
        }
        setError("ValueArrayInvalid");
    }

    void GenericReader::ParseNumber(StringStream& is) {
        const char* szStart = is.Strart();

        for (; is.Peek() != '\0'; is.Take()) {
            if (is.Peek() == '-' || is.Peek() == '.' || (is.Peek() >= '0' && is.Peek() <= '9')) {
                continue;
            } else {
                _cur->type = VALUE_NUMBER;
                _cur->value = szStart;
                _cur->valueSize = is.Strart() - szStart;
                return;
            }
        }
        setError("ValueInvalid");
    }

    void GenericReader::ParseObject(StringStream& is) {
        assert(is.Peek() == '{');
        const char* szStart = is.Strart();
        is.Take();  // Skip '{'

        SkipWhitespace(is);
        if (is.Peek() == '}') {
            is.Take();
            return;
        }

        _cur->type = VALUE_OBJECT;
        for (uint32_t memberCount = 0; !is.isEnd();) {
            if (is.Peek() != '"') {
                setError("ObjectMissName");
                return;
            }
            GenericValue* temp = _cur;
            _cur = (GenericValue*)_alloc->Malloc(sizeof(GenericValue));
            memset(_cur, 0, sizeof(GenericValue));
            _cur->prev = temp;
            if (!memberCount) {
                temp->child = _cur;
            } else {
                temp->next = _cur;
            }
            
            ParseKey(is);

            SkipWhitespace(is);
            if (!Consume(is, ':')) {
                setError("ObjectMissColon");
                return;
            }

            SkipWhitespace(is);
            ParseValue(is);
            ++memberCount;

            SkipWhitespace(is);
            switch (is.Peek()) {
                case ',':
                    is.Take();
                    SkipWhitespace(is);
                    break;
                case '}':
                    is.Take();
                    return;
                default:
                    setError("ObjectMissCommaOrCurlyBracket");
                    break; // This useless break is only for making warning and coverage happy
            }
        }
        _cur = _cur->prev;
    }

}
