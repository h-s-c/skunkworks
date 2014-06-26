// http://nguillemot.blogspot.de/2012/06/side-story-compile-time-string-hashing.html

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <zeug/detail/stdfix.hpp>

namespace zeug
{

/* Compile time string hash */
class string_hash
{ 
  public:

    explicit constexpr string_hash(const char* str):
    hash(hashString(str))
    {
    }
    
    explicit constexpr string_hash(const std::int64_t hash):
    hash(hash)
    {
    }
    
    explicit constexpr string_hash(void* hash):
    hash(*(std::int64_t*)(hash))
    {
    }
    
    inline size_t Size() const
    {
        return sizeof(this->hash);
    }
    
    inline std::int64_t* Get()
    {
        return &this->hash;
    }
    
    inline std::int64_t operator=(const string_hash& other)  const
    {
        return other.hash;
    }
    
    inline bool operator==(const string_hash& other) const
    {
        return this->hash == other.hash;
    }
    
    inline bool operator!=(const string_hash& other) const
    {
        return this->hash != other.hash;
    }
    
    /*inline bool operator==(const std::int64_t& other) const
    {
        return this->hash == other;
    }
    
    inline bool operator==(void* other) const
    {
        return this->hash == *reinterpret_cast<std::int64_t*>(other);
    }*/
    
  private:        
    /* Performs a compile time recursive string hash using the djb2 algorithm explained here: http://www.cse.yorku.ca/~oz/hash.html*/
    static inline constexpr std::int64_t hashString(const char* str)
    {
        return ( !str ? 0 :
        hashStringRecursive(5381, str));
    }
    static inline constexpr std::int64_t hashStringRecursive(std::int64_t hash, const char* str)
    {
        return ( !*str ? hash :
        hashStringRecursive(((hash << 5) + hash) + *str, str + 1));
    }
    
    std::int64_t hash;
};
}
