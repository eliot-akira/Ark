inline ValueType Value::valueType() const
{
    return m_type;
}

inline bool Value::isFunction() const
{
    return m_type == ValueType::PageAddr || m_type == ValueType::Closure || m_type == ValueType::CProc;
}

// getters

#define GET_VAL(type, val) (*static_cast<type*>(val))

inline double Value::number() const
{
    return GET_VAL(double, m_value);
}

inline const std::string& Value::string() const
{
    return GET_VAL(std::string, m_value);
}

inline const std::vector<Value>& Value::const_list() const
{
    return GET_VAL(std::vector<Value>, m_value);
}

inline const UserType& Value::usertype() const
{
    return GET_VAL(UserType, m_value);
}

inline PageAddr_t Value::pageAddr() const
{
    return GET_VAL(PageAddr_t, m_value);
}

inline NFT Value::nft() const
{
    return GET_VAL(NFT, m_value);
}

inline const Value::ProcType& Value::proc() const
{
    return GET_VAL(ProcType, m_value);
}

inline const Closure& Value::closure() const
{
    return GET_VAL(Closure, m_value);
}

#undef GET_VAL

// end getters

inline bool operator==(const Value::ProcType& f, const Value::ProcType& g)
{
    return f.template target<Value (std::vector<Value>&)>() == g.template target<Value (std::vector<Value>&)>();
}

inline bool operator==(const Value& A, const Value& B)
{
    // values should have the same type
    if (A.m_type != B.m_type)
        return false;
    
    switch (A.m_type)
    {
        case ValueType::List:
            return A.const_list() == B.const_list();
        
        case ValueType::Number:
            return A.number() == B.number();
        
        case ValueType::String:
            return A.string() == B.string();
        
        case ValueType::PageAddr:
            return A.pageAddr() == B.pageAddr();
        
        case ValueType::NFT:
            return A.nft() == B.nft();
        
        case ValueType::CProc:
            return A.proc() == B.proc();
        
        case ValueType::Closure:
            return A.closure() == B.closure();
        
        case ValueType::User:
            return A.usertype() == B.usertype();
    }
}

inline bool operator<(const Value& A, const Value& B)
{
    if (A.m_type != B.m_type)
        return (static_cast<int>(A.m_type) - static_cast<int>(B.m_type)) < 0;
    
    switch (A.m_type)
    {
        case ValueType::List:
            return A.const_list() < B.const_list();
        
        case ValueType::Number:
            return A.number() < B.number();
        
        case ValueType::String:
            return A.string() < B.string();
        
        case ValueType::PageAddr:
            return A.pageAddr() < B.pageAddr();
        
        case ValueType::NFT:
            if (A.nft() == B.nft())
                return false;
            else if (B.nft() == NFT::True)
                return true;
            return false;
        
        case ValueType::CProc:
        case ValueType::Closure:
            return false;
        
        case ValueType::User:
            return A.usertype() < B.usertype();
    }
}

inline bool operator!=(const Value& A, const Value& B)
{
    return !(A == B);
}

inline bool operator!(const Value& A)
{
    switch (A.valueType())
    {
        case ValueType::List:
            return A.const_list().empty();
        
        case ValueType::Number:
            return !A.number();
        
        case ValueType::String:
            return A.string().empty();
        
        case ValueType::NFT:
        {
            if (A.nft() == NFT::True)
                return false;
            return true;
        }

        case ValueType::User:
            return A.usertype().not_();
        
        default:
            return false;
    }
}