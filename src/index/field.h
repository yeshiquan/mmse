#pragma once

#include <stdint.h>
#include <string>

namespace unise {

using DocId = uint64_t;
using TermId = uint64_t;
using FieldId = uint32_t;

enum FieldStoreType {YES, NO};

struct Field {
    Field(FieldId id, FieldStoreType t) 
            : field_id(id)
            , store_type(t) {}
    FieldId field_id {0};
    FieldStoreType store_type;
};

struct NumberField : public Field {
    NumberField(FieldId id, float v, FieldStoreType t) 
            : Field(id, t) {
        value.float_v = v;
    }
    NumberField(FieldId id, double v, FieldStoreType t) 
            : Field(id, t) {
        value.double_v = v;
    }
    NumberField(FieldId id, int32_t v, FieldStoreType t) 
            : Field(id, t) {
        value.int32_v = v;
    }
    NumberField(FieldId id, int64_t v, FieldStoreType t) 
            : Field(id, t) {
        value.int64_v = v;
    }            
    union {
        float float_v;
        double double_v;
        int32_t int32_v;
        int64_t int64_v;
    } value;
};

struct StringField : public Field {
    StringField(FieldId id, std::string v, FieldStoreType t) 
            : value(std::move(v))
            , Field(id, t) {}
    std::string value;
};

struct TextField : public Field {
    TextField(FieldId id, std::string v, FieldStoreType t) 
            : value(std::move(v))
            , Field(id, t) {}    
    std::string value;
};

// Token是term在文档中的1次出现
struct Token {
    TermId        term_id;
    DocId         docid;
    FieldId       field_id;
    uint32_t      pos;
    int32_t       score;
    std::string   value;
};

// 这个结构用于查询，查询最基础的粒度是term级别
struct Term {
    Term(std::string fn, std::string tv) 
            : field_name(fn)
            , term_value(tv) {}
    Term(Term&& other) 
            : field_name(std::move(other.field_name))
            , term_value(std::move(other.term_value))
            {} 
    std::string field_name;
    std::string term_value;
};

inline size_t gen_term_id(const std::string& term_str) {
    return std::hash<std::string>{}(term_str);
}

static constexpr DocId NO_MORE_DOCS = INT_MAX;

} // namespace