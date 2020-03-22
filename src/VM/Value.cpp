#include <Ark/VM/Value.hpp>

#include <Ark/VM/Frame.hpp>
#include <Ark/Utils.hpp>

// using placement new to construct the type in the buffer
#define INIT_VAL(type, data) static_cast<void*>(new (&data) (type))
#define GET_VAL(type, val) (*static_cast<type*>(val))
#define DESTROY_VAL(type, val) (static_cast<type*>(val)->~type())

namespace Ark::internal
{
    Value::Value()
    {}

    Value::Value(const Value& other) :
        m_type(other.m_type), m_const(other.m_const)
    {
        switch (m_type)
        {
            case ValueType::List:
            {
                m_value = INIT_VAL(std::vector<Value>, m_data);
                GET_VAL(std::vector<Value>, m_value) = GET_VAL(std::vector<Value>, other.m_value);
                break;
            }

            case ValueType::Number:
            {
                m_value = INIT_VAL(double, m_data);
                GET_VAL(double, m_value) = GET_VAL(double, other.m_value);
                break;
            }

            case ValueType::String:
            {
                m_value = INIT_VAL(std::string, m_data);
                GET_VAL(std::string, m_value) = GET_VAL(std::string, other.m_value);
                break;
            }

            case ValueType::PageAddr:
            {
                m_value = INIT_VAL(PageAddr_t, m_data);
                GET_VAL(PageAddr_t, m_value) = GET_VAL(PageAddr_t, other.m_value);
                break;
            }

            case ValueType::NFT:
            {
                m_value = INIT_VAL(NFT, m_data);
                GET_VAL(NFT, m_value) = GET_VAL(NFT, other.m_value);
                break;
            }

            case ValueType::CProc:
            {
                m_value = INIT_VAL(Value::ProcType, m_data);
                GET_VAL(Value::ProcType, m_value) = GET_VAL(Value::ProcType, other.m_value);
                break;
            }

            case ValueType::Closure:
            {
                m_value = INIT_VAL(Closure, m_data);
                GET_VAL(Closure, m_value) = GET_VAL(Closure, other.m_value);
                break;
            }

            case ValueType::User:
            {
                m_value = INIT_VAL(UserType, m_data);
                GET_VAL(UserType, m_value) = GET_VAL(UserType, other.m_value);
                break;
            }

            default:
                break;
        }
    }

    Value::~Value()
    {
        if (m_value != nullptr)
        {
            // because destructors are called like so: ptr->~type()
            // so we can't write ptr->~std::vector<...>(), it wouldn't work
            using namespace std;

            switch (m_type)
            {
                case ValueType::List:
                    DESTROY_VAL(vector<Value>, m_value);
                    break;
                
                case ValueType::String:
                    DESTROY_VAL(string, m_value);
                    break;
                
                case ValueType::CProc:
                    // function<Value (std::vector<Value>&)> == ProcType
                    DESTROY_VAL(function<Value (vector<Value>&)>, m_value);
                    break;
                
                case ValueType::Closure:
                    DESTROY_VAL(Closure, m_value);
                    break;
                
                case ValueType::User:
                    DESTROY_VAL(UserType, m_value);
                    break;
                
                default:
                    // no need to destroy Number (double), NFT (enum class),
                    // PageAddr (uint16_t)
                    break;
            }
        }
    }

    // --------------------------

    Value::Value(ValueType type) :
        m_type(type), m_const(false)
    {
        if (m_type == ValueType::List)
            m_value = INIT_VAL(std::vector<Value>, m_data);
    }

    Value::Value(int value) :
        m_value(INIT_VAL(double, m_data)), m_type(ValueType::Number), m_const(false)
    {
        *static_cast<double*>(m_value) = static_cast<double>(value);
    }

    Value::Value(float value) :
        m_value(INIT_VAL(double, m_data)), m_type(ValueType::Number), m_const(false)
    {
        *static_cast<double*>(m_value) = static_cast<double>(value);
    }

    Value::Value(double value) :
        m_value(INIT_VAL(double, m_data)), m_type(ValueType::Number), m_const(false)
    {
        *static_cast<double*>(m_value) = static_cast<double>(value);
    }

    Value::Value(const std::string& value) :
        m_value(INIT_VAL(std::string, m_data)), m_type(ValueType::String), m_const(false)
    {
        *static_cast<std::string*>(m_value) = value;
    }

    Value::Value(std::string&& value) :
        m_value(INIT_VAL(std::string, m_data)), m_type(ValueType::String), m_const(false)
    {
        *static_cast<std::string*>(m_value) = std::move(value);
    }

