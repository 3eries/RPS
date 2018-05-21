//
//  SBJSON.cpp
//
//  Created by sm on 15. 05. 13..
//
//

#include "SBJSON.h"

#include "../utils/SBStringUtils.h"

USING_NS_CC;
using namespace std;

string SBJSON::toJSONString(Value value) {
    
    Json::Value root = toJSONValue(value);
    Json::StyledWriter writer;
    
    return writer.write(root);
}

Json::Value SBJSON::toJSONValue(Value value) {
    
    Json::Value jsonValue;
    
    switch( value.getType() ) {
        case Value::Type::INTEGER:      jsonValue = Json::Value(value.asInt());             break;
        case Value::Type::UNSIGNED:     jsonValue = Json::Value(value.asUnsignedInt());     break;
        case Value::Type::FLOAT:        jsonValue = Json::Value(value.asFloat());           break;
        case Value::Type::DOUBLE:       jsonValue = Json::Value(value.asDouble());          break;
        case Value::Type::STRING:       jsonValue = Json::Value(value.asString());          break;
        case Value::Type::BOOLEAN:      jsonValue = Json::Value(value.asBool());            break;
        case Value::Type::VECTOR: {
            auto vector = value.asValueVector();
            for( auto v : vector ) {
                jsonValue.append(toJSONValue(v));
            }
            
        } break;
            
        case Value::Type::MAP: {
            auto map = value.asValueMap();
            for( auto it = map.begin(); it != map.end(); ++it ) {
                jsonValue[it->first] = toJSONValue(it->second);
            }
            
        } break;
            
        default: CCASSERT(false, "SBJSON toJSONValue error: not supported type.");
    }
    
    return jsonValue;
}

Value SBJSON::parse(const string &jsonStr) {
    
    Json::Value root;
    Json::Reader reader;
    
    bool parsingSuccessful = reader.parse(skip(jsonStr.c_str()), root);
    CCASSERT(parsingSuccessful, "SBJSON parse error.");
    
    return parse(root, true);
}

Value SBJSON::parse(Json::Value jsonValue, bool isJsonValue) {

    Value value;
    
    if( jsonValue.isNull() ) {
    
    } else if( jsonValue.isInt() ) {
        Json::LargestInt i = jsonValue.asLargestInt();
        
        if( i >= INT_MIN && i <= INT_MAX ) {
            value = (int)i;
        } else {
            value = StringUtils::format("%lli", i);
        }
        
    } else if( jsonValue.isUInt() ) {
        value = StringUtils::format("%llu", jsonValue.asLargestUInt());
        
    } else if( jsonValue.isArray() ) {
        ValueVector vector;
        
        for( int i = 0; i < jsonValue.size(); ++i ) {
            auto v = jsonValue[i];
            vector.push_back(parse(v, true));
        }
        
        value = vector;
        
    } else if( jsonValue.isObject() ) {
        ValueMap map;
        
        for( auto it = jsonValue.begin(); it != jsonValue.end(); ++it ) {
            string name = it.memberName();
            auto v = jsonValue.get(name, "");
            
            map.insert(make_pair(name, parse(v, true)));
        }
        
        value = map;
    }
    else if( jsonValue.isDouble() )   value = jsonValue.asDouble();
    else if( jsonValue.isString() )   value = jsonValue.asString();
    else if( jsonValue.isBool() )     value = jsonValue.asBool();
    else CCASSERT(false, "SBJSON parse error.");
    
    return value;
}

bool SBJSON::isJSON(const string &jsonStr) {
    
    Json::Value root;
    Json::Reader reader;
    
    return reader.parse(skip(jsonStr.c_str()), root);
}

const char* SBJSON::skip(const char* in) {
    
    while( in && *in && (unsigned char)*in <= 32 )
        in++;
    
    return in;
}

Json::LargestUInt SBJSON::getLargestUInt(Value value) {
    
    if( value.getType() == Value::Type::STRING ) {
        return SBStringUtils::toNumber<Json::LargestUInt>(value.asString());
    } else {
        return value.asInt();
    }
}