    Value::Value(PageAddr_t value) :
        m_value(INIT_VAL(PageAddr_t, m_data)), m_type(ValueType::PageAddr), m_const(false)
    {
        *static_cast<PageAddr_t*>(m_value) = value;
    }

    Value::Value(NFT value) :
        m_value(INIT_VAL(NFT, m_data)), m_type(ValueType::NFT), m_const(false)
    {
        *static_cast<NFT*>(m_value) = value;
    }

    Value::Value(Value::ProcType value) :
        m_value(INIT_VAL(Value::ProcType, m_data)), m_type(ValueType::CProc), m_const(false)
    {
        *static_cast<Value::ProcType*>(m_value) = std::move(value);
    }

    Value::Value(std::vector<Value>&& value) :
        m_value(INIT_VAL(std::vector<Value>, m_data)), m_type(ValueType::List), m_const(false)
    {
        *static_cast<std::vector<Value>*>(m_value) = std::move(value);
    }

    Value::Value(Closure&& value) :
        m_value(INIT_VAL(Closure, m_data)), m_type(ValueType::Closure), m_const(false)
    {
        *static_cast<Closure*>(m_value) = std::move(value);
    }

    Value::Value(UserType&& value) :
        m_value(INIT_VAL(UserType, m_data)), m_type(ValueType::User), m_const(false)
    {
        *static_cast<UserType*>(m_value) = std::move(value);
    }

    // --------------------------

    std::vector<Value>& Value::list()
    {
        return GET_VAL(std::vector<Value>, m_value);
    }

    Closure& Value::closure_ref()
    {
        return GET_VAL(Closure, m_value);
    }

    std::string& Value::string_ref()
    {
        return GET_VAL(std::string, m_value);
    }

    UserType& Value::usertype_ref()
    {
        return GET_VAL(UserType, m_value);
    }

    // --------------------------

    void Value::push_back(const Value& value)
    {
        list().push_back(value);
    }

    void Value::push_back(Value&& value)
    {
        list().push_back(std::move(value));
    }

    // --------------------------

    void Value::registerVM(Ark::VM_t<false>* vm)
    {
        m_vmf = vm;
    }

    void Value::registerVM(Ark::VM_t<true>* vm)
    {
        m_vmt = vm;
    }

    // --------------------------

    // needed to calculate the precision of numbers when displaying them
    // otherwise, cout will choose to display about 2 decimals instead of every single decimal
    int dec_places(double d)
    {
        constexpr double precision = 1e-7;
        double temp = 0.0;
        int decimal_places = 0;

        do
        {
            d *= 10;
            temp = d - static_cast<int>(d);
            decimal_places++;
        } while(temp > precision && decimal_places < std::numeric_limits<double>::digits10);
        
        return decimal_places;
    }

    int dig_places(double d)
    {
        int digit_places = 0;
        int i = static_cast<int>(d);
        while (i != 0)
        {
            digit_places++;
            i /= 10;
        }
        return digit_places;
    }

    std::ostream& operator<<(std::ostream& os, const Value& V)
    {
        switch (V.valueType())
        {
        case ValueType::Number:
        {
            double d = V.number();
            os.precision(dig_places(d) + dec_places(d));
            os << d;
            break;
        }
        
        case ValueType::String:
            os << V.string();
            break;
        
        case ValueType::PageAddr:
            os << "Function @ " << V.pageAddr();
            break;
        
        case ValueType::NFT:
        {
            NFT nft = V.nft();
            if (nft == NFT::Nil)
                os << "nil";
            else if (nft == NFT::False)
                os << "false";
            else if (nft == NFT::True)
                os << "true";
            else if (nft == NFT::Undefined)
                os << "undefined";
            break;
        }

        case ValueType::CProc:
            os << "CProcedure";
            break;
        
        case ValueType::List:
        {
            os << "[";
            for (std::size_t index = 0; index < V.const_list().size(); ++index)
            {
                auto& t = V.const_list()[index];
                if (t.valueType() == ValueType::String)
                    os << "\"" << t << "\"";
                else
                    os << t;
                if (index + 1 != V.const_list().size())
                    os << " ";
            }
            os << "]";
            break;
        }

        case ValueType::Closure:
            os << V.closure();
            break;
        
        case ValueType::User:
            os << V.usertype();
            break;
        
        default:
            os << "~\\._./~";
            break;
        }

        return os;
    }
}